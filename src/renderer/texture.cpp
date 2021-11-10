#include "texture.h"
#include "assert.h"
#include "glad/glad.h"
#include "vector"
#include <renderer_core.h>
namespace arc::tex {
struct Texture {
  RenderID render_id;
  int width, height, depth;

  unsigned int internal_format, based_format, data_type;
};
struct Tex {
  bool set;
  union {
    int next_empty;
    Texture tex;
  };
};

std::vector<Tex> textures(1, {false, -1});

namespace impl {
unsigned int getFilter(unsigned int type) {
  if (type == LINEAR)
    return GL_LINEAR;
  if (type == NEAREST)
    return GL_NEAREST;
  assert(false);
}

unsigned int getWrap(unsigned int type) {
  if (type == CLAMP_TO_EDGE)
    return GL_CLAMP_TO_EDGE;
  if (type == REPEAT)
    return GL_REPEAT;
  assert(false);
}
unsigned int getDataType(unsigned int type) {
  // data type

  if (type == UNSIGNED_BYTE)
    return GL_UNSIGNED_BYTE;
  if (type == BYTE)
    return GL_BYTE;
  if (type == UNSIGNED_SHORT)
    return GL_UNSIGNED_SHORT;
  if (type == SHORT)
    return GL_SHORT;
  if (type == UNSIGNED_INT)
    return GL_UNSIGNED_INT;
  if (type == INT)
    return GL_INT;
  if (type == HALF_FLOAT)
    return GL_HALF_FLOAT;
  if (type == FLOAT)
    return GL_FLOAT;

  assert(false);
}
unsigned int getBaseFormat(unsigned int type) {
  if (type == DEPTH_COMPONENT)
    return GL_DEPTH_COMPONENT;
  if (type == DEPTH_STENCIL)
    return GL_DEPTH_STENCIL;
  if (type == RED)
    return GL_RED;
  if (type == RG)
    return GL_RG;
  if (type == RGB)
    return GL_RGB;
  if (type == RGBA)
    return GL_RGBA;
  assert(false);
}
unsigned int getInteralFormat(unsigned int format, unsigned int type) {
  assert(format != DEPTH_COMPONENT);
  assert(format != DEPTH_STENCIL);

  if (format == RED) {
    if (type == UNSIGNED_BYTE)
      return GL_R8;
    if (type == BYTE)
      return GL_R8;
    if (type == UNSIGNED_SHORT)
      return GL_R16;
    if (type == SHORT)
      return GL_R16;
    if (type == UNSIGNED_INT)
      return GL_R32UI;
    if (type == INT)
      return GL_R32I;
    if (type == HALF_FLOAT)
      return GL_R16F;
    if (type == FLOAT)
      return GL_R32F;
    assert(false);
  }

  if (format == RG) {
    if (type == UNSIGNED_BYTE)
      return GL_RG8;
    if (type == BYTE)
      return GL_RG8;
    if (type == UNSIGNED_SHORT)
      return GL_RG16;
    if (type == SHORT)
      return GL_RG16;
    if (type == UNSIGNED_INT)
      return GL_RG32UI;
    if (type == INT)
      return GL_RG32I;
    if (type == HALF_FLOAT)
      return GL_RG16F;
    if (type == FLOAT)
      return GL_RG32F;
    assert(false);
  }

  if (format == RGB) {
    if (type == UNSIGNED_BYTE)
      return GL_RGB8;
    if (type == BYTE)
      return GL_RGB8;
    if (type == UNSIGNED_SHORT)
      return GL_RGB16;
    if (type == SHORT)
      return GL_RGB16;
    if (type == UNSIGNED_INT)
      return GL_RGB32UI;
    if (type == INT)
      return GL_RGB32I;
    if (type == HALF_FLOAT)
      return GL_RGB16F;
    if (type == FLOAT)
      return GL_RGB32F;
    assert(false);
  }

  if (format == RGBA) {
    if (type == UNSIGNED_BYTE)
      return GL_RGBA8;
    if (type == BYTE)
      return GL_RGBA8;
    if (type == UNSIGNED_SHORT)
      return GL_RGBA16;
    if (type == SHORT)
      return GL_RGBA16;
    if (type == UNSIGNED_INT)
      return GL_RGBA32UI;
    if (type == INT)
      return GL_RGBA32I;
    if (type == HALF_FLOAT)
      return GL_RGBA16F;
    if (type == FLOAT)
      return GL_RGBA32F;
    assert(false);
  }

  assert(false);
}
} // namespace impl
TextureID NextID() {
  if (textures[0].next_empty == -1) {
    textures.push_back({});
    return textures.size() - 1;
  }
  TextureID id = textures[0].next_empty;
  textures[0].next_empty = textures[id].next_empty;
  return id;
}
void Create2D(TextureID &id, int width, int height, unsigned int format,
              unsigned int type, void *data) {
  id = NextID();
  textures[id].set = true;
  auto &tex = textures[id].tex;

  tex.width = width;
  tex.height = height;
  tex.internal_format = impl::getInteralFormat(format, type);
  tex.based_format = impl::getBaseFormat(format);
  tex.data_type = impl::getDataType(type);

  glGenTextures(1, &tex.render_id);
  glBindTexture(GL_TEXTURE_2D, tex.render_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //                             internal format                     data format
  glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, width, height, 0,
               tex.based_format, tex.data_type, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
void Create3D(TextureID &id, int width, int height, int depth,
              unsigned int format, unsigned int type, void *data) {
  id = NextID();
  textures[id].set = true;
  auto &tex = textures[id].tex;
  tex.width = width;
  tex.height = height;
  tex.internal_format = impl::getInteralFormat(format, type);
  tex.based_format = impl::getBaseFormat(format);
  tex.data_type = impl::getDataType(type);

  glCreateTextures(GL_TEXTURE_3D, 1, &tex.render_id);
  /* glBindTexture(GL_TEXTURE_3D, tex->render_id); */
  glTextureParameteri(tex.render_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex.render_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex.render_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTextureParameteri(tex.render_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(tex.render_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //                             internal format                     data format
  glTextureStorage3D(tex.render_id, 1, tex.internal_format, width, height,
                     depth);
  glTextureSubImage3D(tex.render_id, 0, 0, 0, 0, width, height, depth,
                      tex.based_format, tex.data_type, data);
  /* glBindTexture(GL_TEXTURE_3D, 0); */
}
void Dispose(TextureID id) {
  if (textures[id].set) {
    textures[id].set = false;
    glDeleteTextures(1, &textures[id].tex.render_id);
    textures[id].next_empty = textures[0].next_empty;
    textures[0].next_empty = id;
  }
}
void Dispose() {
  for (int i = 0; i < textures.size(); ++i) {
    Dispose(i);
  }
}
void Bind(TextureID id, unsigned int slot) {
  glBindTextureUnit(slot, textures[id].tex.render_id);
}

void SetFilter2D(TextureID id, const FilterType type) {
  glBindTexture(GL_TEXTURE_2D, textures[id].tex.render_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, impl::getFilter(type));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, impl::getFilter(type));
  glBindTexture(GL_TEXTURE_2D, 0);
}
void SetWrap2D(TextureID id, unsigned int type) {
  glBindTexture(GL_TEXTURE_2D, textures[id].tex.render_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, impl::getWrap(type));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, impl::getWrap(type));
  glBindTexture(GL_TEXTURE_2D, 0);
}
void Resize2D(TextureID id, int width, int height, unsigned int format,
              unsigned int type, void *data) {

  Texture &tex = textures[id].tex;
  tex.width = width;
  tex.height = height;
  tex.internal_format = impl::getInteralFormat(format, type);
  tex.based_format = impl::getBaseFormat(format);
  tex.data_type = impl::getDataType(type);

  glBindTexture(GL_TEXTURE_2D, tex.render_id);
  glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, width, height, 0,
               tex.based_format, tex.data_type, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
void Resize2D(TextureID id, int width, int height) {
  Texture &tex = textures[id].tex;
  tex.width = width;
  tex.height = height;
  glBindTexture(GL_TEXTURE_2D, tex.render_id);
  glTexImage2D(GL_TEXTURE_2D, 0, tex.internal_format, width, height, 0,
               tex.based_format, tex.data_type, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}
void SetFilter3D(TextureID id, const FilterType type) {
  glBindTexture(GL_TEXTURE_3D, textures[id].tex.render_id);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, impl::getFilter(type));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, impl::getFilter(type));
  glBindTexture(GL_TEXTURE_3D, 0);
}
void SetWrap3D(TextureID id, unsigned int type) {
  glBindTexture(GL_TEXTURE_3D, textures[id].tex.render_id);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, impl::getWrap(type));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, impl::getWrap(type));
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, impl::getWrap(type));
  glBindTexture(GL_TEXTURE_3D, 0);
}
void Resize3D(TextureID id, int width, int height, int depth,
              unsigned int format, unsigned int type, void *data) {
  Texture &tex = textures[id].tex;
  tex.width = width;
  tex.height = height;
  tex.depth = depth;
  tex.internal_format = impl::getInteralFormat(format, type);
  tex.based_format = impl::getBaseFormat(format);
  tex.data_type = impl::getDataType(type);

  glBindTexture(GL_TEXTURE_3D, tex.render_id);
  glTexImage3D(GL_TEXTURE_3D, 0, tex.internal_format, width, height, depth, 0,
               tex.based_format, tex.data_type, data);
  glBindTexture(GL_TEXTURE_3D, 0);
}
void BindImage(TextureID id, unsigned int unit,bool layered) {
  glBindImageTexture(unit, textures[id].tex.render_id, 0, layered?GL_TRUE:GL_FALSE, 0,
                     GL_WRITE_ONLY, textures[id].tex.internal_format);
}

unsigned int GetID(TextureID id) { return textures[id].tex.render_id; }
} // namespace arc::texture
