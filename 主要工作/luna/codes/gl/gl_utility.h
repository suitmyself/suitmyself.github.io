/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: gl_utility.h
 * @version    : 1.0
 */

#pragma once

#include <iostream>

#include "gl_include.h"

#include "glm/fwd.hpp"
#include "glm/ext.hpp"

#include "core/log/log.h"

namespace luna {

/*
 * glAssert, assert function extracted from Flex
 */
inline void glAssert(const char* msg, long line, const char* file)
{
    struct glError
    {
        GLenum code;
        const char* name;
    };

    static const glError errors[] = {
                                      { GL_NO_ERROR, "No Error" },
                                      { GL_INVALID_ENUM, "Invalid Enum" },
                                      { GL_INVALID_VALUE, "Invalid Value" },
                                      { GL_INVALID_OPERATION, "Invalid Operation" }
                                    };

    GLenum e = glGetError();

    if (e == GL_NO_ERROR)
    {
        return;
    }
    else
    {
        const char* errorName = "Unknown error";

        // find error message
        for (uint32_t i = 0; i < sizeof(errors) / sizeof(glError); i++)
        {
            if (errors[i].code == e)
            {
                errorName = errors[i].name;
            }
        }

        LOGE("OpenGL: %s - error %s in %s at line %d\n", msg, errorName, file, int(line));
        assert(0);
    }
}

#if defined(NDEBUG)
    #define glVerify(x) x
#else
    #define glVerify(x) do{x; glAssert(#x, __LINE__, __FILE__);} while(false)
    //#define glVerify(x)                                        \
    //    [&]() {                                                \
    //        if constexpr (std::is_same_v<decltype(x), void>)   \
    //        {                                                  \
    //            x;                                             \
    //            glAssert(#x, __LINE__, __FILE__);              \
    //        }                                                  \
    //        else                                               \
    //        {                                                  \
    //            decltype(auto) ret = x;                        \
    //            glAssert(#x, __LINE__, __FILE__);              \
    //            return ret;                                    \
    //        }                                                  \
    //    }()
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int openGLGetCurBindShaderID()
{
    GLint cur_program_id = 0;
    glVerify(glGetIntegerv(GL_CURRENT_PROGRAM, &cur_program_id));
    return cur_program_id;
}

inline int openGLGetCurShaderAttribLocation(const std::string& name)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return glGetAttribLocation(cur_shader_id, name.c_str());
}

inline void openGLCheckShaderBind(int shader_id)
{
    GLint cur_program_id = 0;
    glVerify(glGetIntegerv(GL_CURRENT_PROGRAM, &cur_program_id));

    if (cur_program_id != shader_id)
        throw std::invalid_argument("error: this shader_program is not binded, please call use() before setting");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_OUTPUT false

inline bool openGLSetShaderInt(int shader_id, const std::string& name, int val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform1i(location, val));
    return true;
}

inline bool openGLSetShaderIntArray(int shader_id, const std::string& name, unsigned int num, const int* val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);
    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }
    glVerify(glUniform1iv(location, num, val));
    return true;
}

inline bool openGLSetShaderBool(int shader_id, const std::string& name, bool val)
{
    return openGLSetShaderInt(shader_id, name, val);
}

inline bool openGLSetShaderBoolArray(int shader_id, const std::string& name, unsigned int num, const bool* val)
{
    std::vector<int> int_val(num);
    for (unsigned int i = 0; i < num; ++i)
        int_val[i] = val[i] ? 1 : 0;

    return openGLSetShaderIntArray(shader_id, name, num, int_val.data());
}

inline bool openGLSetShaderFloat(int shader_id, const std::string& name, float val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform1f(location, val));
    return true;
}

inline bool openGLSetShaderFloatArray(int shader_id, const std::string& name, unsigned int num, const float* val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);
    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }
    glVerify(glUniform1fv(location, num, val));
    return true;
}

inline bool openGLSetShaderFloat2V(int shader_id, const std::string& name, unsigned int num, const float* val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform2fv(location, num, val));
    return true;
}

inline bool openGLSetShaderVec2(int shader_id, const std::string& name, const glm::vec2& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform2fv(location, 1, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderVec2(int shader_id, const std::string& name, float x, float y)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform2f(location, x, y));
    return true;
}

inline bool openGLSetShaderVec3(int shader_id, const std::string& name, const glm::vec3& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform3fv(location, 1, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderVec3(int shader_id, const std::string& name, float x, float y, float z)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform3f(location, x, y, z));
    return true;
}

inline bool openGLSetShaderVec4(int shader_id, const std::string& name, const glm::vec4& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform4fv(location, 1, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderVec4(int shader_id, const std::string& name, float x, float y, float z, float w)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform4f(location, x, y, z, w));
    return true;
}

