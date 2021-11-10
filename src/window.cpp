/* standard libraries */
#include "assert.h"
#include "map"
#include "stdio.h"
/* external libraries */
#include "glad/glad.h"
// glad before glfw
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/* project libraries */
#include "window.h"

namespace arc::window {
// structs
struct Window {
  void *win{nullptr};
  const char *title;
  int w;
  int h;
  bool running;
  bool vsync{false};

  UpdateFn update_imgui_fn{nullptr};
  UpdateFn update_game_fn{nullptr};

  CloseFn close_fn{[]() {}};
  ResizeFn resize_fn{[](int w, int h) {}};
  KeyPressFn key_press_fn{[](int, int, bool) {}};
  KeyReleaseFn key_release_fn{[](int, int) {}};
  ButtonPressFn button_press_fn{[](int, int) {}};
  ButtonReleaseFn button_release_fn{[](int, int) {}};
  ScrollFn scroll_fn{[](double, double) {}};
  MoveFn move_fn{[](double, double) {}};
  ArcNotifyFn arc_notify_fn{[](const char*){}};
};

// declarations
void ImGuiSetup();
void ImGuiDispose();
// Functions
static const char* ImGui_GetClipboardText(void* user_data);
static void ImGui_SetClipboardText(void* user_data, const char* text);
void ImGui_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ImGui_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ImGui_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ImGui_WindowFocusCallback(GLFWwindow* window, int focused);
void ImGui_CursorEnterCallback(GLFWwindow* window, int entered);
void ImGui_CharCallback(GLFWwindow* window, unsigned int c);

// variables
Window win;
double last_frame_time;
double delta_time;

static bool s_glfw_initialized = false;
static void GLFWErrorCallback(int error, const char *message) {
  fprintf(stderr, "GLFW Error (%d): %s\n", error, message);
}
int Setup(int w, int h, const char *title) {
  if (win.win) {
    fprintf(stderr, "Window Setup failed: window already exists");
    return 1;
  }
  /* assert(win.id!=nullptr, "Window Setup failed! Window already exists"); */

  win.title = title;
  win.w = w;
  win.h = h;

  /* logci("Creating window {0} ({1}, {2})", props.title, props.width, */
  /*       props.height); */

  if (!s_glfw_initialized) {
    // TODO: glfwterminate on system shutdown
    int success = glfwInit();
    /* arc_core_assert(success, "Could not initialize GLFW!"); */
    glfwSetErrorCallback(GLFWErrorCallback);

    s_glfw_initialized = true;
  }
  win.win = glfwCreateWindow(win.w, win.h, win.title, nullptr, nullptr);

  glfwMakeContextCurrent((GLFWwindow *)win.win);
  int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!status) {
    fprintf(stderr, "Failed to initialize Glad!");
    return 1;
  }

  printf("OpenGL Info:\n");
  printf("  Vendor: %s,\n", glGetString(GL_VENDOR));
  printf("  Renderer: %s,\n", glGetString(GL_RENDERER));
  printf("  Version: %s,\n", glGetString(GL_VERSION));

  SetVsync(true);

  glfwSetCharCallback((GLFWwindow *)win.win,
                      ImGui_CharCallback);
  /* glfwSetMouseButtonCallback( */
  /*     (GLFWwindow *)win.win, */
  /*     [](GLFWwindow *window, int button, int action, int mods) {}); */
  /* glfwSetScrollCallback( */
  /*     (GLFWwindow *)win.win, */
  /*     [](GLFWwindow *window, double x_offset, double y_offset) {}); */
  /* glfwSetCursorPosCallback( */
  /*     (GLFWwindow *)win.win, */
  /*     [](GLFWwindow *window, double x_pos, double y_pos) {}); */

