/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: opengl shader
 * @version    : 1.0
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/macros/platform.h"
#include "core/task/task_queue.h"

#include "gl_include.h"
#include "gl_utility.h"

#include "gl_texture.h"

#include "gl_shader.h"


namespace luna {

/*
 * glslPrintShaderLog, output error message if fail to compile shader sources
 */

void glslPrintShaderLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten = 0;
    std::string infoLog;

    GLint result;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &result);

    // only print log if compile fails
    if (result == GL_FALSE)
    {
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

        if (infologLength > 1)
        {
            infoLog.resize(infologLength);
            glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog.data());
            LOGE("error: %s", infoLog.c_str());
        }
        throw std::runtime_error("error: can not compiler shader");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
GLShader::GLShader(const char * vertex_shader_source,
                   const char * fragment_shader_source,
                   const char * geometry_shader_source,
                   const char * tess_control_shader_source,
                   const char * tess_evaluation_shader_source)
{
    this->createFromCStyleString(vertex_shader_source,
                                 fragment_shader_source,
                                 geometry_shader_source,
                                 tess_control_shader_source,
                                 tess_evaluation_shader_source);
}
*/

GLShader::GLShader(GLShader && rhs) noexcept
{
    this->m_program = rhs.m_program;
    this->m_tex_map = std::move(rhs.m_tex_map);
    this->m_cur_tex_offset = rhs.m_cur_tex_offset;

    this->m_is_created_from_file = rhs.m_is_created_from_file;
    this->m_auto_reload_from_file = rhs.m_auto_reload_from_file;

    this->m_file_paths = std::move(rhs.m_file_paths);
    this->m_file_watchers = std::move(rhs.m_file_watchers);
    this->m_auto_reload_callback = std::move(rhs.m_auto_reload_callback);

    rhs.m_program = 0;
}

GLShader & GLShader::operator= (GLShader && rhs) noexcept
{
    this->m_program = rhs.m_program;
    this->m_tex_map = std::move(rhs.m_tex_map);
    this->m_cur_tex_offset = rhs.m_cur_tex_offset;

    this->m_is_created_from_file = rhs.m_is_created_from_file;
    this->m_auto_reload_from_file = rhs.m_auto_reload_from_file;

    this->m_file_paths = std::move(rhs.m_file_paths);
    this->m_file_watchers = std::move(rhs.m_file_watchers);
    this->m_auto_reload_callback = std::move(rhs.m_auto_reload_callback);

    rhs.m_program = 0;

    return *this;
}

GLShader::~GLShader()
{
    this->destroy();
}

void GLShader::createFromCStyleString(const char * vertex_shader_source,
                                      const char * fragment_shader_source,
                                      const char * geometry_shader_source,
                                      const char * tess_control_shader_source,
                                      const char * tess_evaluation_shader_source)
{
    this->createFromString(vertex_shader_source != nullptr ? vertex_shader_source : "",
                           fragment_shader_source != nullptr ? fragment_shader_source : "",
                           geometry_shader_source != nullptr ? geometry_shader_source : "",
                           tess_control_shader_source != nullptr ? tess_control_shader_source : "",
                           tess_evaluation_shader_source != nullptr ? tess_evaluation_shader_source : "");
}

void GLShader::createFromFile(const std::string & vertex_shader_file,
                              const std::string & fragment_shader_file,
                              const std::string & geometry_shader_file,
                              const std::string & tess_control_shader_file,
                              const std::string & tess_evaluation_shader_file)
{
    std::string shader_files[5] = { vertex_shader_file, fragment_shader_file, geometry_shader_file, tess_control_shader_file, tess_evaluation_shader_file };
    std::string shader_strs[5];

    for(int i = 0; i < 5; ++i)
    {
        if (shader_files[i].empty() == true)
            continue;

        std::ifstream input_file(shader_files[i]);

        if(input_file.fail() == true)
        {
            LOGE("error: can't open file %s", shader_files[i].c_str());

            //std::exit(EXIT_FAILURE);
            throw std::runtime_error("error: can not open file");
        }

        std::stringstream sstream;
        sstream << input_file.rdbuf();

        input_file.close();

        shader_strs[i] = sstream.str();
    }

    this->createFromString(shader_strs[0],
                           shader_strs[1],
                           shader_strs[2],
                           shader_strs[3],
                           shader_strs[4]);

    //------

    m_is_created_from_file = true;

    for (int i = 0; i < 5; ++i)
    {
        if (shader_files[i].empty() == true)
            continue;

        m_file_paths.push_back(shader_files[i]);
    }
}


