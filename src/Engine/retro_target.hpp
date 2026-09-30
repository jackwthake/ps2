#pragma once

#include <glad/glad.h>
#include "shader_prog.hpp"

class retro_target {
  GLuint fbo = 0;
  GLuint color_tex = 0;
  GLuint depth_rbo = 0;

  GLuint quad_vao = 0;
  GLuint quad_vbo = 0;

  unsigned internal_w, internal_h;
  shader_prog composite_shader;

  bool bypass = false; // debug: skip low-res pass, render straight to window

  void init_framebuffer();
  void init_quad();

public:
  retro_target(unsigned internal_width, unsigned internal_height);
  ~retro_target();

  retro_target(const retro_target &) = delete;
  retro_target &operator=(const retro_target &) = delete;

  // bind the low-res target and clear it; call before drawing the 3D scene
  void begin_scene(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;

  // upscale + dither/quantize into whatever framebuffer is bound at output_w x output_h
  // (caller is responsible for having already bound framebuffer 0 and set the viewport,
  // or leave that to Renderer::end_frame — see note below)
  void composite(unsigned output_w, unsigned output_h) const;

  unsigned width() const  { return internal_w; }
  unsigned height() const { return internal_h; }

  void set_bypass(bool b) { bypass = b; }
  bool is_bypassed() const { return bypass; }
};