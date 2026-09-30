#include <ps2/retro_target.hpp>
#include <cstdio>
#include <cstdlib>

namespace {

const char *composite_vert_src = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
out vec2 vUV;
void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char *composite_frag_src = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScreenTex;
uniform float uColorLevels; // e.g. 16.0

float dither_pattern(vec2 frag_coord) {
    int x = int(mod(frag_coord.x, 4.0));
    int y = int(mod(frag_coord.y, 4.0));
    int index = x + y * 4;
    float bayer[16] = float[](
        0.0, 8.0, 2.0, 10.0,
        12.0, 4.0, 14.0, 6.0,
        3.0, 11.0, 1.0, 9.0,
        15.0, 7.0, 13.0, 5.0
    );
    return bayer[index] / 16.0;
}

vec3 quantize(vec3 color, float levels) {
    return floor(color * levels + 0.5) / levels;
}

void main() {
    vec3 color = texture(uScreenTex, vUV).rgb;
    float d = dither_pattern(gl_FragCoord.xy) - 0.5;
    color += d / uColorLevels;
    color = quantize(color, uColorLevels);
    FragColor = vec4(color, 1.0);
}
)";

} // namespace


retro_target::retro_target(unsigned internal_width, unsigned internal_height)
  : internal_w(internal_width), internal_h(internal_height),
    composite_shader(composite_vert_src, composite_frag_src, true) {
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