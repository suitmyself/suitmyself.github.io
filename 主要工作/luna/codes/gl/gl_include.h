/**
 * @author     : Chen Wei
 * @date       : 2023-05-10
 * @description: gl_include.h
 * @version    : 1.0
 */

#pragma once

#include "core/macros/platform.h"

#if defined(__ANDROID__)
    #include <GLES3/gl31.h>
#elif defined _WIN32 || __gnu_linux__
    #include <GL/glew.h>
#elif defined(__APPLE__)
    #if __IOS__
            // iOS, tvOS, or watchOS device
            #include <OpenGLES/ES3/gl.h>
            #include <OpenGLES/ES3/glext.h>
    #elif __MACOS__
            // Other kinds of Apple platforms
            #include <GL/glew.h>
    #else
        #error "Unknown Apple platform"
    #endif
#endif
