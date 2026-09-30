#pragma once

#include <iostream>
#include <glad/glad.h>

class shader_prog {
  GLuint vert, frag, program;
public:
  // if !from_source then attempt to load from the res/shaders/<name>
  // otherwise just attempt to compile the args as source code
  shader_prog(const std::string &vert, const std::string &frag, const bool from_source);
  ~shader_prog();

  void use() const;
  void set_int(const std::string &name, int value) const;
  void set_float(const std::string &name, float value) const;
};