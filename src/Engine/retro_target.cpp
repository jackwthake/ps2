// retro_target.cpp
#include "retro_target.hpp"
#include <cstdio>
#include <cstdlib>


retro_target::retro_target(unsigned internal_width, unsigned internal_height)
  : internal_w(internal_width), internal_h(internal_height),
    composite_shader("core/composite.vert", "core/composite.frag") {
  init_framebuffer();
  init_quad();
}


void retro_target::init_framebuffer() {
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, internal_w, internal_h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

  glGenRenderbuffers(1, &depth_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, internal_w, internal_h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "RetroTarget: framebuffer incomplete (0x%x)\n", status);
    exit(EXIT_FAILURE);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void retro_target::init_quad() {
  // NDC fullscreen quad, position + uv interleaved
  float verts[] = {
    // pos        // uv
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
  };

  glGenVertexArrays(1, &quad_vao);
  glGenBuffers(1, &quad_vbo);

  glBindVertexArray(quad_vao);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


void retro_target::begin_scene(float r, float g, float b, float a) const {
  if (bypass) return; // caller's own clear/viewport applies in this mode

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, internal_w, internal_h);
  glEnable(GL_DEPTH_TEST);
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void retro_target::composite(unsigned output_w, unsigned output_h) const {
  if (bypass) return; // scene was already drawn straight to the bound target

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, output_w, output_h);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  composite_shader.use();
  composite_shader.set_int("uScreenTex", 0);
  composite_shader.set_float("uColorLevels", 16.0f);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_tex);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}


retro_target::~retro_target() {
  glDeleteFramebuffers(1, &fbo);
  glDeleteTextures(1, &color_tex);
  glDeleteRenderbuffers(1, &depth_rbo);
  glDeleteVertexArrays(1, &quad_vao);
  glDeleteBuffers(1, &quad_vbo);
}