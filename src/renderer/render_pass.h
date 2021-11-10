#ifndef ARC_RENDER_PASS
#define ARC_RENDER_PASS
#include "shader.h"
#include "texture.h"
#include "vector"
namespace arc::pass{
  struct Frame{
    std::string name;
    int format;
    int type;
    int filter;
  };

  void Create(unsigned int &id,int w, int h, const std::string& shader_path,const std::initializer_list<Frame> frames);
  void Dispose(unsigned int id);
  void Dispose();
  void Resize(unsigned int id, int width, int height);
  void Bind(unsigned int id);
  void Unbind();
  unsigned int GetAttachment(unsigned int id, const std::string &name);
  void BindFrame(unsigned int id, const std::string& name, int slot);
  unsigned int GetShader(unsigned int id);

}
#endif
