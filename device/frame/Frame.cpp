// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Frame.h"
// std
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

#include <scene/World.h>

#include <vtkm/cont/ArrayHandleBasic.h>
#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/MapperVolume.h>
#include <vtkm/rendering/MapperWireframer.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>

#include <vtkm/cont/Invoker.h>
#include <vtkm/worklet/WorkletMapField.h>

using vtkm::rendering::CanvasRayTracer;
using vtkm::rendering::MapperRayTracer;
using vtkm::rendering::MapperVolume;
using vtkm::rendering::MapperWireframer;

namespace vtkm_device {

constexpr float toneMap(vtkm::Float32 v)
{
  return std::pow(v, 1.f / 2.2f);
}

static uint32_t cvt_uint32(const vtkm::Float32 &f)
{
  return static_cast<uint32_t>(255.f * std::clamp(f, 0.f, 1.f));
}

static uint32_t cvt_uint32(const vtkm::Vec4f_32 &v)
{
  return (cvt_uint32(v[0]) << 0) | (cvt_uint32(v[1]) << 8)
      | (cvt_uint32(v[2]) << 16) | (cvt_uint32(v[3]) << 24);
}

static uint32_t cvt_uint32_srgb(const vtkm::Vec4f_32 &v)
{
  return cvt_uint32(
      vtkm::Vec4f_32(toneMap(v[0]), toneMap(v[1]), toneMap(v[2]), v[3]));
}

class ConvertToRGBA : public vtkm::worklet::WorkletMapField
{
 public:
  using ControlSignature = void(FieldIn inputArray, FieldOut outputArray);
  using ExecutionSignature = void(InputIndex, _1, _2);
  using InputDomain = _1;

  template <typename InFieldType, typename OutFieldType>
  VTKM_EXEC void operator()(const vtkm::Id &inIdx,
      const InFieldType &inField,
      OutFieldType &outField) const
  {
    outField = cvt_uint32(inField);
  }
};

class ConvertToSRGBA : public vtkm::worklet::WorkletMapField
{
 public:
  using ControlSignature = void(FieldIn inputArray, FieldOut outputArray);
  using ExecutionSignature = void(InputIndex, _1, _2);
  using InputDomain = _1;

  template <typename InFieldType, typename OutFieldType>
  VTKM_EXEC void operator()(const vtkm::Id &inIdx,
      const InFieldType &inField,
      OutFieldType &outField) const
  {
    outField = cvt_uint32_srgb(inField);
  }

 private:
  vtkm::Float32 Exponent = 1.1f / 2.2f;
};

// Helper functions ///////////////////////////////////////////////////////////

template <typename R, typename TASK_T>
static std::future<R> async(TASK_T &&fcn)
{
  auto task = std::packaged_task<R()>(std::forward<TASK_T>(fcn));
  auto future = task.get_future();

  std::thread([task = std::move(task)]() mutable { task(); }).detach();

  return future;
}

template <typename R>
static bool is_ready(const std::future<R> &f)
{
  return !f.valid()
      || f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

// Frame definitions //////////////////////////////////////////////////////////

Frame::Frame(VTKmDeviceGlobalState *s) : helium::BaseFrame(s)
{
  s->objectCounts.frames++;
}

Frame::~Frame()
{
  wait();
  deviceState()->objectCounts.frames--;
}

bool Frame::isValid() const
{
  return m_valid;
}

VTKmDeviceGlobalState *Frame::deviceState() const
{
  return (VTKmDeviceGlobalState *)helium::BaseObject::m_state;
}

void Frame::commit()
{
  m_renderer = getParamObject<Renderer>("renderer");
  if (!m_renderer) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'renderer' on frame");
  }

  m_camera = getParamObject<Camera>("camera");
  if (!m_camera) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'camera' on frame");
  }

