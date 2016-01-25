// Copyright 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone/ui/events/remote_event_dispatcher.h"

#include "base/bind.h"
#include "ozone/ui/public/messages.h"

namespace ui {

RemoteEventDispatcher::RemoteEventDispatcher()
    : EventConverterOzoneWayland(),
      sender_(NULL) {
}

RemoteEventDispatcher::~RemoteEventDispatcher() {
}

void RemoteEventDispatcher::ChannelEstablished(IPC::Sender* sender) {
  loop_ = base::MessageLoop::current();
  sender_ = sender;
}

void RemoteEventDispatcher::MotionNotify(float x, float y) {
  Dispatch(new WaylandInput_MotionNotify(x, y));
}

void RemoteEventDispatcher::ButtonNotify(unsigned handle,
                                         ui::EventType type,
                                         ui::EventFlags flags,
                                         float x,
                                         float y) {
  Dispatch(new WaylandInput_ButtonNotify(handle, type, flags, x, y));
}

void RemoteEventDispatcher::AxisNotify(float x,
                                       float y,
                                       int xoffset,
                                       int yoffset) {
  Dispatch(new WaylandInput_AxisNotify(x, y, xoffset, yoffset));
}

void RemoteEventDispatcher::PointerEnter(unsigned handle,
                                         float x,
                                         float y) {
  Dispatch(new WaylandInput_PointerEnter(handle, x, y));
}

void RemoteEventDispatcher::PointerLeave(unsigned handle,
                                         float x,
                                         float y) {
  Dispatch(new WaylandInput_PointerLeave(handle, x, y));
}

void RemoteEventDispatcher::KeyNotify(ui::EventType type,
                                      unsigned code,
                                      unsigned modifiers) {
  Dispatch(new WaylandInput_KeyNotify(type, code, modifiers));
}

void RemoteEventDispatcher::TouchNotify(ui::EventType type,
                                        float x,
                                        float y,
                                        int32_t touch_id,
                                        uint32_t time_stamp) {
  Dispatch(new WaylandInput_TouchNotify(type, x, y, touch_id, time_stamp));
}

void RemoteEventDispatcher::OutputSizeChanged(unsigned width,
                                              unsigned height) {
  Dispatch(new WaylandInput_OutputSize(width, height));
}

void RemoteEventDispatcher::WindowResized(unsigned handle,
                                          unsigned width,
                                          unsigned height) {
  Dispatch(new WaylandWindow_Resized(handle, width, height));
}

void RemoteEventDispatcher::CloseWidget(unsigned handle) {
  Dispatch(new WaylandInput_CloseWidget(handle));
}

void RemoteEventDispatcher::Commit(unsigned handle,
                                   const std::string& text) {
  Dispatch(new WaylandInput_Commit(handle, text));
}

void RemoteEventDispatcher::PreeditChanged(unsigned handle,
                                           const std::string& text,
                                           const std::string& commit) {
  Dispatch(new WaylandInput_PreeditChanged(handle, text, commit));
}

void RemoteEventDispatcher::PreeditEnd() {
  Dispatch(new WaylandInput_PreeditEnd());
}

void RemoteEventDispatcher::PreeditStart() {
  Dispatch(new WaylandInput_PreeditStart());
}

#if defined(WEBOS)
void RemoteEventDispatcher::CloseWindow(unsigned handle) {
  Dispatch(new WaylandWindow_CloseWindow(handle));
}
#endif

void RemoteEventDispatcher::Dispatch(IPC::Message* message) {
    ui::EventConverterOzoneWayland::PostTaskOnMainLoop(
          base::Bind(&RemoteEventDispatcher::Send, this, message));
}

void RemoteEventDispatcher::Send(RemoteEventDispatcher* dispatcher,
                                 IPC::Message* message) {
  dispatcher->sender_->Send(message);
}

}  // namespace ui
