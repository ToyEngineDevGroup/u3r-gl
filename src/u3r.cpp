#include "u3r.hpp"
#include <GL/gl.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <memory>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

namespace U3R{
  Instance::Instance(){
    
  }

  Instance::~Instance(){

  }

  void Instance::set_camera(std::shared_ptr<Camera> cam){
    m_cam = cam;
  }

  void Instance::clear(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Instance::render(std::shared_ptr<Pass> pass, std::vector<std::shared_ptr<RenderObject>>& objs){
    glUseProgram(pass->m_gl_shader_id);
    glEnable(GL_DEPTH_TEST);
    glUniformMatrix4fv(glGetUniformLocation(pass->m_gl_shader_id, "u_view_matrix"), 1, GL_FALSE, glm::value_ptr(m_cam->m_t));
    glUniform3fv(glGetUniformLocation(pass->m_gl_shader_id, "u_view_position"), 1, glm::value_ptr(m_cam->m_pos));
    for(auto obj : objs){
      glBindVertexArray(obj->m_gl_vao_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->m_gl_ebo_id);
      glDrawElements(GL_TRIANGLES, obj->m_indices.size(), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  Camera::Camera(){
    
  }

  Camera::~Camera(){

  }

  void Camera::look_at(U3R::Vector3 p, U3R::Vector3 t){
    U3R::Vector3 up(0.0f, 1.0f, 0.0f);
    m_t = glm::lookAt(p, t, up);
    m_pos = p;
  }

  RenderObject::RenderObject(){
    m_gl_ebo_id = 0;
    m_gl_vao_id = 0;
  }

  RenderObject::~RenderObject(){
    if(m_gl_vao_id)glDeleteVertexArrays(1, &m_gl_vao_id);
    if(m_gl_vbo_ids.size()>0)glDeleteBuffers(m_gl_vbo_ids.size(), m_gl_vbo_ids.data());
    if(m_gl_ebo_id)glDeleteBuffers(1, &m_gl_ebo_id);
  }

  U3R::Array* RenderObject::get_input_ptr(unsigned int x){
    return m_input[x].get();
  }

  std::vector<unsigned int>* RenderObject::get_indices_ptr(){
    return &m_indices;
  }

  void RenderObject::set_input_structure(std::vector<unsigned int> sizes){
    unsigned int sz_inputs = sizes.size();
    m_gl_vbo_ids.resize(sz_inputs);
    glGenBuffers(sz_inputs, m_gl_vbo_ids.data());
    glGenVertexArrays(1, &m_gl_vao_id);
    glBindVertexArray(m_gl_vao_id);
    for(int i=0; i<sz_inputs; ++i){
      glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_ids[i]);
      glVertexAttribPointer(i, sizes[i], GL_FLOAT, GL_FALSE, 0, nullptr);
      glEnableVertexAttribArray(i);
      m_input.push_back(std::shared_ptr<U3R::Array>(new U3R::Array));
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenBuffers(1, &m_gl_ebo_id);
  }

  void RenderObject::load_input(){
    for(int i=0; i<m_gl_vbo_ids.size(); ++i){
      glBindBuffer(GL_ARRAY_BUFFER, m_gl_vbo_ids[i]);
      glBufferData(GL_ARRAY_BUFFER, m_input[i]->size()*sizeof(U3R::Number), m_input[i]->data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  Pass::Pass(){

  }

  Pass::~Pass(){

  }

  std::unique_ptr<std::string> __read_all(std::string filename){
    std::unique_ptr<std::string> compiled(new std::string);
    std::ifstream in(filename);

    std::ostringstream oss;
    oss<<in.rdbuf();
    *compiled = oss.str();
    
    return std::move(compiled);
  }
  
  void Pass::compile_shader_from_file(std::string vsfilename, std::string fsfilename){
    std::unique_ptr<std::string> vs_source = __read_all(vsfilename);
    std::unique_ptr<std::string> fs_source = __read_all(fsfilename);
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    char * vs_c_str = const_cast<char*>(vs_source->data());
    char * fs_c_str = const_cast<char*>(fs_source->data());
    int vs_len = vs_source->size();
    int fs_len = fs_source->size();
    glShaderSource(vs, 1, &vs_c_str, &vs_len);
    glShaderSource(fs, 1, &fs_c_str, &fs_len);
    glCompileShader(vs);
    char log[512];
    glGetShaderInfoLog(vs, 512, 0, log);
    std::cout<<log<<std::endl;
    glGetShaderInfoLog(fs, 512, 0, log);
    std::cout<<log<<std::endl;
    glCompileShader(fs);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    m_gl_shader_id = program;
  }

}
