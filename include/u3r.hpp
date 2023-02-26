#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

namespace U3R{
  class Instance;
  class Camera;
  class Light;
  class RenderObject;
  class Pass;
  class Texture;

  typedef glm::vec4 Vector4;
  typedef glm::vec3 Vector3;
  typedef glm::mat4x4 Transform;
  typedef glm::vec4 Color;
  typedef float Number;

  class Instance{
  public:
    Instance();
    void set_camera(std::shared_ptr<U3R::Camera> cam); // 相机
    void set_light_source(std::shared_ptr<U3R::Light> light); // 光源
    void enable_light(bool enable); // 允许光照
    void clear();
    void render(std::shared_ptr<U3R::Pass> pass, std::vector<std::shared_ptr<U3R::RenderObject>>& objs); // 绘制
    ~Instance();
  private:
    std::shared_ptr<U3R::Camera> m_cam;
    std::shared_ptr<U3R::Light> m_light;
  };
  class Camera{
  public:
    friend class Instance;
    Camera();
    void look_at(U3R::Vector3 p, U3R::Vector3 t);
    ~Camera();
  private:
    U3R::Vector3 m_pos;
    U3R::Transform m_t;
  };
  
  class Light{
  public:
    friend class Instance;
    Light();
    void set_position(U3R::Vector3 p);
    void set_light(U3R::Color ambient, U3R::Color diffuse, U3R::Number shininess);
    ~Light();
    struct LightColor{
      U3R::Color ambient;
      U3R::Color diffuse;
      U3R::Number shininess;
    };
  private:
    U3R::Vector3 m_pos;
    U3R::Light::LightColor m_light_color;
  };

  typedef std::vector<U3R::Number> Array;
  
  class RenderObject{
  public:
    friend class Instance;
    RenderObject();
    void add_uniform(unsigned int location);
    U3R::Array* get_input_ptr(unsigned int location);
    std::vector<unsigned int>* get_indices_ptr();
    U3R::Array* get_uniform_ptr(unsigned int location);
    void set_sampler(unsigned int sampler, std::shared_ptr<U3R::Texture> tex);
    void set_input_structure(std::vector<unsigned int> sizes);
    void load_input();
    void load_uniform();
    ~RenderObject();
  private:
    std::vector<std::shared_ptr<U3R::Array>> m_input;
    std::map<unsigned int, std::shared_ptr<U3R::Array>> m_uniform;
    std::vector<std::shared_ptr<U3R::Texture>> m_tex;
    std::vector<unsigned int> m_indices;
    unsigned int m_gl_vao_id;
    std::vector<unsigned int> m_gl_vbo_ids;
    unsigned int m_gl_ebo_id;
  };

  class Pass{
  public:
    friend class Instance;
    Pass();
    void compile_shader_from_file(std::string vertex_shader_file_name, std::string fragment_shader_file_name);
    unsigned int get_index(std::string uniform_name);
    ~Pass();
  private:
    unsigned int m_gl_shader_id;
  };

  class Texture{
  public:
    friend class Instance;
    Texture();
    static const unsigned int FILTER_NEAREST = 0;
    static const unsigned int FILTER_LINEAR = 1;

    static const unsigned int COLOR_RGB = 1;
    static const unsigned int COLOR_RGBA = 2;

    static const unsigned int DATA_TYPE_U8 = 0;
    static const unsigned int DATA_TYPE_U16 = 1;

    void set_data(unsigned int width, unsigned int height, unsigned int color_mode, unsigned int data_type);
    void get_data_ptr();
    ~Texture();
  private:
    unsigned int m_width;
    unsigned int m_height;
    std::vector<unsigned char> m_data;
  };

};

