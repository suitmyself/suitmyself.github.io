/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl shader
 * @version    : 1.0
 */

#pragma once

#include <iostream>
#include <unordered_map>

#include <glm/fwd.hpp>

#include "core/file/file_watcher.h"
#include "core/file/file_async_watcher.h"

namespace luna {

class GLTexture;

class GLShader
{
public:

    GLShader() = default;

    //disable copy
    GLShader(const GLShader & rhs) = delete;
    GLShader & operator = (const GLShader & rhs) = delete;

    //enable move
    GLShader(GLShader && rhs) noexcept;
    GLShader & operator = (GLShader && rhs) noexcept;

    ~GLShader();

    /*
    GLShader(const char * vertex_shader_source,
             const char * fragment_shader_source,
             const char * geometry_shader_source = nullptr,
             const char * tess_control_shader_source = nullptr,
             const char * tess_evaluation_shader_source = nullptr);
    */

    void createFromCStyleString(const char * vertex_shader_source,
                                const char * fragment_shader_source,
                                const char * geometry_shader_source = nullptr,
                                const char * tess_control_shader_source = nullptr,
                                const char * tess_evaluation_shader_source = nullptr);

    void  createFromFile(const std::string & vertex_shader_file,
                         const std::string & fragment_shader_file,
                         const std::string & geometry_shader_file = {},
                         const std::string & tess_control_shader_file = {},
                         const std::string & tess_evaluation_shader_file = {});

    void createFromString(const std::string & vertex_shader_str,
                          const std::string & fragment_shader_str,
                          const std::string & geometry_shader_str = {},
                          const std::string & tess_control_shader_str = {},
                          const std::string & tess_evaluation_shader_str = {});

    void destroy();

    void use() const;
    void unUse() const;

    //uniform setter
    bool setBool(const std::string & name, bool val);
    bool setInt(const std::string & name, int val);
    bool setFloat(const std::string & name, float val);

    bool setBoolArray(const std::string& name, unsigned int num, const bool* val);
    bool setIntArray(const std::string& name, unsigned int num, const int* val);
    bool setFloatArray(const std::string& name, unsigned int num, const float* val);

    bool setBoolArray(const std::string& name, const std::vector<bool>& val);
    bool setIntArray(const std::string& name, const std::vector<int>& val);
    bool setFloatArray(const std::string& name, const std::vector<float>& val);

    bool setVec2(const std::string & name, const glm::vec2 & val);
    bool setVec2(const std::string & name, float x, float y);
    bool setVec2(const std::string& name, const float* val_ptr);

    bool setVec3(const std::string & name, const glm::vec3 & val);
    bool setVec3(const std::string & name, float x, float y, float z);
    bool setVec3(const std::string & name, const float* val_ptr);

    bool setVec4(const std::string & name, const glm::vec4 & val);
    bool setVec4(const std::string & name, float x, float y, float z, float w);
    bool setVec4(const std::string & name, const float* val_ptr);

    bool setMat2(const std::string & name, const glm::mat2 & val);
    bool setMat3(const std::string & name, const glm::mat3 & val);
    bool setMat4(const std::string & name, const glm::mat4 & val);

    bool setTexture(const std::string & name, int tex_id);

    bool setTexture(const std::string & name, const GLTexture& tex);

    bool setAttribLocation(const std::string & name, int loc);

    int getAttribLocation(const std::string & name) const;

    bool setUBO(const std::string & name, int ubo_id, int binding_point);

    bool isValid() const;

    unsigned int id() const;

    void enableAutoReloadFromFile(bool enable); //enable auto reload if created from file, for hot reload
    void setAutoReloadCallback(const std::function<void()>& callback); //set callback for auto reload

private:
    unsigned int m_program = 0;

    int m_cur_tex_offset = 0;
    std::unordered_map<std::string, int> m_tex_map; // <name, texture unit id>

    //-------------------

    bool m_is_created_from_file = false;
    std::vector<std::string> m_file_paths;

    bool m_auto_reload_from_file = false;
    mutable std::vector<FileAsyncWatcher> m_file_watchers;

    std::function<void()> m_auto_reload_callback;
};

} // end of namespace luna
