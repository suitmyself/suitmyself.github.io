/**
 * @author     : Chen Wei
 * @date       : 2023-05-16
 * @description: opengl debug message callback (copy from: https://gist.github.com/Plasmoxy/aec637b85e306f671339dcfd509efc82)
 * @version    : 1.0
 */


// =============== How to use ================
// The following function calls should be made directly after OpenGL
// initialization.

// Enable the debugging layer of OpenGL
//
// GL_DEBUG_OUTPUT - Faster version but not useful for breakpoints
// GL_DEBUG_OUTPUT_SYNCHRONUS - Callback is in sync with errors, so a breakpoint
// can be placed on the callback in order to get a stacktrace for the GL error. (enable together with GL_DEBUG_OUTPUT !)

// glEnable(GL_DEBUG_OUTPUT);
// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

// Set the function that will be triggered by the callback, the second parameter
// is the data parameter of the callback, it can be useful for different
// contexts but isn't necessary for our simple use case.
// glDebugMessageCallback(GLDebugMessageCallback, nullptr);

// REQUIREMENTS: OpenGL version with the KHR_debug extension available.
// modified for C++ by Plasmoxy [7. 5. 2020], I'm using: GLEW and GLFW3, OpenGL 4.6
// original gist: https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f

#include "gl_debug_message_callback.h"

#if !__IOS__ && !__ANDROID__
//NOTE(Chen Wei): not supported by iOS && Android

// Callback function for printing debug statements
void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar* msg, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    if (!(_type == "OTHER" && _severity == "NOTIFICATION") && !(_type == "PERFORMANCE" && _severity == "MEDIUM"))
        printf("OpenGL error [%d]: %s of %s severity, raised from %s: %s\n", id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);

    // ignore notification severity (you can add your own ignores)
    // + Adds __debugbreak if _DEBUG is defined (automatic in visual studio)
    // note: __debugbreak is specific for MSVC, won't work with gcc/clang
    // -> in that case remove it and manually set breakpoints
    if (_severity != "NOTIFICATION" && _type != "PERFORMANCE") {

#ifdef _DEBUG
        __debugbreak();
#endif
    }
}

#endif