  ImGuiSetup();
  return 0;
}
int Dispose() {
  if (win.win) {
    glfwDestroyWindow((GLFWwindow *)win.win);
    win.win = nullptr;
    ImGuiDispose();
    return 0;
  }
  return 1;
}
void SetResizeCallback(ResizeFn fn) {
  win.resize_fn = fn;
  glfwSetWindowSizeCallback(
      (GLFWwindow *)win.win,
      [](GLFWwindow *, int w, int h) { 
      win.resize_fn(w, h); });
}
void SetCloseCallback(CloseFn fn) {
  win.close_fn = fn;
  glfwSetWindowCloseCallback((GLFWwindow *)win.win,
                             [](GLFWwindow *) { win.close_fn(); });
}
void SetKeyCallback(KeyPressFn fn_press, KeyReleaseFn fn_release) {
  win.key_press_fn = fn_press;
  win.key_release_fn = fn_release;

  glfwSetKeyCallback(
      (GLFWwindow *)win.win,
      [](GLFWwindow *window, int key, int scan_code, int action, int mods) {
      ImGui_KeyCallback(window,key,scan_code,action,mods);
        switch (action) {
        case GLFW_PRESS:
          win.key_press_fn(key, mods, false); // TODO: make custom key codes
          break;
        case GLFW_RELEASE:
          win.key_release_fn(key, mods);
          break;
        case GLFW_REPEAT:
          win.key_press_fn(key, mods, true); // make custom key codes
          break;
        }
      });
}
void SetButtonCallback(ButtonPressFn fn_press, ButtonReleaseFn fn_release) {
  win.button_press_fn = fn_press;
  win.button_release_fn = fn_release;

  glfwSetMouseButtonCallback(
      (GLFWwindow *)win.win,
      [](GLFWwindow *window, int button, int action, int mods) {
        switch (action) {
        case GLFW_PRESS: {
          win.button_press_fn(button, mods);
          break;
        }
        case GLFW_RELEASE: {
          win.button_release_fn(button, mods);
          break;
        }
        }
      });
}
void SetScrollCallback(ScrollFn fn) {
  win.scroll_fn = fn;
  glfwSetScrollCallback((GLFWwindow *)win.win,
                        [](GLFWwindow *window, double d_x, double d_y) {
                        ImGui_ScrollCallback(window,d_x,d_y);
                          win.scroll_fn(d_x, d_y);
                        });
}
void SetMoveCallback(MoveFn fn) {
  win.move_fn = fn;

  glfwSetCursorPosCallback(
      (GLFWwindow *)win.win,
      [](GLFWwindow *window, double x, double y) { win.move_fn(x, y); });
}
// custom callbacks
void SetArcNotifyCallback(ArcNotifyFn fn){
  win.arc_notify_fn = fn;
}
void SendEventArcNotify(const char* message){
  win.arc_notify_fn(message);
}
void SetVsync(bool val) {
  assert(s_glfw_initialized);
  if (val) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }
  win.vsync = val;
}
void SetSize(int w, int h) { glfwSetWindowSize((GLFWwindow *)win.win, w, h); }
// Functions
static const char* ImGui_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void ImGui_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{


    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGui_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    ImGuiIO& io = ImGui::GetIO();
    if (key >= 0 && key < IM_ARRAYSIZE(io.KeysDown))
    {
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;
    }

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void ImGui_WindowFocusCallback(GLFWwindow* window, int focused)
{

    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void ImGui_CursorEnterCallback(GLFWwindow* window, int entered)
{


}

void ImGui_CharCallback(GLFWwindow* window, unsigned int c)
{

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}
void ImGuiSetup() {
  bool err = gladLoadGL() == 0;
  if (err) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return;
  } else {
    printf("dela :(\n");
  }
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard
  // Controls
  /* io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen; */
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

  // io.Fonts->AddFontFromFileTTF("/home/adolin/CLionProjects/arc_engine/test_core/assets/roboto_font/Roboto-Medium.ttf",
  // 18.0f); io.FontDefault =
  // io.Fonts->AddFontFromFileTTF("/home/adolin/CLionProjects/arc_engine/test_core/assets/roboto_font/Roboto-Medium.ttf",
  // 18.0f);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();

  // SetDarkThemeColors();

  GLFWwindow *window = static_cast<GLFWwindow *>(win.win);
  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL3_Init("#version 410");

  // Setup Platform/Renderer bindings
}

void ImGuiDispose() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

int Run() {
  win.running = true;
  if (win.update_game_fn == nullptr || win.update_imgui_fn == nullptr) {
    fprintf(stderr,
            "Run error: game update fn and imgui update fn must be set\n");
    return 1;
  }
  last_frame_time = glfwGetTime();
  while (win.running) {
    double time = glfwGetTime();
    delta_time = time - last_frame_time;
    last_frame_time = time;

    glfwPollEvents();
    // game
    win.update_game_fn(delta_time);
    // ImGUI
    {
      // begin
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      win.update_imgui_fn(delta_time);
      // end
      ImGuiIO &io = ImGui::GetIO();
      io.DisplaySize = ImVec2((float)win.w, (float)win.h);
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers((GLFWwindow *)win.win);
  }
  return 0;
}
void Close() {
  win.running = false;
  printf("closed\n");
}
void SetUpdate(UpdateFn fn) { win.update_game_fn = fn; }
void SetImGuiUpdate(UpdateFn fn) { win.update_imgui_fn = fn; }

void Resize(int w, int h) {
  win.w = w;
  win.h = h;
}
int Width() { return win.w; }
int Height() { return win.h; }
void HideCursor() {
  glfwSetInputMode((GLFWwindow *)win.win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void ShowCursor() {
  glfwSetInputMode((GLFWwindow *)win.win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
} // namespace arc::window
