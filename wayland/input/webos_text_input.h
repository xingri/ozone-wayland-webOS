// Copyright 2014 LG Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OZONE_WAYLAND_TEXT_INPUT_H_
#define OZONE_WAYLAND_TEXT_INPUT_H_

#include "ozone/wayland/display.h"
#include "wayland-text-client-protocol.h"

namespace ozonewayland {

class WaylandWindow;
class WaylandInputDevice;

class WaylandTextInput {
 public:
  explicit WaylandTextInput(WaylandInputDevice* inputDevice);
  ~WaylandTextInput();
  void ResetIme();
  void ShowInputPanel(wl_seat* input_seat);
  void HideInputPanel(wl_seat* input_seat);
  void SetActiveWindow(WaylandWindow* window);
  WaylandInputDevice* getInputDevice() { return input_device_; }

  static void OnCommitString(void* data,
                             struct text_model* text_input,
                             uint32_t serial,
                             const char* text);

  static void OnPreeditString(void* data,
                             struct text_model* text_input,
                             uint32_t serial,
                             const char* text,
                             const char* commit);

  static void OnDeleteSurroundingText(void* data,
                             struct text_model* text_input,
                             uint32_t serial,
                             int32_t index,
                             uint32_t length);

  static void OnCursorPosition(void* data,
                             struct text_model* text_input,
                             uint32_t serial,
                             int32_t index,
                             int32_t anchor);

  static void OnPreeditStyling(void* data,
                             struct text_model* text_input,
                             uint32_t serial,
                             uint32_t index,
                             uint32_t length,
                             uint32_t style);

  static void OnPreeditCursor(void* data,
                            struct text_model* text_input,
                            uint32_t serial,
                            int32_t index);

  static void OnModifiersMap(void* data,
                            struct text_model* text_input,
                            struct wl_array* map);

  static void OnKeysym(void* data,
                       struct text_model* text_input,
                       uint32_t serial,
                       uint32_t time,
                       uint32_t key,
                       uint32_t state,
                       uint32_t modifiers);

  static void OnEnter(void* data,
                      struct text_model* text_input,
                      struct wl_surface* surface);

  static void OnLeave(void* data,
                      struct text_model* text_input);

  static void OnInputPanelState(void* data,
                      struct text_model* text_input,
                      uint32_t state);

  static void OnTextModelInputPanelRect(void *data,
                      struct text_model *text_input,
                      int32_t x,
                      int32_t y,
                      uint32_t width,
                      uint32_t height);
private:
  struct text_model* text_model_;
  bool is_visible_;
  WaylandWindow* active_window_;
  WaylandWindow* last_active_window_;
  WaylandInputDevice* input_device_;

  DISALLOW_COPY_AND_ASSIGN(WaylandTextInput);
};

} // namespace ozonewayland

#endif  // OZONE_WAYLAND_TEXT_INPUT_H_