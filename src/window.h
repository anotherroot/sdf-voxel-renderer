#ifndef ARC_WINDOW
#define ARC_WINDOW
namespace arc::window {

typedef void (*UpdateFn)(double);
typedef void (*CloseFn)();
typedef void (*ResizeFn)(int, int);
typedef void (*KeyPressFn)(int, int, bool);
typedef void (*KeyReleaseFn)(int, int);
typedef void (*ButtonPressFn)(int, int);
typedef void (*ButtonReleaseFn)(int, int);
typedef void (*ScrollFn)(double, double);
typedef void (*MoveFn)(double, double);
typedef void (*ArcNotifyFn)(const char*);

int Setup(int w, int h, const char *title = "Octree Renderer");
int Dispose();
int Run();
void Close();
void Resize(int w, int h);
int Width();
int Height();
void HideCursor();
void ShowCursor();

void SetVsync(bool val);
void SetSize(int w, int h);
void SetCloseCallback(CloseFn fn);
void SetResizeCallback(ResizeFn fn);

void SetKeyCallback(KeyPressFn fn_press, KeyReleaseFn fn_release);
void SetButtonCallback(ButtonPressFn fn_press, ButtonReleaseFn fn_release);
void SetScrollCallback(ScrollFn fn);
void SetMoveCallback(MoveFn fn);

void SetArcNotifyCallback(ArcNotifyFn fn);
void SendEventArcNotify(const char* message);

void SetUpdate(UpdateFn fn);
void SetImGuiUpdate(UpdateFn fn);

} // namespace arc::window
#endif
