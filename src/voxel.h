#ifndef ARC_VOXEL_MODEL
#define ARC_VOXEL_MODEL
#include "glm/glm.hpp"
#include "string"
#include "core.h"
#include <renderer_core.h>
namespace arc {

class VoxelModel {
public:
  static Scope<VoxelModel> Create(const std::string &file_name);
  static Scope<VoxelModel> Create(const Size3D &size,  uint8_t *data);
  VoxelModel();
  ~VoxelModel();

  inline const glm::ivec3 &raw_size() const { return m_raw_size; }
  inline const glm::ivec3 &fixed_size() const { return m_fixed_size; }
  inline RenderID render_id() const { return m_redner_id; }

private:
  static Scope<VoxelModel> LoadModel(const std::string &file_name);

private:
  uint8_t *m_data;
  glm::ivec3 m_raw_size, m_fixed_size;
  RenderID m_redner_id;
};

} // namespace arc
#endif
