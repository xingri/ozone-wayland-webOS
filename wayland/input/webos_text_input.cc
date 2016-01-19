// Copyright 2014 LG Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone/wayland/input/webos_text_input.h"

#include <string>

#include "ozone/ui/events/event_factory_ozone_wayland.h"
#include "ozone/ui/events/keyboard_codes_ozone.h"
#include "ozone/wayland/display.h"
#include "ozone/wayland/input/keyboard.h"
#include "ozone/wayland/input/keyboard_engine_xkb.h"
#include "ozone/wayland/input_device.h"
#include "ozone/wayland/shell/shell_surface.h"
#include "ozone/wayland/window.h"

namespace ozonewayland {

const struct text_model_listener text_model_listener_ = {
  WaylandTextInput::OnCommitString,
  WaylandTextInput::OnPreeditString,
  WaylandTextInput::OnDeleteSurroundingText,
  WaylandTextInput::OnCursorPosition,
  WaylandTextInput::OnPreeditStyling,
  WaylandTextInput::OnPreeditCursor,
  WaylandTextInput::OnModifiersMap,
  WaylandTextInput::OnKeysym,
  WaylandTextInput::OnEnter,
  WaylandTextInput::OnLeave,
  WaylandTextInput::OnInputPanelState,
  WaylandTextInput::OnTextModelInputPanelRect
};

WaylandTextInput::WaylandTextInput(WaylandInputDevice* inputDevice):
  text_model_(NULL), is_visible_(false), active_window_(NULL), last_active_window_(NULL),
  input_device_(inputDevice) {
}

WaylandTextInput::~WaylandTextInput() {
  if (text_model_)
    text_model_destroy(text_model_);
}

void WaylandTextInput::SetActiveWindow(WaylandWindow* window) {
  active_window_ = window;
  if (active_window_)
    last_active_window_ = active_window_;
}

void WaylandTextInput::OnCommitString(void* data,
                                      struct text_model* text_input,
                                      uint32_t serial,
                                      const char* text) {
  ui::EventConverterOzoneWayland* dispatcher =
        ui::EventFactoryOzoneWayland::GetInstance()->EventConverter();
  DCHECK(static_cast<WaylandTextInput*>(data)->last_active_window_);
  dispatcher->Commit(static_cast<WaylandTextInput*>(data)->
      last_active_window_->Handle(), std::string(text));
}

void WaylandTextInput::OnPreeditString(void* data,
                                       struct text_model* text_input,
                                       uint32_t serial,
                                       const char* text,
                                       const char* commit) {
  ui::EventConverterOzoneWayland* dispatcher =
         ui::EventFactoryOzoneWayland::GetInstance()->EventConverter();
  DCHECK(static_cast<WaylandTextInput*>(data)->last_active_window_);
  dispatcher->PreeditChanged(static_cast<WaylandTextInput*>(data)->
     last_active_window_->Handle(), std::string(text), std::string(commit));
}

void WaylandTextInput::OnDeleteSurroundingText(void* data,
                                       struct text_model* text_input,
                                       uint32_t serial,
                                       int32_t index,
                                       uint32_t length) {
}

void WaylandTextInput::OnCursorPosition(void* data,
                                       struct text_model* text_input,
                                       uint32_t serial,
                                       int32_t index,
                                       int32_t anchor) {
}

void WaylandTextInput::OnPreeditStyling(void* data,
                                       struct text_model* text_input,
                                       uint32_t serial,
                                       uint32_t index,
                                       uint32_t length,
                                       uint32_t style) {
}

void WaylandTextInput::OnPreeditCursor(void* data,
                                       struct text_model* text_input,
                                       uint32_t serial,
                                       int32_t index) {
}

void WaylandTextInput::OnModifiersMap(void* data,
                                      struct text_model* text_input,
                                      struct wl_array* map) {
}

void WaylandTextInput::OnKeysym(void* data,
                                struct text_model* text_input,
                                uint32_t serial,
                                uint32_t time,
                                uint32_t key,
                                uint32_t state,
                                uint32_t modifiers) {
  WaylandTextInput* textInuput = static_cast<WaylandTextInput*>(data);

  // Copid from WaylandKeyboard::OnKeyNotify()
  ui::EventType type = ui::ET_KEY_PRESSED;
  WaylandDisplay::GetInstance()->SetSerial(serial);
  if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
    type = ui::ET_KEY_RELEASED;

  // Check if we can ignore the KeyEvent notification, saves an IPC call.
  if (textInuput->getInputDevice()->GetKeyBoard()->GetBackend()->
    IgnoreKeyNotify(key, type == ui::ET_KEY_PRESSED))
    return;

  ui::EventConverterOzoneWayland* dispatcher =
          ui::EventFactoryOzoneWayland::GetInstance()->EventConverter();

  switch (key) {
    case XKB_KEY_KP_Enter:
    case XKB_KEY_Return:
    case XKB_KEY_ISO_Enter:
      dispatcher->KeyNotify(type, ui::OZONEACTIONKEY_RETURN, modifiers);
      break;
    case XKB_KEY_BackSpace:  // FIXME: Back space is not handled.
      dispatcher->KeyNotify(type, ui::OZONEACTIONKEY_BACK, modifiers);
      break;
    case XKB_KEY_Left:
    case XKB_KEY_KP_Left:
      dispatcher->KeyNotify(type, ui::OZONEACTIONKEY_LEFT, modifiers);
      break;
    case XKB_KEY_Right:
    case XKB_KEY_KP_Right:
      dispatcher->KeyNotify(type, ui::OZONEACTIONKEY_RIGHT, modifiers);
      break;
    default:
      break;
  }
}

void WaylandTextInput::OnEnter(void* data,
                               struct text_model* text_input,
                               struct wl_surface* surface) {
}

void WaylandTextInput::OnLeave(void* data,
                               struct text_model* text_input) {
}

void WaylandTextInput::OnInputPanelState(void* data,
                               struct text_model* text_input,
                               uint32_t state) {
}

void WaylandTextInput::OnTextModelInputPanelRect(void *data,
                      struct text_model *text_model,
                      int32_t x,
                      int32_t y,
                      uint32_t width,
                      uint32_t height) {
}

void WaylandTextInput::ResetIme() {
#if defined(ENABLE_OZONE_WAYLAND_VKB)
  if (!text_model_ && WaylandDisplay::GetInstance()->GetTextModelFactory()) {
    text_model_ = text_model_factory_create_text_model(
        WaylandDisplay::GetInstance()->GetTextModelFactory());
    text_model_add_listener(text_model_, &text_model_listener_, this);
  }
#endif
}

void WaylandTextInput::ShowInputPanel(wl_seat* input_seat) {
#if defined(ENABLE_OZONE_WAYLAND_VKB)
  if (is_visible_)
    return;
  uint32_t serial= 0;

  if (!text_model_ && WaylandDisplay::GetInstance()->GetTextModelFactory()) {
    text_model_ = text_model_factory_create_text_model(
        WaylandDisplay::GetInstance()->GetTextModelFactory());
    text_model_add_listener(text_model_, &text_model_listener_, this);
  }

  if (text_model_ && active_window_) {
    text_model_activate(text_model_,
                        serial,
                        input_seat,
                        active_window_->ShellSurface()->GetWLSurface());
    is_visible_ = true;
  }
#endif
}

void WaylandTextInput::HideInputPanel(wl_seat* input_seat) {
#if defined(ENABLE_OZONE_WAYLAND_VKB)
  if (!is_visible_)
    return;

  if (text_model_)
  {
    text_model_deactivate(text_model_, input_seat);
    text_model_destroy(text_model_);
    text_model_ = false;
    is_visible_ = false;
  }
#endif
}

}  // namespace ozonewayland
