#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "camera_controller.h"
#include "window.h"
namespace arc {
/**********************************************************************
 *                            Debug Camera                            *
 **********************************************************************/
DebugCamera::DebugCamera(const glm::vec3 &pos, const glm::vec3 &look_at_target,
                         int w, int h) {
  pos_ = pos;
  speed_ = 5;
  controlling_ = false;
  up_ = {0, 1, 0};
  fov_ = 60.0f;
  ratio_ = (float)w / (float)h;
  z_near = 0.1;
  z_far = 100;
  dir_ = {0, 0, 0};
  look_at_ = glm::normalize(look_at_target - pos);
  right_ = glm::cross(look_at_,up_);
  impl::SetLookAt(look_at_, yaw_, pitch_);
  UpdateProjection_();
  UpdateView();
  /* yaw_ = 180 + 45; */
  /* pitch_ = -45; */
}
void DebugCamera::Update(float delta_time) {
  pos_ += speed_ * (look_at_ * dir_.z + up_ * dir_.y + right_ * dir_.x) *
          delta_time;
  UpdateView();
}

bool DebugCamera::OnButtonPressed(int button, int mode) { return false; }
bool DebugCamera::OnButtonReleased(int button) { return false; }
bool DebugCamera::OnScroll(double x_offset, double y_offset) {
  speed_ *= y_offset < 0 ? 1.1 : 0.9;
  return false;
}
bool DebugCamera::OnMove(double x, double y) {
  if (!controlling_) return false;
    float dx = (x_ - x);
    float dy = (y_ - y);
    yaw_ += dx;
    pitch_ = glm::clamp(pitch_ + dy, -89.0f, 89.0f);
    if (yaw_ < 0) {
      yaw_ += 360;
    } else if (yaw_ >= 360) {
      yaw_ -= 360;
    }

    look_at_ = impl::GetLookAt(yaw_, pitch_);
    right_ = glm::cross(look_at_,up_);
    UpdateView();
  return false;
}
bool DebugCamera::OnKeyPressed(int key_code, int mode, bool repeat) {

  if (!repeat) {
    if (key_code == GLFW_KEY_W)
      dir_.z += 1;
    if (key_code == GLFW_KEY_S)
      dir_.z -= 1;
    if (key_code == GLFW_KEY_A)
      dir_.x -= 1;
    if (key_code == GLFW_KEY_D)
      dir_.x += 1;
    if (key_code == GLFW_KEY_SPACE)
      dir_.y += 1;
    if (key_code == GLFW_KEY_LEFT_SHIFT)
      dir_.y -= 1;
  }

  if (!repeat && key_code == GLFW_KEY_C) {
    controlling_ = !controlling_;
    if (controlling_) {
      window::HideCursor();
    } else {
      window::ShowCursor();
    }
  }

  return false;
}
bool DebugCamera::OnKeyReleased(int key_code, int mode) {

  if (key_code == GLFW_KEY_W)
    dir_.z += -1;
  if (key_code == GLFW_KEY_S)
    dir_.z -= -1;
  if (key_code == GLFW_KEY_A)
    dir_.x += 1;
  if (key_code == GLFW_KEY_D)
    dir_.x -= 1;
  if (key_code == GLFW_KEY_SPACE)
    dir_.y += -1;
  if (key_code == GLFW_KEY_LEFT_SHIFT)
    dir_.y -= -1;

  return false;
}

/**********************************************************************
 *                            Game Camera                             *
 **********************************************************************/
#define MAX_ZOOM 10000
#define MIN_ZOOM 500
GameCamera::GameCamera(int w, int h) {

  pos_ = {300, 200, 300};
  speed_ = {5};
  up_ = {0, 1, 0};
  ratio_ = {(float)w / (float)h};
  z_near = {0};
  z_far = {100};
  zoom_ = {1000};
  rotating_ = {false};
  dir_ = {0, 0, 0};
  look_at_ = impl::GetLookAt(180 - 45, -25);
  right_ = glm::cross(look_at_,up_);
  camera_.SetView(pos_, look_at_, up_);
  camera_.SetProjectionOrth(zoom_, ratio_, z_near, z_far);
}
void GameCamera::Update(float delta_time) {}
bool GameCamera::OnButtonPressed(int button, int mode) { return false; }
bool GameCamera::OnButtonReleased(int button) { return false; }
bool GameCamera::OnScroll(double x_offset, double y_offset) { return false; }
bool GameCamera::OnMove(double x_pos, double y_pos) { return false; }
bool GameCamera::OnKeyPressed(int key, int mode, bool repeat) { return false; }
bool GameCamera::OnKeyReleased(int key, int mode) { return false; }

/**********************************************************************
 *                      Implementation Functions                      *
 **********************************************************************/
namespace impl {
void SetLookAt(const glm::vec3 &look_at, float &yaw, float &pitch) {
  pitch = glm::asin(look_at.y);
  yaw = glm::acos(look_at.z / glm::cos(pitch));
}
glm::vec3 GetLookAt(float yaw, float pitch) {
  yaw = glm::radians(yaw);
  pitch = glm::radians(pitch);
  return {glm::sin(yaw) * glm::cos(pitch), glm::sin(pitch),
          glm::cos(yaw) * glm::cos(pitch)};
}

} // namespace impl

// get target direction from yaw and pitch

} // namespace arc
/* void Create(Camera &cam, int w, int h) { */
/*   cam.yaw = 180 + 45; */
/*   cam.pitch = -45; */
/*   cam.speed = 5; */
/*   cam.fp=false; */
/*   camera::Create(cam, {10, 10, 10}, impl::LookAt(cam.yaw, cam.pitch), {0, 1,
 * 0}, */
