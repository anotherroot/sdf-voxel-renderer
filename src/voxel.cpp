#include "voxel.h"
#include "fstream"
#include "queue"
#include "texture.h"
#include "unordered_map"
#define OGT_VOX_IMPLEMENTATION
#include <ogt_vox.h>
namespace arc {


 Scope<VoxelModel> VoxelModel::LoadModel(const std::string &file_name){
   auto ret = CreateScope<VoxelModel>();
  if (ret == nullptr)
    return nullptr;

  std::ifstream file(file_name);
  ARC_ASSERT(file.is_open(),"File %s not found!\n", file_name.c_str());

  file.seekg(0, std::ios::end);
  int s = file.tellg();
  file.seekg(0, std::ios::beg);

  uint8_t *buff = new uint8_t[s];
  file.read(reinterpret_cast<char *>(buff), s);
  const ogt_vox_scene *scene = ogt_vox_read_scene(buff, s);
  delete[] buff;
  file.close();

  printf("Groups: %d\n", scene->num_groups);
  printf("Instances: %d\n", scene->num_instances);
  printf("Layers: %d\n", scene->num_layers);
  printf("Models: %d\n", scene->num_models);
  auto ogt_model = scene->models[scene->instances[0].model_index];

  // get sizes
  glm::ivec3 raw_size = {ogt_model->size_x, ogt_model->size_y,
                         ogt_model->size_z};
  logi("raw: (%d, %d, %d)", raw_size[0], raw_size[1], raw_size[2]);
  ret->m_raw_size = {raw_size.x, raw_size.z, raw_size.y}; // NOTE: y and z sawpped
  auto fixSize = [](glm::ivec3 a) {
    for (int i = 0; i < 3; ++i) {
      for (int j = 7; j >= 2; --j) {
        if (a[i] & (1 << j)) {
          if ((a[i] & ((1 << j) - 1)))
            a[i] = 1 << (j + 1);
          break;
        } else if (j == 2) {
          a[i] = 4;
        }
      }
    }
    return a;
  };
  glm::ivec3 size = fixSize(raw_size);
  ret->m_fixed_size = {size.x, size.z, size.y}; // NOTE: y and z sawpped
  logi("fixed: (%d, %d, %d)", size[0], size[1], size[2]);

  // allocate
  int full_size = size.x * size.y * size.z;
  ret->m_data = new uint8_t[full_size * 4];
  if(ret->m_data==nullptr){
    loge("Voxel Data dynamiuc allocation failed!\n");
    return ret;
  }
  for (int i = 0; i < full_size * 4; ++i) {
    ret->m_data[i] = 1;
  }

  for (int x = 0; x < ogt_model->size_x; ++x) {
    for (int y = 0; y < ogt_model->size_y; ++y) {
      for (int z = 0; z < ogt_model->size_z; ++z) {
        int i_in = z * ogt_model->size_x * ogt_model->size_y +
                   y * ogt_model->size_x + x;
        glm::vec4 v(x, y, z, 0);
        int i_out =
            ((ogt_model->size_y - y - 1) * ret->m_fixed_size.x * ret->m_fixed_size.y +
             z * ret->m_fixed_size.x + x) *
            4;
        uint8_t ci = ret->m_data[i_out] = ogt_model->voxel_data[i_in];

        if (ci) {
          auto col = scene->palette.color[ci];
          ret->m_data[i_out + 0] = col.r;
          ret->m_data[i_out + 1] = col.g;
          ret->m_data[i_out + 2] = col.b;
          ret->m_data[i_out + 3] = 0;
        } else {
          ret->m_data[i_out + 3] = 1;
        }
      }
    }
  }

  return ret;
}

Scope<VoxelModel> VoxelModel::Create(const std::string &file_name) {
  auto model = LoadModel(file_name);
  const auto& s = model->m_fixed_size;
  tex::Create3D(model->m_redner_id, s.x,s.y,s.z,tex::RGBA,tex::UNSIGNED_BYTE,model->m_data);
  return model;
}
// create voxel
Scope<VoxelModel> VoxelModel::Create(const Size3D &s, uint8_t *data){
  auto model = CreateScope<VoxelModel>(); 
  model->m_data = new uint8_t[s.w*s.h*s.d*4];
  model->m_raw_size = {s.w,s.h,s.d};
  model->m_fixed_size = {s.w,s.h,s.d};
  tex::Create3D(model->m_redner_id, s.w, s.h, s.d, tex::RGBA, tex::UNSIGNED_BYTE, data);
  return model;
}

VoxelModel::VoxelModel(){
}
VoxelModel::~VoxelModel(){
  if(this->m_data){
    logi("Clearing data for a voxel model");
    delete[] this->m_data;
  }
}
} // namespace arc::voxel
