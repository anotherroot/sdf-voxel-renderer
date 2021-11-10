#include "renderer.h"
#include "glad/glad.h"
#include "iostream"
namespace arc::renderer {

struct QuadVertex {
  glm::vec3 pos;
  glm::vec2 tex_coords;
};
struct LineVertex {
  glm::vec3 pos;
  glm::vec3 color;
};

unsigned int quad_vbo, quad_vao;
QuadVertex quad_vertices[] = {
    {{-1, -1, 0}, {0, 0}}, {{1, -1, 0}, {1, 0}}, {{1, 1, 0}, {1, 1}},
    {{-1, -1, 0}, {0, 0}}, {{1, 1, 0}, {1, 1}},  {{-1, 1, 0}, {0, 1}},
};
unsigned int line_vbo, line_vao;
LineVertex line_vertices[3];

void Setup() {
  // setup quad drawing vertex array
  glGenVertexArrays(1, &quad_vao);
  glBindVertexArray(quad_vao);
  glGenBuffers(1, &quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex),
                        (void *)(sizeof(QuadVertex::pos)));
  glEnableVertexAttribArray(1);

  // setup line drawing vertex array
  glGenVertexArrays(1, &line_vao);
  glBindVertexArray(line_vao);
  glGenBuffers(1, &line_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex),
                        (void *)(sizeof(LineVertex::pos)));
  glEnableVertexAttribArray(1);

}
void Dispose() {
  glDeleteBuffers(1, &quad_vbo);
  glDeleteVertexArrays(1, &quad_vao);
  glDeleteBuffers(1, &line_vbo);
  glDeleteVertexArrays(1, &line_vao);
}
void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1,
              const glm::vec3 &color) {
  line_vertices[0].pos = p0;
  line_vertices[0].color = color;
  line_vertices[1].pos = p1;
  line_vertices[1].color = color;

  glBindVertexArray(line_vao);
  glBindBuffer(GL_ARRAY_BUFFER,line_vbo);
  glBufferSubData(GL_ARRAY_BUFFER,0,2*sizeof(LineVertex),line_vertices);
  glDrawArrays(GL_LINES, 0, 2);
}
void DrawScreenQuad() {
  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
} // namespace arc::renderer