  m_world = getParamObject<World>("world");
  if (!m_world) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "missing required parameter 'world' on frame");
  }

  m_valid = m_renderer && m_renderer->isValid() && m_camera
      && m_camera->isValid() && m_world && m_world->isValid();

  m_colorType = getParam<anari::DataType>("channel.color", ANARI_UNKNOWN);
  m_depthType = getParam<anari::DataType>("channel.depth", ANARI_UNKNOWN);
  m_primIdType =
      getParam<anari::DataType>("channel.primitiveId", ANARI_UNKNOWN);
  m_objIdType = getParam<anari::DataType>("channel.objectId", ANARI_UNKNOWN);
  m_instIdType = getParam<anari::DataType>("channel.instanceId", ANARI_UNKNOWN);

  m_frameData.size = getParam<uint2>("size", uint2(10));
  this->Canvas = vtkm::rendering::CanvasRayTracer(
      this->m_frameData.size[0], this->m_frameData.size[1]);

  const auto numPixels = m_frameData.size[0] * m_frameData.size[1];

  m_perPixelBytes = 4 * (m_colorType == ANARI_FLOAT32_VEC4 ? 4 : 1);
  m_pixelBuffer.resize(numPixels * m_perPixelBytes);

  m_depthBuffer.resize(m_depthType == ANARI_FLOAT32 ? numPixels : 0);
  m_frameChanged = true;

  m_primIdBuffer.clear();
  m_objIdBuffer.clear();
  m_instIdBuffer.clear();

  if (m_primIdType == ANARI_UINT32)
    m_primIdBuffer.resize(numPixels);
  if (m_objIdType == ANARI_UINT32)
    m_objIdBuffer.resize(numPixels);
  if (m_instIdType == ANARI_UINT32)
    m_instIdBuffer.resize(numPixels);
}

bool Frame::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (type == ANARI_FLOAT32 && name == "duration") {
    helium::writeToVoidP(ptr, m_duration);
    return true;
  }

  return 0;
}

void Frame::renderFrame()
{
  this->refInc(helium::RefType::INTERNAL);

  auto *state = deviceState();
  state->waitOnCurrentFrame();
  state->currentFrame = this;

  m_future = async<void>([&, state]() {
    auto start = std::chrono::steady_clock::now();
    state->renderingSemaphore.frameStart();
    state->commitBufferFlush();

    if (!m_renderer) {
      reportMessage(
          ANARI_SEVERITY_ERROR, "skipping render of incomplete frame object");
      std::fill(m_pixelBuffer.begin(), m_pixelBuffer.end(), 0);
    }
    else
    {
      const auto &instances = this->m_world->instances();
      auto camera = this->m_camera->camera(this->m_world->bounds());

#if 0
      std::cout << "\n\nANARI camera:" << std::endl;
      camera.Print();
#endif

      for (const auto& instance : instances)
      {
        if (instance->group() == nullptr)
          continue;

        for (const auto& volume : instance->group()->volumes())
        {
          const auto actor = volume->actor();
          const auto mapper = volume->mapper();
          vtkm::rendering::Scene scene;
          scene.AddActor(*actor);

          vtkm::rendering::View3D view(scene,
              *mapper,
              this->Canvas,
              camera,
              this->m_renderer->background());
          view.SetWorldAnnotationsEnabled(false);
          view.SetRenderAnnotationsEnabled(false);
          view.Paint();
        }

        for (const auto& surface : instance->group()->surfaces())
        {
          std::cout<<"Render Surface"<<std::endl;
          const auto geom = surface->geometry();
          const auto actor = geom->actor();
          const auto mapper = geom->mapper();
          vtkm::rendering::Scene scene;
          scene.AddActor(*actor);

          vtkm::rendering::View3D view(scene,
              *mapper,
              this->Canvas,
              camera,
              this->m_renderer->background());
          view.SetWorldAnnotationsEnabled(false);
          view.SetRenderAnnotationsEnabled(false);
          view.Paint();
        }
      }
    }

    state->renderingSemaphore.frameEnd();
    auto end = std::chrono::steady_clock::now();
    m_duration = std::chrono::duration<float>(end - start).count();
  });
}

