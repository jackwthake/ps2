#pragma once

#include <glad/glad.h>
#include <ps2/shader_prog.hpp>

#include <string>
#include <cstddef>
#include <vector>

class model {
  GLuint vao = 0, vbo = 0;

protected:
  shader_prog shader;
  size_t vertex_count = 0;

public:
  model(const std::vector<float> &vertices,
        const std::string &vert_path, const std::string &frag_path);

  ~model();

  model(const model &) = delete;
  model &operator=(const model &) = delete;

  model(model &&other) noexcept;
  model &operator=(model &&other) noexcept;

  void render();
};