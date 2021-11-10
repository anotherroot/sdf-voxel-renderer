#ifndef ARC_RENDERER
#define ARC_RENDERER
#include "glm/glm.hpp"
namespace arc::renderer{
  void Setup();
  void Dispose();
  void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& color);
  void DrawScreenQuad();
}
#endif
