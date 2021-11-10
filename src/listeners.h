#ifndef ARC_LISTENERS
#define ARC_LISTENERS
namespace arc {


// mybe in the future
/* class TouchListener { */
/*   virtual bool OnTouchPress(int id, float x, float y) = 0; */
/*   virtual bool OnTouchRelease(int id, float x, float y) = 0; */
/*   virtual bool OnTouchMove(int id, float dx, float dy) = 0; */
/*   virtual bool OnMultiGesture(float x, float y, float dist, float theta) = 0; */
/* }; */

class MouseListener {
public:

  virtual bool OnButtonPressed(int button, int mode) {return false;}
  virtual bool OnButtonReleased(int button) {return false;}
  virtual bool OnScroll(double x_offset, double y_offset){return false;}
  virtual bool OnMove(double x_pos, double y_pos) {return false;}

};

class KeyboardListener {
public:
  virtual bool OnKeyPressed(int key, int mode, bool repeat) {return false;}
  virtual bool OnKeyReleased(int key,int mode) {return false;}
  virtual bool OnChar(unsigned int key_code) {return false;}
};

class WindowListener {
public:
  virtual bool OnWindowResize(int width, int height) {return false;}
  virtual bool OnWindowClose() {return false;}
};
//...
} // namespace arc
#endif // ARC_ENGINE_LISTENERS_H