inline bool openGLSetShaderFloat4V(int shader_id, const std::string& name, unsigned int num, const float* val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniform4fv(location, num, val));
    return true;
}

inline bool openGLSetShaderMat2(int shader_id, const std::string& name, const glm::mat2& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderMat3(int shader_id, const std::string& name, const glm::mat3& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderMat4(int shader_id, const std::string& name, const glm::mat4& val)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    int location = glGetUniformLocation(shader_id, name.c_str());
    if (location == -1)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val)));
    return true;
}

inline bool openGLSetShaderAttribLocation(int shader_id, const std::string& name, int loc)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    glVerify(glBindAttribLocation(shader_id, loc, name.c_str()));

    return true;
}

inline bool openGLSetShaderUBO(int shader_id, const std::string& name, int ubo_id, int binding_point)
{
    if (shader_id == 0)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: cur bind shader is 0! ");
#endif
        return false;
    }
    openGLCheckShaderBind(shader_id);

    GLint block_index = glGetUniformBlockIndex(shader_id, name.c_str());
    if (block_index == GL_INVALID_INDEX)
    {
#if DEBUG_OUTPUT
        LOGE("wanning: no uniform block attribute %s found!", name.c_str());
#endif
        return false;
    }

    glVerify(glUniformBlockBinding(shader_id, block_index, binding_point));

    glVerify(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo_id));

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool openGLSetCurBindShaderBool(const std::string& name, bool val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderBool(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderBoolArray(const std::string& name, unsigned int num, const bool* val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderBoolArray(cur_shader_id, name, num, val);
}

inline bool openGLSetCurBindShaderInt(const std::string& name, int val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderInt(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderIntArray(const std::string& name, unsigned int num, const int* val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderIntArray(cur_shader_id, name, num, val);
}

inline bool openGLSetCurBindShaderFloat(const std::string& name, float val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderFloat(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderFloatArray(const std::string& name, unsigned int num, const float* val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderFloatArray(cur_shader_id, name, num, val);
}

inline bool openGLSetCurBindShaderFloat2V(const std::string& name, unsigned int num, const float* val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderFloat2V(cur_shader_id, name, num, val);
}

inline bool openGLSetCurBindShaderVec2(const std::string& name, const glm::vec2& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec2(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderVec2(const std::string& name, float x, float y)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec2(cur_shader_id, name, x, y);
}

inline bool openGLSetCurBindShaderVec3(const std::string& name, const glm::vec3& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec3(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderVec3(const std::string& name, float x, float y, float z)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec3(cur_shader_id, name, x, y, z);
}

inline bool openGLSetCurBindShaderVec4(const std::string& name, const glm::vec4& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec4(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderVec4(const std::string& name, float x, float y, float z, float w)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderVec4(cur_shader_id, name, x, y, z, w);
}

inline bool openGLSetCurBindShaderFloat4V(const std::string& name, unsigned int num, const float* val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderFloat4V(cur_shader_id, name, num, val);
}

inline bool openGLSetCurBindShaderMat2(const std::string& name, const glm::mat2& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderMat2(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderMat3(const std::string& name, const glm::mat3& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderMat3(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderMat4(const std::string& name, const glm::mat4& val)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderMat4(cur_shader_id, name, val);
}

inline bool openGLSetCurBindShaderAttribLocation(const std::string& name, int loc)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderAttribLocation(cur_shader_id, name, loc);
}

inline bool openGLSetCurBindShaderUBO(const std::string& name, int ubo_id, int binding_point)
{
    int cur_shader_id = openGLGetCurBindShaderID();
    return openGLSetShaderUBO(cur_shader_id, name, ubo_id, binding_point);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline GLenum openGLCheckCurFramebufferStatus(GLenum target = GL_FRAMEBUFFER)
{
    GLenum status = glCheckFramebufferStatus(target);
    switch (status)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;

    case GL_FRAMEBUFFER_UNDEFINED:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_UNDEFINED");
        break;
    }

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
        break;
    }

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
        break;
    }

#if !__ANDROID__ && !__IOS__
    //NOTE(Chen Wei): openGLES do not support GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
        break;
    }

    //NOTE(Chen Wei): openGLES do not support GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
        break;
    }
#endif

    case GL_FRAMEBUFFER_UNSUPPORTED:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_UNSUPPORTED");
        break;
    }

    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    {
        LOGE("framebuffer error: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
        break;
    }

    default:
    {
        LOGE("framebuffer error: unknown");
        break;
    }

    }

    return status;
}

} //end of namespace luna
