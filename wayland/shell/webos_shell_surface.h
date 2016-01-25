// Copyright 2014 LG Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OZONE_WAYLAND_SHELL_WEBOS_SURFACE_H_
#define OZONE_WAYLAND_SHELL_WEBOS_SURFACE_H_

#include "ozone/wayland/shell/wl_shell_surface.h"
#include "wayland-webos-shell-client-protocol.h"

namespace ozonewayland {

class WaylandSurface;
class WaylandWindow;

class WebosShellSurface : public WLShellSurface {
 public:
  WebosShellSurface();
  virtual ~WebosShellSurface();

  virtual void InitializeShellSurface(WaylandWindow* window) OVERRIDE;
  virtual void UpdateShellSurface(WaylandWindow::ShellType type,
                                  WaylandShellSurface* shell_parent,
                                  unsigned x,
                                  unsigned y) OVERRIDE;
  virtual void Maximize() OVERRIDE;
  virtual void Minimize() OVERRIDE;

  static void HandleStateChanged(void* data,
                              struct wl_webos_shell_surface* webos_shell_surface,
                              uint32_t state);

  static void HandlePositionChanged(void* data,
                              struct wl_webos_shell_surface* webos_shell_surface,
                              int32_t x,
                              int32_t y);

  static void HandleClose(void* data,
                         struct wl_webos_shell_surface* webos_shell_surface);

  static void HandleExposed(void* data,
                         struct wl_webos_shell_surface* webos_shell_surface,
                         struct wl_array *rectangles);

  static void HandleStateAboutToChange(void* data,
                         struct wl_webos_shell_surface* webos_shell_surface,
                         uint32_t state);

 private:
  wl_webos_shell_surface* webos_shell_surface_;
  DISALLOW_COPY_AND_ASSIGN(WebosShellSurface);
};

}  // namespace ozonewayland

#endif  // OZONE_WAYLAND_SHELL_WEBOS_SURFACE_H_
