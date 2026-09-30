#pragma once

#include <iostream>
#include <glad/glad.h>

class shader_prog {
  GLuint vert, frag, program;
public:
  shader_prog(const std::string &vert_name, const std::string &frag_name);
  ~shader_prog();

  void use() const;
  void set_int(const std::string &name, int value) const;
  void set_float(const std::string &name, float value) const;
};