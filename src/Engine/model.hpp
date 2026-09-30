#pragma once

#include <glad/glad.h>
#include <Engine/shader_prog.hpp>

#include <memory>
#include <string>
#include <cstddef>

class model {
  GLuint vao = 0, vbo = 0;

protected:
  shader_prog shader;
  std::unique_ptr<float[]> vertices;
  size_t vertex_count; // number of vertices, not floats

public:
  model(std::unique_ptr<float[]> vertices, size_t vertex_count,
        const std::string &vert_path, const std::string &frag_path);

  ~model();

  model(const model &) = delete;
  model &operator=(const model &) = delete;

  model(model &&other) noexcept;
  model &operator=(model &&other) noexcept;

  void render();
};