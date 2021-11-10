
#include "render_pass.h"
#include "glad/glad.h"
#include "unordered_map"
namespace arc::pass {
struct RenderPass {
  unsigned int fb_id;
  unsigned int shader_id;
  std::unordered_map<std::string, unsigned int> frames; // textures
  std::vector<Frame> init_frames;
};
std::unordered_map<unsigned int, RenderPass *> passes;
unsigned int next_id{0};

unsigned int draw_buffers[] = {
    GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1,  GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,  GL_COLOR_ATTACHMENT4,  GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,  GL_COLOR_ATTACHMENT7,  GL_COLOR_ATTACHMENT8,
    GL_COLOR_ATTACHMENT9,  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
    GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14};

void BuildFrames(RenderPass *pass, int w, int h) {
  // create Frame Buffer
  glGenFramebuffers(1, &pass->fb_id);
  glBindFramebuffer(GL_FRAMEBUFFER, pass->fb_id);
  glDrawBuffers(pass->init_frames.size(), draw_buffers);

  unsigned int attachment = 0;
  for (const auto &i : pass->init_frames) {
    unsigned int tex_id;
    tex::Create2D(tex_id, w, h, i.format, i.type, nullptr);
    tex::SetFilter2D(tex_id, i.filter);
    pass->frames.insert({i.name, tex_id});
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment,
                           GL_TEXTURE_2D, tex::GetID(tex_id), 0);
    attachment++;
  }
  // TODO: in the future can add depth stencil buffer here

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    printf("Frame Buffer created\n");
  } else {
    int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    fprintf(stderr, "Frame Buffer Create failed: error code %x!\n", err);

    assert(false);
  }

  // unbind frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Create(unsigned int &id, int w, int h, const std::string &shader_path,
            const std::initializer_list<Frame> frames) {
  id = next_id++;
  RenderPass *pass = new RenderPass;

  // create shader
  shader::Create(pass->shader_id, shader_path);
  // save init varibales
  pass->init_frames = frames;
  // build frame buffer and frames
  BuildFrames(pass, w, h);

  passes.insert({id, pass});
}

inline void DisposeFB(RenderPass *pass) {
  for (auto i : pass->frames) {
    tex::Dispose(i.second);
  }
  pass->frames.clear();
  glDeleteFramebuffers(1, &pass->fb_id);
}
inline void Dispose(RenderPass *pass) {
  shader::Dispose(pass->shader_id);
  DisposeFB(pass);
}

void Dispose(unsigned int id) {
  if (passes.find(id) != passes.end()) {
    Dispose(passes[id]);
    passes.erase(id);
  }
}
void Dispose() {
  for (auto i : passes) {
    Dispose(i.second);
  }
  passes.clear();
}
void Resize(unsigned int id, int width, int height) {
  if (passes.find(id) != passes.end()) {
    RenderPass *pass = passes[id];
    DisposeFB(pass);
    BuildFrames(pass, width, height);
  }
}
void Bind(unsigned int id) {
  RenderPass *pass = passes[id];
  glBindFramebuffer(GL_FRAMEBUFFER, pass->fb_id);
  shader::Bind(pass->shader_id);
}
void Unbind() {
  shader::Unbind();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int GetAttachment(unsigned int id, const std::string &name) {
  return tex::GetID(passes[id]->frames[name]);
}
void BindFrame(unsigned int id, const std::string &name, int slot) {
  tex::Bind(passes[id]->frames[name], slot);
}
  unsigned int GetShader(unsigned int id){
    return passes[id]->shader_id;
  }
} // namespace arc::pass
