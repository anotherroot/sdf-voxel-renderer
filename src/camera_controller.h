#ifndef ARC_CAMERA_CONTROLLER
#define ARC_CAMERA_CONTROLLER
#include "camera.h"
#include "listeners.h"
#include "log.h"
namespace arc {
namespace impl {
class CameraController : public WindowListener,
                         MouseListener,
                         KeyboardListener {
public:
  virtual void Update(float delta_time) = 0;

  inline bool OnWindowResize(int w, int h) override {
    ratio_ = ((float)w) / (float)h;
    loge("real ratio: %f", ratio_);
    UpdateProjection_();
    return false;
  }

  inline const Camera &camera() const { return camera_; }
  inline const glm::mat4 &view_projection() const {
    return camera_.view_projection_;
  }
  inline const glm::vec3 &pos() const { return pos_; }
  inline const glm::vec3 &look_at() const { return look_at_; }
  inline const glm::vec3 &right() const { return right_; }

  inline virtual bool perspective() const = 0;

protected:
  virtual inline void UpdateProjection_() = 0;
  inline void UpdateView() { camera_.SetView(pos_, look_at_, up_); }
  float ratio_;
  float speed_;

  float z_near, z_far;
  glm::vec3 dir_;
  glm::vec3 right_;
  glm::vec3 pos_;
  glm::vec3 look_at_, up_;
  Camera camera_;
};
/**********************************************************************
 *                      Implementation Functions                      *
 **********************************************************************/
void SetLookAt(const glm::vec3 &look_at, float &yaw, float &pitch);
glm::vec3 GetLookAt(float yaw, float pitch);
} // namespace impl

/**********************************************************************
 *                            Debug Camera                            *
 **********************************************************************/
class DebugCamera final : public impl::CameraController {
public:
  DebugCamera(const glm::vec3 &pos, const glm::vec3 &look_at_target, int w,
              int h);
  ~DebugCamera() = default;
  void Update(float delta_time) override;
  inline void UpdatePrevPos(double x, double y) {
    x_ = x;
    y_ = y;
  }

  bool OnButtonPressed(int button, int mode) override;
  bool OnButtonReleased(int button) override;
  bool OnScroll(double x_offset, double y_offset) override;
  bool OnMove(double x_pos, double y_pos) override;
  bool OnKeyPressed(int key, int mode, bool repeat) override;
  bool OnKeyReleased(int key, int mode) override;

  inline bool perspective() const override { return true; }

private:
  inline void UpdateProjection_() override {
    camera_.SetProjectionPersp(fov_, ratio_, z_near, z_far);
  }
  float fov_;
  float pitch_, yaw_;
  float x_, y_;
  // flags
  bool controlling_;
};

/**********************************************************************
 *                            Game Camera                             *
 **********************************************************************/
class GameCamera final : public impl::CameraController {
public:
  GameCamera(int w, int h);
  ~GameCamera() = default;
  void Update(float delta_time) override;

  bool OnButtonPressed(int button, int mode) override;
  bool OnButtonReleased(int button) override;
  bool OnScroll(double x_offset, double y_offset) override;
  bool OnMove(double x_pos, double y_pos) override;
  bool OnKeyPressed(int key, int mode, bool repeat) override;
  bool OnKeyReleased(int key, int mode) override;

  inline bool perspective() const override { return false; }

private:
  inline void UpdateProjection_() override {
    camera_.SetProjectionOrth(zoom_, ratio_, z_near, z_far);
  }
  float zoom_;

  bool rotating_;
};

} // namespace arc
#endif

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
/* camera::SetRatio(cam, w , h); */
/* } */
/* void UpdatePos(Camera &cam, float delta_time) { */
/* cam.pos += */
/*     cam.speed * */
/*     (cam.look_at * cam.dir.z + cam.up * cam.dir.y + cam.right * cam.dir.x) *
 */
/*     delta_time; */
/* impl::SetView(cam); */
/* } */
/* void OnKeyPressed(Camera &cam, unsigned int key_code, bool repeat) { */
/* if (!repeat) { */
/*   if (key_code == GLFW_KEY_W) */
/*     cam.dir.z += 1; */
/*   if (key_code == GLFW_KEY_S) */
/*     cam.dir.z -= 1; */
/*   if (key_code == GLFW_KEY_A) */
/*     cam.dir.x -= 1; */
/*   if (key_code == GLFW_KEY_D) */
/*     cam.dir.x += 1; */
/*   if (key_code == GLFW_KEY_SPACE) */
/*     cam.dir.y += 1; */
/*   if (key_code == GLFW_KEY_LEFT_SHIFT) */
/*     cam.dir.y -= 1; */
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
/*   cam.dir.z += -1; */
/* if (key_code == GLFW_KEY_S) */
/*   cam.dir.z -= -1; */
/* if (key_code == GLFW_KEY_A) */
/*   cam.dir.x += 1; */
/* if (key_code == GLFW_KEY_D) */
/*   cam.dir.x -= 1; */
/* if (key_code == GLFW_KEY_SPACE) */
/*   cam.dir.y += -1; */
/* if (key_code == GLFW_KEY_LEFT_SHIFT) */
/*   cam.dir.y -= -1; */
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