void *Frame::map(std::string_view channel,
    uint32_t *width,
    uint32_t *height,
    ANARIDataType *pixelType)
{
  wait();

  // types are in: anari enum
  // report message method to convey info and faults.

  *width = m_frameData.size[0];
  *height = m_frameData.size[1];

  if (channel == "channel.color") {
    *pixelType = this->m_colorType;
    if (this->m_colorType == ANARI_FLOAT32_VEC4) {
      // change this to GetReadPointer().
      vtkm::cont::ArrayHandleBasic<vtkm::Vec4f> basicArray =
          this->Canvas.GetColorBuffer();
      return basicArray.GetWritePointer();
    } else if (this->m_colorType == ANARI_UFIXED8_VEC4) {
      this->m_intFrameBuffer.Allocate(*width * *height);
      ConvertToRGBA worklet;
      vtkm::cont::Invoker invoker;
      invoker(worklet, this->Canvas.GetColorBuffer(), this->m_intFrameBuffer);
      vtkm::cont::ArrayHandleBasic<vtkm::UInt32> basicArray =
          this->m_intFrameBuffer;
      return basicArray.GetWritePointer();
    } else if (this->m_colorType == ANARI_UFIXED8_RGBA_SRGB) {
      this->m_intFrameBuffer.Allocate(*width * *height);
      ConvertToSRGBA worklet;
      vtkm::cont::Invoker invoker;
      invoker(worklet, this->Canvas.GetColorBuffer(), this->m_intFrameBuffer);

      vtkm::cont::ArrayHandleBasic<vtkm::UInt32> basicArray =
          this->m_intFrameBuffer;
      return basicArray.GetWritePointer();
    }
    return nullptr;
  } else if (channel == "channel.depth") {
    *pixelType = ANARI_FLOAT32;
    vtkm::cont::ArrayHandleBasic<vtkm::Float32> basicArray =
        this->Canvas.GetDepthBuffer();
    return (void *)basicArray.GetWritePointer();
  } else if (channel == "channel.primitiveId" && !m_primIdBuffer.empty()) {
    *pixelType = ANARI_UINT32;
    return m_primIdBuffer.data();
  } else if (channel == "channel.objectId" && !m_objIdBuffer.empty()) {
    *pixelType = ANARI_UINT32;
    return m_objIdBuffer.data();
  } else if (channel == "channel.instanceId" && !m_instIdBuffer.empty()) {
    *pixelType = ANARI_UINT32;
    return m_instIdBuffer.data();
  } else {
    *width = 0;
    *height = 0;
    *pixelType = ANARI_UNKNOWN;
    return nullptr;
  }
}

void Frame::unmap(std::string_view channel)
{
  // if (channel == "channel.color" &&
  // this->m_bytesFrameBuffer.GetNumberOfValues() > 0)
}

int Frame::frameReady(ANARIWaitMask m)
{
  if (m == ANARI_NO_WAIT)
    return ready();
  else {
    wait();
    return 1;
  }
}

void Frame::discard()
{
  this->m_intFrameBuffer.ReleaseResources();
}

bool Frame::ready() const
{
  return is_ready(m_future);
}

void Frame::wait() const
{
  if (m_future.valid()) {
    m_future.get();
    this->refDec(helium::RefType::INTERNAL);
    if (deviceState()->currentFrame == this)
      deviceState()->currentFrame = nullptr;
  }
}

void Frame::writeSample(int x, int y, const PixelSample &s)
{
  const auto idx = y * m_frameData.size[0] + x;
  auto *color = m_pixelBuffer.data() + (idx * m_perPixelBytes);
  switch (m_colorType) {
  case ANARI_UFIXED8_VEC4: {
    auto c = cvt_uint32(s.color);
    std::memcpy(color, &c, sizeof(c));
    break;
  }
  case ANARI_UFIXED8_RGBA_SRGB: {
    auto c = cvt_uint32_srgb(s.color);
    std::memcpy(color, &c, sizeof(c));
    break;
  }
  case ANARI_FLOAT32_VEC4: {
    std::memcpy(color, &s.color, sizeof(s.color));
    break;
  }
  default:
    break;
  }
  if (!m_depthBuffer.empty())
    m_depthBuffer[idx] = s.depth;
  if (!m_primIdBuffer.empty())
    m_primIdBuffer[idx] = s.primId;
  if (!m_objIdBuffer.empty())
    m_objIdBuffer[idx] = s.objId;
  if (!m_instIdBuffer.empty())
    m_instIdBuffer[idx] = s.instId;
}

} // namespace vtkm_device

VTKM_ANARI_TYPEFOR_DEFINITION(vtkm_device::Frame *);
