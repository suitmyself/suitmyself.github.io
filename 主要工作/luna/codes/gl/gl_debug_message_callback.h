/**
 * @author     : Chen Wei
 * @date       : 2023-05-16
 * @description: opengl debug message callback (copy from: https://gist.github.com/Plasmoxy/aec637b85e306f671339dcfd509efc82)
 * @version    : 1.0
 */

#pragma once

#if !__IOS__ 
//NOTE(Chen Wei): not supported by iOS

#include <iostream>

#include "gl_include.h"

void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar* msg, const void* data);

#endif