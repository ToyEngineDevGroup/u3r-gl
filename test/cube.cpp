#include "u3r.hpp"
#include <glm/ext.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void framebuffer_size_callback(GLFWwindow * window, int w, int h){
  glViewport(0, 0, w, h);
}

int main(){

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(1024, 768, "Cube", nullptr, nullptr);

  if(!window){
    std::cout<<"Cannot create window!"<<std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cout<<"Cannot load functions!"<<std::endl;
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, 1024, 768);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // 创建实例
  std::shared_ptr<U3R::Instance> p_instance(new U3R::Instance);

  // 创建相机
  std::shared_ptr<U3R::Camera> p_cam(new U3R::Camera);
  p_cam->look_at(U3R::Vector3(1.0f, 1.0f, 1.0f), U3R::Vector3(0.0f, 0.0f, 0.0f));
  p_instance->set_camera(p_cam);
  
  // 创建Pass
  std::shared_ptr<U3R::Pass> p_pass(new U3R::Pass);
  p_pass->compile_shader_from_file(std::string("res/shader.vs"), std::string("res/shader.fs"));
  
  // 创建可渲染对象
  std::shared_ptr<U3R::RenderObject> p_object(new U3R::RenderObject);
  std::vector<unsigned int> vec_input_struct;
  vec_input_struct.push_back(3);
  p_object->set_input_structure(vec_input_struct);
  float vertices[] = {
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f
  };
  p_object->get_input_ptr(0)->assign(vertices, vertices+sizeof(vertices)/sizeof(float));
  unsigned int indices[] = {
    0, 1, 4,
    1, 4, 5,
    1, 2, 5,
    2, 5, 6,
    2, 3, 6,
    3, 6, 7,
    3, 4, 7,
    4, 0, 3
  };
  p_object->get_indices_ptr()->assign(indices, indices+sizeof(indices)/sizeof(unsigned int));
  p_object->load_input();

  for(auto x : *p_object->get_input_ptr(0)) std::cout<<x<<" ";
  std::cout<<std::endl;
  for(auto x : *p_object->get_indices_ptr()) std::cout<<x<<" ";

  std::vector<std::shared_ptr<U3R::RenderObject>> objs;
  objs.push_back(p_object);

  glm::vec3 eye(1.0f, 0.0f, 0.0f);

  float t = 0.00;
  
  while(!glfwWindowShouldClose(window)){
    p_cam->look_at(eye, U3R::Vector3(0.0f, 0.0f, 0.0f));
    eye = U3R::Vector3(cos(t), 1.0f, sin(t));
    t = t+0.01;

    p_instance->clear();
    p_instance->render(p_pass, objs);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  return 0;
}
