#pragma once

#include <glad/glad.h>
#include <Engine/shader_prog.hpp>

#include <memory>
#include <string>

class model {
  GLuint vao, vbo;
protected:
  shader_prog shader;
  std::unique_ptr<float[]> vertices;

public:
  model(std::unique_ptr<float[]> vertices, const std::string &vert_path,
        const std::string &frag_path);

  ~model();

  void render();
};