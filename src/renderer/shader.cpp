/* standard libraries */
#include "assert.h"
#include "fstream"
#include "stdio.h"
#include "string"
#include "unordered_map"
#include "vector"
/* external libraries */
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
/* project libraries */
#include "shader.h"

namespace arc::shader {

struct Shader {
  unsigned int id;
  std::string path;

  std::unordered_map<GLuint, GLuint> shader_ids;
  std::unordered_map<std::string, GLuint> locations;
  GLuint program;
};

std::unordered_map<unsigned int, Shader *> shaders;
unsigned int next_id{0};
unsigned int bound_program;


static GLenum ShaderTypeFromString(const std::string &type) {
  if (type == "vertex")
    return GL_VERTEX_SHADER;
  if (type == "fragment")
    return GL_FRAGMENT_SHADER;
  if (type == "compute")
    return GL_COMPUTE_SHADER;
  assert(false);
}

void Process(const std::string &source,
             std::unordered_map<unsigned int, std::string> &sources) {

  const char *type_token = "#type";
  size_t type_token_length = 5;
  size_t pos = source.find(type_token, 0);
  while (pos != std::string::npos) {
    size_t eol = source.find_first_of("\r\n", pos);
    assert(eol != std::string::npos);
    size_t begin = pos + type_token_length + 1;
    std::string type = source.substr(begin, eol - begin);
    assert(type == "vertex" || type == "fragment" || type == "compute");

    size_t next_line_pos = source.find_first_of("\r\n", eol);
    pos = source.find(type_token, next_line_pos);
    sources[ShaderTypeFromString(type)] =
        source.substr(next_line_pos, pos - (next_line_pos == std::string::npos
                                                ? source.size() - 1
                                                : next_line_pos));
  }
}
void Read(const std::string &file_path, std::string &out) {
  std::ifstream in(file_path, std::ios::in | std::ios::binary);
  if (!in) {

    fprintf(stderr, "Shader error: Cant read file %s\n", file_path.c_str());
    assert(false);
  }
  in.seekg(0, std::ios::end);
  out.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(&out[0], out.size());

  in.close();
}
int Compile(Shader *shader,
            const std::unordered_map<unsigned int, std::string> &sources) {
  GLuint program = glCreateProgram();
  assert(sources.size() <= 2);
  int gl_shader_id_index = 0;
  for (auto &kv : sources) {
    GLenum type = kv.first;

    GLuint sha = glCreateShader(type);

    const GLchar *source = kv.second.c_str();
    glShaderSource(sha, 1, &source, 0);

    glCompileShader(sha);

    GLint isCompiled = 0;
    glGetShaderiv(sha, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(sha, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> infoLog(maxLength);
      glGetShaderInfoLog(sha, maxLength, &maxLength, &infoLog[0]);

      // We don't need the shader anymore.
      glDeleteShader(sha);

      // Use the infoLog as you see fit.
      fprintf(stderr, "%s\n", infoLog.data());
      return 1;
    }
    glAttachShader(program, sha);

    shader->shader_ids.insert({kv.first, sha});
  }

  shader->program = program;

  // Link our program
  glLinkProgram(program);

  // Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

    // We don't need the program anymore.
    glDeleteProgram(program);
    // Don't leak shaders either.
    for (int i = 0; i < 2; ++i) {
      glDeleteShader(shader->shader_ids[i]);
    }

    fprintf(stderr, "%s\n", infoLog.data());
    return 1;
  }

  // Always detach shaders after a successful link.
  for (int i = 0; i < 2; ++i) {
    glDetachShader(program, shader->shader_ids[i]);
  }
  return 0;
}
void Create(unsigned int &id, const std::string &path) {
  Shader *shader = new Shader();
  id = next_id++;
  shader->id = id;
  shader->path = path;
  std::unordered_map<unsigned int, std::string> sources;
  std::string source;
  Read(path, source);
  Process(source, sources);
  Compile(shader, sources);

  shaders.insert({id, shader});
}
void Dispose(Shader *shader) {
  for (auto id : shader->shader_ids) {
    glDeleteShader(id.second);
  }
  glDeleteProgram(shader->program);
  delete shader;
}
void Dispose(unsigned int id) {
  if (shaders.find(id) != shaders.end()) {
    Shader *shader = shaders[id];
    for (auto i : shader->shader_ids) {
      glDeleteShader(i.second);
    }
    glDeleteProgram(shader->program);
    delete shader;
    shaders.erase(id);
  }
}

void Dispose() {
  Unbind();
  for (auto shader : shaders) {
    Dispose(shader.second);
  }
  shaders.clear();
}

void Reload(unsigned int id) {
  auto it = shaders.find(id);
  if (it == shaders.end()) {
    fprintf(stderr, "Shader error: id %d doesn't exist!\n", id);
    return;
  }
  Shader *shader = new Shader();
  std::unordered_map<unsigned int, std::string> sources;
  std::string source;
  shader->id = id;
  shader->path = it->second->path;
  Read(it->second->path, source);
  Process(source, sources);
  if (Compile(shader, sources)) {
    delete shader;
    return;
  }

  Dispose(it->second);
  shaders.erase(id);
  shaders.insert({id, shader});
}
void Bind(unsigned int id) {
  bound_program = id;
  glUseProgram(shaders[id]->program);
}
void Unbind() { glUseProgram(0); }

std::unordered_map<std::string, GLint> locations;
void SetInt(int location, int value) { glUniform1i(location, value); }

void SetIntArray(int location, int *values, unsigned int count) {
  glUniform1iv(location, count, values);
}

void SetFloat(int location, float value) { glUniform1f(location, value); }

void SetFloat2(int location, const glm::vec2 &values) {
  glUniform2f(location, values.x, values.y);
}

void SetFloat3(int location, const glm::vec3 &values) {
  glUniform3f(location, values.x, values.y, values.z);
}

void SetFloat4(int location, const glm::vec4 &values) {
  glUniform4f(location, values.x, values.y, values.z, values.w);
}

void SetMat3(int location, const glm::mat3 &matrix) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void SetMat4(int location, const glm::mat4 &matrix) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void SetInt(unsigned int id, const std::string& location, int value){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform1i(it->second,value);
}
void SetIntArray(unsigned int id, const std::string& location, int *values, unsigned int count){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform1iv(it->second,count,values);
}
void SetFloat(unsigned int id, const std::string& location, float value){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform1f(it->second,value);
}
void SetFloat2(unsigned int id, const std::string& location, const glm::vec2 &values){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform2f(it->second,values.x, values.y);
}
void SetFloat3(unsigned int id, const std::string& location, const glm::vec3 &values){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform3f(it->second,values.x, values.y,values.z);
}
void SetFloat4(unsigned int id, const std::string& location, const glm::vec4 &values){

  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniform4f(it->second,values.x, values.y,values.z, values.w);
}
void SetMat3(unsigned int id, const std::string& location, const glm::mat3 &matrix){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniformMatrix4fv(it->second,1,GL_FALSE,glm::value_ptr(matrix));
}
void SetMat4(unsigned int id, const std::string& location, const glm::mat4 &matrix){
  Shader* shader = shaders[id];
  auto it =shader->locations.find(location);  
  if(it== shader->locations.end()){
    shader->locations.insert({location,glGetUniformLocation(shader->program,location.c_str())});
    it = shader->locations.find(location);
  }
  glUniformMatrix4fv(it->second,1,GL_FALSE,glm::value_ptr(matrix));
}


} // namespace arc::shader
