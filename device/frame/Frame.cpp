// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Frame.h"
// std
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

#include <scene/World.h>


#include <vtkm/rendering/Actor.h>
#include <vtkm/rendering/CanvasRayTracer.h>
#include <vtkm/rendering/MapperRayTracer.h>
#include <vtkm/rendering/MapperVolume.h>
#include <vtkm/rendering/MapperWireframer.h>
#include <vtkm/rendering/Scene.h>
#include <vtkm/rendering/View3D.h>

using vtkm::rendering::CanvasRayTracer;
using vtkm::rendering::MapperRayTracer;
using vtkm::rendering::MapperVolume;
using vtkm::rendering::MapperWireframer;

namespace vtkm_device {

// Helper functions ///////////////////////////////////////////////////////////

static uint32_t cvt_uint32(const float &f)
{
  return static_cast<uint32_t>(255.f * std::clamp(f, 0.f, 1.f));
}

static uint32_t cvt_uint32(const float4 &v)
{
  return (cvt_uint32(v[0]) << 0) | (cvt_uint32(v[1]) << 8)
      | (cvt_uint32(v[2]) << 16) | (cvt_uint32(v[3]) << 24);
}

static uint32_t cvt_uint32_srgb(const float4 &v)
{
  return cvt_uint32(float4(toneMap(v[0]), toneMap(v[1]), toneMap(v[2]), v[3]));
}

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
  //m_frameData.invSize = 1.f / float2(m_frameData.size);
  m_frameData.invSize[0] = 1.f / (float)m_frameData.size[0];
  m_frameData.invSize[1] = 1.f / (float)m_frameData.size[1];

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
    } else {

      auto instances = this->m_world->instances();
      auto camera = this->m_camera->GetCamera();

      std::cout<<"\n\nANARI camera:"<<std::endl;
      camera.Print();

      vtkm::rendering::Camera _camera;
      _camera.SetLookAt(vtkm::Vec3f_32(0.5, 0.5, 0.5));
      _camera.SetViewUp(vtkm::make_Vec(0.f, 1.f, 0.f));
      _camera.SetClippingRange(1.f, 10.f);
      _camera.SetFieldOfView(60.f);
      _camera.SetPosition(vtkm::Vec3f_32(1.5, 1.5, 1.5));

      std::cout<<"Correct camera:"<<std::endl;
      _camera.Print();

      /*
            camera.SetPosition(_camera.GetPosition());
            camera.SetLookAt(_camera.GetLookAt());
            camera.SetViewUp(_camera.GetViewUp());
            camera.SetFieldOfView(_camera.GetFieldOfView());
            camera.SetClippingRange(_camera.GetClippingRange());
            vtkm::Bounds vp = _camera.GetViewport();
            camera.SetViewport(vp.X.Min, vp.X.Max, vp.Y.Min, vp.Y.Max);
      */

      // This should be a loop over volumes and surfaces.
      if ((instances.size() > 0) &&
          (instances[0]->group()->volumes().size() > 0))
      {
        vtkm::cont::ColorTable colorTable("inferno");
        // Background color:
        vtkm::rendering::Color bg(0.2f, 0.2f, 0.2f, 1.0f);
        vtkm::rendering::Actor actor = *instances[0]->group()->volumes()[0]->actor();
        vtkm::rendering::Scene scene;
        scene.AddActor(actor);

        CanvasRayTracer canvas(m_frameData.size[0], m_frameData.size[1]);
        vtkm::rendering::View3D view(scene, *instances[0]->group()->volumes()[0]->mapper(), canvas, camera, bg);
        view.SetWorldAnnotationsEnabled(false);
        view.SetRenderAnnotationsEnabled(false);
        view.Paint();
        auto colorBuff = canvas.GetColorBuffer();
        auto zBuff = canvas.GetDepthBuffer();
        auto colorPortal = colorBuff.ReadPortal();
        auto zPortal = zBuff.ReadPortal();
        //view.SaveAs("volume.png");
        int idx = 0;
        for (int y = 0; y < m_frameData.size[1]; y++)
          for (int x = 0; x < m_frameData.size[0]; x++)
          {
            auto color = colorPortal.Get(idx);
            PixelSample pixel;
            pixel.color = {color[0], color[1], color[2], color[3]};
            pixel.depth = zPortal.Get(idx);
            this->writeSample(x,y, pixel);

            idx++;
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

  *width = m_frameData.size[0];
  *height = m_frameData.size[1];

  if (channel == "channel.color") {
    *pixelType = m_colorType;
    return m_pixelBuffer.data();
  } else if (channel == "channel.depth" && !m_depthBuffer.empty()) {
    *pixelType = ANARI_FLOAT32;
    return m_depthBuffer.data();
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
  // no-op
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
  // no-op
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

float2 Frame::screenFromPixel(const float2 &p) const
{
  return p * m_frameData.invSize;
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
