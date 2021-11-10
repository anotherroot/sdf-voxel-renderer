/* standard libraries */
#include "queue"
#include "random"
#include "stack"
#include "stdio.h"
#include "unordered_map"
/* external libraries */
#include "glad/glad.h"
// glad before glfw
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>
/* project libraries */
#include "render_pass.h"
#include "renderer.h"
#include "camera_controller.h"
#include "voxel.h"
#include "window.h"

#include "log.h"

using namespace arc;

void UpdateGenTex();

// Variables
//================================================
glm::vec3 backgorund;
unsigned int albedo;
unsigned int depth;
unsigned int comp;
unsigned int gen_tex;
unsigned int tex_shader;
unsigned int compute_shader;
unsigned int gen_tex_shader;
Ref<DebugCamera> debug_cam;
glm::vec2 mouse_position;

bool run_build_tex = false;
float time_ = 0;
int tex_w = 512, tex_h = 512;
int gen_s = 512;
float fps;
unsigned int map_id;
unsigned int test_render_pass;
// project

// models
Ref<VoxelModel> empty_model;
struct VoxelModelData {
  std::string relative_path;
  Ref<VoxelModel> model;
};
std::unordered_map<std::string, VoxelModelData> models;

void UpdateGenTex() {
  // gen tex
  {
    shader::Bind(gen_tex_shader);
    shader::SetFloat(1, time_);
    tex::BindImage(gen_tex, 0, true);
    glDispatchCompute((GLuint)gen_s / 8, (GLuint)gen_s / 8, (GLuint)gen_s / 8);
  }
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
// Events
//================================================
void OnWindowResize(int w, int h) {
  window::Resize(w, h);

  debug_cam->OnWindowResize(w, h);
  tex_h = h / (18);
  tex_w = w / (32);
  tex::Resize2D(albedo, w, h, tex::RGBA, tex::BYTE, NULL);
  tex::Resize2D(depth, w, h, tex::RGBA, tex::BYTE, NULL);
  pass::Resize(test_render_pass, w, h);
  glViewport(0, 0, w, h);
}
void OnWindowClose() {

  window::Close();
}
void OnKeyPress(int key_code, int mode, bool repeat) {
  if (!repeat && key_code == GLFW_KEY_R) {
    shader::Reload(tex_shader);
    shader::Reload(compute_shader);
    shader::Reload(gen_tex_shader);
    UpdateGenTex();
  }
  if (!repeat && key_code == GLFW_KEY_P) {
    run_build_tex = !run_build_tex;
  }

  if (key_code == GLFW_KEY_L) {
    tex::SetFilter2D(albedo, tex::LINEAR);
  }

  if (key_code == GLFW_KEY_N) {
    tex::SetFilter2D(albedo, tex::NEAREST);
  }

    debug_cam->OnKeyPressed(key_code, mode, repeat);

}
void OnKeyRelease(int key_code, int mode) {
    debug_cam->OnKeyReleased(key_code, mode);
}
void OnButtonPress(int button, int mode) {
}
void OnButtonRelease(int button, int mode) {}
void OnScroll(double d_x, double d_y) {
    debug_cam->OnScroll(d_x, d_y);
}

void OnMove(double x, double y) {

  mouse_position = glm::vec2(x, y);
  debug_cam->OnMove(x, y);
  debug_cam->UpdatePrevPos(x, y);
}

// GUI
//================================================
void ImGuiOptions() {
  ImGui::Begin("Options");
  ImGui::ColorEdit3("Color", (float *)&backgorund[0]);
  ImGui::Text(("fps: " + std::to_string(fps)).c_str());
  if (ImGui::TreeNode("Camera")) {
    ImGui::Text("Selected");
    ImGui::TreePop();
  }
  ImGui::End();
}

void ImGuiUpdate(double delta_time) {
  ImGui::ShowDemoWindow();
  ImGuiOptions();

  ImGui::Begin("Depth");
  ImGui::Text("Depth");
  ImGui::Image(reinterpret_cast<ImTextureID>(tex::GetID(depth)),
               ImVec2(window::Width() / 2.0f, window::Height() / 2.0f),
               ImVec2(0, 1), ImVec2(1, 0));

  ImGui::Text("Comp");
  ImGui::Image(reinterpret_cast<ImTextureID>(tex::GetID(comp)),
               ImVec2(window::Width() / 2.0f, window::Height() / 2.0f),
               ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
}
// GUI
//================================================
void Setup() {

  shader::Create(
      tex_shader,
      "/home/tilen/Projects/voxel_city/sdf-voxel-renderer/assets/shaders/texture.glsl");

  shader::Create(
      compute_shader,
      "/home/tilen/Projects/voxel_city/sdf-voxel-renderer/assets/shaders/compute.glsl");

  shader::Create(
      gen_tex_shader,
      "/home/tilen/Projects/voxel_city/sdf-voxel-renderer/assets/shaders/gen_tex.glsl");

  pass::Create(
      test_render_pass, window::Width(), window::Height(),
      "/home/tilen/Projects/voxel_city/sdf-voxel-renderer/assets/shaders/debug.glsl",
      {{"albedo", tex::RGBA, tex::UNSIGNED_BYTE, tex::LINEAR}});

  // outpout texture for compute shader
  {
    tex::Create2D(albedo, tex_w, tex_h, tex::RGBA, tex::BYTE, NULL);
    tex::SetWrap2D(albedo, tex::CLAMP_TO_EDGE);
    tex::SetFilter2D(albedo, tex::LINEAR);
    tex::BindImage(albedo, 0);
  }
  // outpout texture for compute shader

  {
    tex::Create2D(depth, tex_w, tex_h, tex::RGBA, tex::BYTE, NULL);
    tex::SetWrap2D(depth, tex::CLAMP_TO_EDGE);
    tex::SetFilter2D(depth, tex::LINEAR);
    tex::BindImage(depth, 1);
  }

  {
    tex::Create2D(comp, tex_w, tex_h, tex::RGBA, tex::BYTE, NULL);
    tex::SetWrap2D(comp, tex::CLAMP_TO_EDGE);
    tex::SetFilter2D(comp, tex::LINEAR);
    tex::BindImage(comp, 1);
  }


  {
    tex::Create3D(gen_tex, gen_s, gen_s, gen_s, tex::RGBA, tex::BYTE, NULL);
    tex::SetWrap3D(gen_tex, tex::CLAMP_TO_EDGE);
    tex::SetFilter3D(gen_tex, tex::NEAREST);
    tex::BindImage(gen_tex, 0);
  }

  window::SetVsync(true);
  debug_cam = CreateRef<DebugCamera>(
      DebugCamera{{10, 10, 10}, {0, 0, 0}, window::Width(), window::Height()});

  UpdateGenTex();
}

void Dispose() {
  window::Dispose();
  shader::Dispose();
  tex::Dispose();
}
void Update(double delta_time) {
  fps = 1.0 / delta_time;
    debug_cam->Update(delta_time);
  if(run_build_tex){
  time_ += delta_time;
    UpdateGenTex();
  }
  // compute shader
  {
    shader::Bind(compute_shader);
    tex::BindImage(albedo, 0);
    tex::BindImage(depth, 1);
    tex::BindImage(comp, 2);
    shader::SetFloat3(1, backgorund);
      shader::SetMat4(2, debug_cam->view_projection());
      shader::SetFloat3(3, debug_cam->pos());
      shader::SetInt(8, debug_cam->perspective());
      shader::SetFloat3(9, debug_cam->look_at());
    shader::SetFloat3(4, glm::vec3(0.1));
    shader::SetInt(5, 0);
    /* tex::Bind(empty_model->render_id(), 0); */
    /* shader::SetFloat3(6, empty_model->raw_size()); */
    /* shader::SetFloat3(7, 1.0f / (const glm::vec3)empty_model->fixed_size());
     */

    tex::Bind(gen_tex, 0);
    shader::SetFloat3(6, glm::vec3(gen_s));
    shader::SetFloat3(7, 1.0f / glm::vec3(gen_s));

    glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1);
  }
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  // render pass test

  // draw texture
  {
    glClearColor(backgorund[0], backgorund[1], backgorund[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader::Bind(tex_shader);
    shader::SetInt(tex_shader, "u_albedo", 1);
    tex::Bind(albedo, 1);
    shader::SetInt(tex_shader, "u_debug", 0);
    pass::BindFrame(test_render_pass, "albedo", 0);
    renderer::DrawScreenQuad();
    shader::Unbind();
  }
}
//TODO: make into imgui alert
void Notify(const char* message){
  loge(message);
}

int main() {
  window::Setup(256, 256);

  window::SetResizeCallback(OnWindowResize);
  window::SetCloseCallback(OnWindowClose);
  window::SetKeyCallback(OnKeyPress, OnKeyRelease);
  window::SetButtonCallback(OnButtonPress, OnButtonRelease);
  window::SetScrollCallback(OnScroll);
  window::SetMoveCallback(OnMove);

  window::SetArcNotifyCallback(Notify);

  window::SetUpdate(Update);
  window::SetImGuiUpdate(ImGuiUpdate);

  renderer::Setup();
  Setup();
  window::Run();
  Dispose();
  renderer::Dispose();

  window::Dispose();
  return 0;
}
