#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace arc {
void Camera::SetView(const glm::vec3 &pos, const glm::vec3 &look_at,
                     const glm::vec3 &up) {
  view_ = glm::lookAt(pos, pos + look_at, up);
  view_projection_ = projection_ * view_;
}
void Camera::SetProjectionPersp(float fov, float ratio, float z_near,
                                float z_far) {
  projection_ = glm::perspective(glm::radians(fov), ratio, z_near, z_far);
  view_projection_ = projection_ * view_;
}
void Camera::SetProjectionOrth(float zoom, float ratio, float z_near,
                               float z_far) {
  projection_ =
      glm::ortho(-ratio * zoom, ratio * zoom, -zoom, zoom, z_near, z_far);
  view_projection_ = projection_ * view_;
}

} // namespace arc
