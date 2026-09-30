#include <ps2/retro_target.hpp>
#include <ps2/util.hpp>

static GLuint compile_shader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  
  int ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (!ok) {
    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    fprintf(stderr, "shader compile error: %s\n", log);
  }

  return shader;
}


shader_prog::shader_prog(const std::string &vert, const std::string &frag, const bool from_source) {
  this->vert = this->frag = 0;

  // if !from_source then attempt to load from the res/shaders/<name>
  // otherwise just attempt to compile the args as source code
  // this is kind of messy.
  if (!from_source) {
    if (auto vert_src = load_resource_txt("shaders/" + vert)) {
      this->vert = compile_shader(GL_VERTEX_SHADER, vert_src->c_str());
    }

    if (auto frag_src = load_resource_txt("shaders/" + frag)) {
      this->frag = compile_shader(GL_FRAGMENT_SHADER, frag_src->c_str());
    }
  } else {
    this->vert = compile_shader(GL_VERTEX_SHADER, vert.c_str());
    this->frag = compile_shader(GL_FRAGMENT_SHADER, frag.c_str());
  }

  if (!this->vert && !this->frag) {
    exit(EXIT_FAILURE);
  }

  this->program = glCreateProgram();
  glAttachShader(this->program, this->vert);
  glAttachShader(this->program, this->frag);
  glLinkProgram(this->program);

  int linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[512];
    glGetProgramInfoLog(program, 512, NULL, log);
    fprintf(stderr, "link error: %s\n", log);
    exit(EXIT_FAILURE);
  }

  glDeleteShader(this->vert);
  glDeleteShader(this->frag);
}


shader_prog::~shader_prog() {
  glDeleteProgram(this->program);
}


void shader_prog::use() const {
  glUseProgram(this->program);
}


void shader_prog::set_int(const std::string &name, int value) const {
  GLint location = glGetUniformLocation(this->program, name.c_str());
  glUniform1i(location, value);
}


void shader_prog::set_float(const std::string &name, float value) const {
  GLint location = glGetUniformLocation(this->program, name.c_str());
  glUniform1f(location, value);
}
