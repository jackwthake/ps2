#include <ps2/model.hpp>

#include <stdexcept>
#include <utility>

model::model(const std::vector<float> &vertices,
             const std::string &vert_path, const std::string &frag_path)
            : shader(vert_path, frag_path, false), vertex_count(vertices.size() / 3) {
  if (vertices.size() % 3 != 0) {
    throw std::invalid_argument("vertex data must contain 3 floats per vertex");
  }

  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);

  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0); // safe to unbind, VAO remembers the attrib state
  glBindVertexArray(0);
}


model::model(model &&other) noexcept
  : vao(other.vao), vbo(other.vbo), shader(std::move(other.shader)),
    vertex_count(other.vertex_count) {
  other.vao = 0;
  other.vbo = 0;
}


model &model::operator=(model &&other) noexcept {
  if (this != &other) {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);

    this->vao = other.vao;
    this->vbo = other.vbo;
    this->shader = std::move(other.shader);
    this->vertex_count = other.vertex_count;

    other.vao = 0;
    other.vbo = 0;
  }
  return *this;
}


model::~model() {
  glDeleteVertexArrays(1, &this->vao);
  glDeleteBuffers(1, &this->vbo);
}


void model::render() {
  glBindVertexArray(this->vao);
  this->shader.use();
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(this->vertex_count));
}
