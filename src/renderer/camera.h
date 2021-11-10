#ifndef ARC_CAMERA
#define ARC_CAMERA
#include "glm/mat4x4.hpp"
namespace arc {

class Camera {
public:
  void SetView(const glm::vec3 &pos, const glm::vec3 &look_at,
               const glm::vec3 &up);

  void SetProjectionPersp(float fov, float ratio, float z_near, float z_far);
  void SetProjectionOrth(float zoom, float ratio, float z_near, float z_far);

public:
  glm::mat4 view_, projection_, view_projection_;
};
}
#endif
