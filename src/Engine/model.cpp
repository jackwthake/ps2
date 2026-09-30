#include "model.hpp"
  
model::model(std::unique_ptr<float[]> vertices, const std::string &vert_path,
             const std::string &frag_path)
            : shader(vert_path, frag_path) {
  this->vertices = std::move(vertices);

  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), this->vertices.get(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0); // safe to unbind, VAO remembers the attrib state
  glBindVertexArray(0);
}


model::~model() {
  glDeleteVertexArrays(1, &this->vao);
  glDeleteBuffers(1, &this->vbo);
}


void model::render() {
  glBindVertexArray(this->vao);
  this->shader.use();
  glDrawArrays(GL_TRIANGLES, 0, 3);
}