void GLShader::createFromString(const std::string & vertex_shader_str,
                                const std::string & fragment_shader_str,
                                const std::string & geometry_shader_str,
                                const std::string & tess_control_shader_str,
                                const std::string & tess_evaluation_shader_str)
{

    //destroy before create
    this->destroy();

    //create shader program
    this->m_program = glCreateProgram();

    if (this->m_program == 0)
    {
        LOGE("error: can not create shader program");
        throw std::runtime_error("error: can not create shader program");
    }

    GLuint shaders[5] = { 0, 0, 0, 0, 0 };

    GLuint types[5] = {
                        GL_VERTEX_SHADER,
                        GL_FRAGMENT_SHADER,
#if !__IOS__ && !__ANDROID__ //NOTE(Chen Wei): OpenGLES only support vertex & fragment shader
                        GL_GEOMETRY_SHADER,
                        GL_TESS_CONTROL_SHADER,
                        GL_TESS_CONTROL_SHADER
#endif
                      };

    std::string sources[5] = {
                                vertex_shader_str,
                                fragment_shader_str,
                                geometry_shader_str,
                                tess_control_shader_str,
                                tess_evaluation_shader_str
                             };

#if __IOS__ || __ANDROID__
    constexpr int max_shader_num = 2; //NOTE(Chen Wei): OpenGLES only support vertex & fragment shader
#else
    constexpr int max_shader_num = 5;
#endif

    for (int i = 0; i < max_shader_num; ++i)
    {
        if (!sources[i].empty())
        {
            //if source not start with #version, add it automatically
            if (!sources[i].starts_with("#version"))
            {
#if WIN32
                sources[i] =  "#version 410\n" + sources[i];
#elif __ANDROID__ || __IOS__
                if (sources[i].contains("varying") || sources[i].contains("attribute")) //use old syntax
                    sources[i] = "#version 100\n" + sources[i];
                else
                    sources[i] = "#version 300 es\n" + sources[i];
#elif __MACOS__
                if (sources[i].contains("varying") || sources[i].contains("attribute")) //use old syntax
                    sources[i] = "#version 100\n" + sources[i];
                else
                    sources[i] = "#version 410\n" + sources[i];
#else
                static_assert(false, "not supported platforms");
#endif
            }

            shaders[i] = glCreateShader(types[i]);
            const char* source_ptr = sources[i].c_str();
            glShaderSource(shaders[i], 1, &source_ptr, 0);
            glCompileShader(shaders[i]);

            //LOGI(sources[i]);

            //output error message if fails
            glslPrintShaderLog(shaders[i]);

            glAttachShader(m_program, shaders[i]);
        }
    }

    glLinkProgram(m_program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(this->m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::string error_log;
        error_log.resize(1024);
        glGetProgramInfoLog(this->m_program, 1024, 0, error_log.data());

        LOGE("Failed to link program:");
        LOGE("%s", error_log.c_str());

        glDeleteProgram(this->m_program);
        m_program = 0;

        //std::exit(EXIT_FAILURE);
        throw std::runtime_error("error: can not link shader program");
    }

    for (int i = 0; i < 5; ++i)
        glDeleteShader(shaders[i]);
}


void GLShader::destroy()
{
    if (this->m_program == 0)
        return;

    glVerify(glDeleteProgram(this->m_program));
    this->m_program = 0;

    this->m_cur_tex_offset = 0;
    m_tex_map.clear();

    m_is_created_from_file = false;
    m_auto_reload_from_file = false;

    m_file_paths.clear();
    m_file_watchers.clear();
    m_auto_reload_callback = nullptr;
}

void GLShader::use() const
{
    if (this->isValid())
    {
        glVerify(glUseProgram(this->m_program));
    }
    else
    {
        throw std::invalid_argument("error: invalid shader program!");
    }
}

void GLShader::unUse() const
{
    glVerify(glUseProgram(0));
}

bool GLShader::setBool(const std::string & name, bool val)
{
    return this->setInt(name, val);
}

bool GLShader::setInt(const std::string & name, int val)
{
    return openGLSetShaderInt(this->m_program, name, val);
}

bool GLShader::setFloat(const std::string & name, float val)
{
    return openGLSetShaderFloat(this->m_program, name, val);
}

bool GLShader::setBoolArray(const std::string& name, unsigned int num, const bool* val)
{
    return openGLSetShaderBoolArray(this->m_program, name, num, val);
}

bool GLShader::setIntArray(const std::string& name, unsigned int num, const int* val)
{
    return openGLSetShaderIntArray(this->m_program, name, num, val);
}

bool GLShader::setBoolArray(const std::string& name, const std::vector<bool>& val)
{
    std::vector<int> int_val(val.size());
    for (unsigned int i = 0; i < val.size(); ++i)
        int_val[i] = val[i] ? 1 : 0;

    return this->setIntArray(name, val.size(), int_val.data());
}

bool GLShader::setFloatArray(const std::string& name, unsigned int num, const float* val)
{
    return openGLSetShaderFloatArray(this->m_program, name, num, val);
}

bool GLShader::setIntArray(const std::string& name, const std::vector<int>& val)
{
    return openGLSetShaderIntArray(this->m_program, name, val.size(), val.data());
}

bool GLShader::setFloatArray(const std::string& name, const std::vector<float>& val)
{
    return openGLSetShaderFloatArray(this->m_program, name, val.size(), val.data());
}

bool GLShader::setVec2(const std::string & name, const glm::vec2 & val)
{
    return openGLSetShaderVec2(this->m_program, name, val);
}

bool GLShader::setVec2(const std::string & name, float x, float y)
{
    return openGLSetShaderVec2(this->m_program, name, x, y);
}

bool GLShader::setVec2(const std::string& name, const float * val_ptr)
{
    return openGLSetShaderVec2(this->m_program, name, val_ptr[0], val_ptr[1]);
}

bool GLShader::setVec3(const std::string & name, const glm::vec3 & val)
{
    return openGLSetShaderVec3(this->m_program, name, val);
}

bool GLShader::setVec3(const std::string & name, float x, float y, float z)
{
    return openGLSetShaderVec3(this->m_program, name, x, y, z);
}

bool GLShader::setVec3(const std::string& name, const float* val_ptr)
{
    return openGLSetShaderVec3(this->m_program, name, val_ptr[0], val_ptr[1], val_ptr[2]);
}

bool GLShader::setVec4(const std::string & name, const glm::vec4 & val)
{
    return openGLSetShaderVec4(this->m_program, name, val);
}

bool GLShader::setVec4(const std::string & name, float x, float y, float z, float w)
{
    return openGLSetShaderVec4(this->m_program, name, x, y, z, w);
}

bool GLShader::setVec4(const std::string& name, const float* val_ptr)
{
    return openGLSetShaderVec4(this->m_program, name, val_ptr[0], val_ptr[1], val_ptr[2], val_ptr[3]);
}

bool GLShader::setMat2(const std::string & name, const glm::mat2 & val)
{
    return openGLSetShaderMat2(this->m_program, name, val);
}

bool GLShader::setMat3(const std::string & name, const glm::mat3 & val)
{
    return openGLSetShaderMat3(this->m_program, name, val);
}

bool GLShader::setMat4(const std::string & name, const glm::mat4 & val)
{
    return openGLSetShaderMat4(this->m_program, name, val);
}

bool GLShader::setTexture(const std::string& name, int tex_id)
{
    int tex_unit = -1;

    if (m_tex_map.find(name) != m_tex_map.end())
    {
        tex_unit = m_tex_map[name];
    }
    else
    {
        tex_unit = GL_TEXTURE0 + m_cur_tex_offset;
        ++m_cur_tex_offset;

        m_tex_map[name] = tex_unit;
    }

    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    return openGLSetShaderInt(this->m_program, name, tex_unit - GL_TEXTURE0);
}

bool GLShader::setTexture(const std::string& name, const GLTexture& tex)
{
    int tex_unit = -1;

    if (m_tex_map.find(name) != m_tex_map.end())
    {
        tex_unit = m_tex_map[name];
    }
    else
    {
        tex_unit = GL_TEXTURE0 + m_cur_tex_offset;
        ++m_cur_tex_offset;

        m_tex_map[name] = tex_unit;
    }

    glActiveTexture(tex_unit);
    tex.bind();

    return openGLSetShaderInt(this->m_program, name, tex_unit - GL_TEXTURE0);
}

int GLShader::getAttribLocation(const std::string & name) const
{
    this->use();
    int loc = openGLGetCurShaderAttribLocation(name);
    this->unUse();

    return loc;
}

bool GLShader::setUBO(const std::string & name, int ubo_id, int binding_point)
{
    return openGLSetShaderUBO(this->m_program, name, ubo_id, binding_point);
}

bool GLShader::isValid() const
{
    return glIsProgram(this->m_program);
}

unsigned int GLShader::id() const
{
    return this->m_program;
}

void GLShader::enableAutoReloadFromFile(bool enable)
{
    m_auto_reload_from_file = enable;
    if (enable)
    {
        for (const auto& path : m_file_paths)
        {
            m_file_watchers.push_back(FileAsyncWatcher(path, [this](const std::filesystem::path& path, FileAsyncWatcher::EventType type)
            {
                //NOTE(Chen Wei): must run on main thread because we need openGL context
                getMainThreadTaskQueue().push( [this, path]()
                {
                    try
                    {
                        LOGI("INFO: auto reload shader from file: %s", path.string().c_str());

                        //ensure exception-safe

                        GLShader shader;
                        shader.createFromFile(m_file_paths[0],
                                              m_file_paths[1],
                                              m_file_paths.size() >= 3 ? m_file_paths[2] : "",
                                              m_file_paths.size() >= 4 ? m_file_paths[3] : "",
                                              m_file_paths.size() >= 5 ? m_file_paths[4] : "");

                        shader.m_auto_reload_from_file = this->m_auto_reload_from_file;
                        shader.m_file_watchers = std::move(this->m_file_watchers);
                        shader.m_auto_reload_callback = std::move(this->m_auto_reload_callback);

                        *this = std::move(shader);

                        if (m_auto_reload_callback)
                            m_auto_reload_callback();
                    }
                    catch (const std::exception& e)
                    {
                        //do nothing
                    }
                });
            }));
        }
    }
    else
    {
        m_file_watchers.clear();
    }
}

void GLShader::setAutoReloadCallback(const std::function<void()>& callback)
{
    m_auto_reload_callback = callback;
}

}// end of namespace luna