/*                  w,  h, 60.0f, 0.1f, 100.0f, true); */
/* } */
/* void UpdateMove(Camera &cam, float x, float y) { */
/*   if (!cam.fp)return; */
/*   float dx = (cam.x - x); */
/*   float dy = (cam.y - y); */
/*   cam.yaw += dx; */
/*   cam.pitch = glm::clamp(cam.pitch + dy, -89.0f, 89.0f); */
/*   if (cam.yaw < 0) { */
/*     cam.yaw += 360; */
/*   } else if (cam.yaw >= 360) { */
/*     cam.yaw -= 360; */
/*   } */

/* cam.look_at = impl::LookAt(cam.yaw, cam.pitch); */
/* camera::SetLookAt(cam, cam.look_at); */
/* } */
/* void UpdatePrevPos(Camera &cam, float x, float y) { */
/* cam.x = x; */
/* cam.y = y; */
/* } */
/* void UpdateSize(Camera &cam, int w, int h) { */
/* camera::SetRatio(cam, w , h); */ /* } */
/* void UpdatePos(Camera &cam, float delta_time) { */
/* cam.pos += */
/*     cam.speed * */
/*     (cam.look_at * dir_.z + cam.up * dir_.y + cam.right * dir_.x) *
 */
/*     delta_time; */
/* impl::SetView(cam); */
/* } */
/* void OnKeyPressed(Camera &cam, unsigned int key_code, bool repeat) { */
/* if (!repeat) { */
/*   if (key_code == GLFW_KEY_W) */
/*     dir_.z += 1; */
/*   if (key_code == GLFW_KEY_S) */
/*     dir_.z -= 1; */
/*   if (key_code == GLFW_KEY_A) */
/*     dir_.x -= 1; */
/*   if (key_code == GLFW_KEY_D) */
/*     dir_.x += 1; */
/*   if (key_code == GLFW_KEY_SPACE) */
/*     dir_.y += 1; */
/*   if (key_code == GLFW_KEY_LEFT_SHIFT) */
/*     dir_.y -= 1; */
/* } */

/* if (!repeat && key_code == GLFW_KEY_C) { */
/*   cam.fp = !cam.fp; */
/*   if (cam.fp) { */
/*     window::HideCursor(); */
/*   } else { */
/*     window::ShowCursor(); */
/*   } */
/* } */
/* } */
/* void OnKeyReleased(Camera &cam, unsigned int key_code) { */
/* if (key_code == GLFW_KEY_W) */
/*   dir_.z += -1; */
/* if (key_code == GLFW_KEY_S) */
/*   dir_.z -= -1; */
/* if (key_code == GLFW_KEY_A) */
/*   dir_.x += 1; */
/* if (key_code == GLFW_KEY_D) */
/*   dir_.x -= 1; */
/* if (key_code == GLFW_KEY_SPACE) */
/*   dir_.y += -1; */
/* if (key_code == GLFW_KEY_LEFT_SHIFT) */
/*   dir_.y -= -1; */
/* } */
/* void OnScroll(Camera &cam, float dy) { cam.speed *= dy < 0 ? 1.1 : 0.9; } */
/* } // namespace debug */
/* namespace impl{ */

/* void SetView(Camera &c) { */
/*   c.view = glm::lookAt(c.pos, c.pos + c.look_at, c.up); */
/*   c.view_projection = c.projection * c.view; */
/* } */
/* void SetProjection(Camera &c) { */
/*   if (c.perspective) */
/*     c.projection = */
/*         glm::perspective(glm::radians(c.fov), c.ratio, c.z_near, c.z_far); */
/*   else { */
/*     /1* c.projection = glm::ortho(-c.w,  c.w, -c.h, *1/ */
/*     /1*                           c.h, -100, 1000); *1/ */
/*     c.projection = glm::ortho(-c.ratio*c.zoom,  c.ratio*c.zoom, -c.zoom, */
/*                               c.zoom, -100.0f, 1000.0f); */
/*   } */
/*   c.view_projection = c.projection * c.view; */
/* } */

/* glm::vec3 LookAt(float yaw, float pitch) { */
/*   yaw = glm::radians(yaw); */
/*   pitch = glm::radians(pitch); */
/*   return {glm::sin(yaw) * glm::cos(pitch), glm::sin(pitch), */
/*           glm::cos(yaw) * glm::cos(pitch)}; */
/* } */

/* } */
