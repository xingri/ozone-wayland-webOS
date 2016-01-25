// Copyright 2014 LG Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone/wayland/shell/webos_shell_surface.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "ozone/ui/events/event_factory_ozone_wayland.h"
#include "ozone/wayland/display.h"
#include "ozone/wayland/shell/shell.h"

#if defined(BROWSER_COMMON)
#include "base/logging.h"
#include "base/process/kill.h"
#include "chrome/webos/common/shell_updates.h"
#endif

namespace ozonewayland {

namespace {

const char kTagAppId[] = "appId";

} // namespace

WebosShellSurface::WebosShellSurface()
    : WLShellSurface(),
      webos_shell_surface_(NULL) {
}

WebosShellSurface::~WebosShellSurface() {
  if (webos_shell_surface_)
    wl_webos_shell_surface_destroy(webos_shell_surface_);
}

void WebosShellSurface::InitializeShellSurface(WaylandWindow* window) {
  WLShellSurface::InitializeShellSurface(window);
  DCHECK(!webos_shell_surface_);
  WaylandDisplay* display = WaylandDisplay::GetInstance();
  DCHECK(display);
  WaylandShell* shell = WaylandDisplay::GetInstance()->GetShell();
  DCHECK(shell && shell->GetWebosWLShell());
  webos_shell_surface_ = wl_webos_shell_get_shell_surface(shell->GetWebosWLShell(),
                                              GetWLSurface());

#if defined(BROWSER_COMMON)
  display->SetWebosShellSurface(webos_shell_surface_);
#endif

  static const wl_webos_shell_surface_listener webos_shell_surface_listener = {
    WebosShellSurface::HandleStateChanged,
    WebosShellSurface::HandlePositionChanged,
    WebosShellSurface::HandleClose,
    WebosShellSurface::HandleExposed,
    WebosShellSurface::HandleStateAboutToChange
  };

  wl_webos_shell_surface_add_listener(webos_shell_surface_,
                                      &webos_shell_surface_listener,
                                      window);

  DCHECK(webos_shell_surface_);
}

void WebosShellSurface::UpdateShellSurface(WaylandWindow::ShellType type,
                                           WaylandShellSurface* shell_parent,
                                           unsigned x,
                                           unsigned y) {
  switch (type) {
  case WaylandWindow::FULLSCREEN:
    wl_webos_shell_surface_set_state(webos_shell_surface_,
                                     WL_WEBOS_SHELL_SURFACE_STATE_FULLSCREEN);
    break;
  default:
    break;
  }

  WLShellSurface::UpdateShellSurface(type,
                                     shell_parent,
                                     x,
                                     y);
}

void WebosShellSurface::Maximize() {
  wl_webos_shell_surface_set_state(webos_shell_surface_,
                                   WL_WEBOS_SHELL_SURFACE_STATE_MAXIMIZED);
  WLShellSurface::Maximize();
}

void WebosShellSurface::Minimize() {
  wl_webos_shell_surface_set_state(webos_shell_surface_,
                                   WL_WEBOS_SHELL_SURFACE_STATE_MINIMIZED);
  WLShellSurface::Minimize();
}

void WebosShellSurface::HandleStateChanged(void* data,
                                     struct wl_webos_shell_surface* webos_shell_surface,
                                     uint32_t state) {
#if defined(WEBOS_BROWSER)
  if (webos::ShellUpdates::Get())
    webos::ShellUpdates::Get()->OnStateChanged(state);
#endif
}

void WebosShellSurface::HandlePositionChanged(void* data,
                                        struct wl_webos_shell_surface* webos_shell_surface,
                                        int32_t x,
                                        int32_t y) {
}

void WebosShellSurface::HandleClose(void* data,
                              struct wl_webos_shell_surface* webos_shell_surface) {
#if defined(WEBOS_BROWSER)
  webos::ShellUpdates* shell_update = webos::ShellUpdates::Get();
  if (!shell_update) {
    RAW_PMLOG_INFO("[Chrome Browser] :[%s] Killed !! \n", __FUNCTION__);
    if (base::KillProcesses(std::string("chrome"), 0, NULL))
      base::KillProcessGroup(base::GetCurrentProcessHandle());
    return;
  }
  shell_update->OnCloseWindow();
  return;
#endif

  WaylandWindow *window = static_cast<WaylandWindow*>(data);
  ui::EventConverterOzoneWayland* dispatcher =
      ui::EventFactoryOzoneWayland::GetInstance()->EventConverter();
  dispatcher->CloseWindow(window->Handle());
}

void WebosShellSurface::HandleExposed(void* data,
                                struct wl_webos_shell_surface* webos_shell_surface,
                                struct wl_array *rectangles) {
}

void WebosShellSurface::HandleStateAboutToChange(void* data,
                                           struct wl_webos_shell_surface* webos_shell_surface,
                                           uint32_t state) {
#if defined(WEBOS_BROWSER)
  if (webos::ShellUpdates::Get())
    webos::ShellUpdates::Get()->OnStateAboutToChange(state);
#endif
}

}  // namespace ozonewayland
