#ifndef ARC_SHADER
#define ARC_SHADER
#include "string"
#include "glm/mat4x4.hpp"
namespace arc::shader {

void Create(unsigned int &id, const std::string &path);
void Dispose(unsigned int id);
void Dispose();
void Reload(unsigned int id);

void Bind(unsigned int id);
void Unbind();

void SetInt(int location, int value);
void SetIntArray(int location, int *values, unsigned int count);
void SetFloat(int location, float value);
void SetFloat2(int location, const glm::vec2 &values);
void SetFloat3(int location, const glm::vec3 &values);
void SetFloat4(int location, const glm::vec4 &values);
void SetMat3(int location, const glm::mat3 &matrix);
void SetMat4(int location, const glm::mat4 &matrix);

void SetInt(unsigned int id, const std::string& location, int value);
void SetIntArray(unsigned int id, const std::string& location, int *values, unsigned int count);
void SetFloat(unsigned int id, const std::string& location, float value);
void SetFloat2(unsigned int id, const std::string& location, const glm::vec2 &values);
void SetFloat3(unsigned int id, const std::string& location, const glm::vec3 &values);
void SetFloat4(unsigned int id, const std::string& location, const glm::vec4 &values);
void SetMat3(unsigned int id, const std::string& location, const glm::mat3 &matrix);
void SetMat4(unsigned int id, const std::string& location, const glm::mat4 &matrix);

} // namespace arc::shader
#endif
