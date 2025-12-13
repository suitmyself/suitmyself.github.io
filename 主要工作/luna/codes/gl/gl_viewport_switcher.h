/**
 * @author     : Chen Wei
 * @date       : 2023-07-04
 * @description: opengl viewport switcher
 * @version    : 1.0
 */

#pragma once

#include <vector>

namespace luna {

struct GLViewport
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

class GLViewportSwitcher
{
public:
    GLViewportSwitcher() = default;

    GLViewportSwitcher(int width, int height, int num, bool is_horizontal = true);

    GLViewportSwitcher(int x, int y, int width, int height, int num, bool is_horizontal = true);

    bool init(int width, int height, int num, bool is_horizontal = true);

    bool init(int x, int y, int width, int height, int num, bool is_horizontal = true);

    bool addViewport(int x, int y, int width, int height);

    const GLViewport& getViewport(size_t index) const;

    size_t getViewportSize() const;

    bool switchViewport(size_t index) const;

private:

    std::vector<GLViewport> m_viewports;
};

//NOTE(Chen Wei): for mobile platforms(IOS & Android), screen ratio may not match image ratio,
//                in this case, we need to adjust viewport to match image content.

enum class ViewportMatchMode
{
    FULLSCREEN,     //!< show image in fullscreen, i.e. some image pixels are cropped
    NON_FULLSCREEN, //!< show image in non-fullscreen, i.e. all image pixels are shown
};

GLViewport computeViewportByMatchImage(int ori_viewport_width,
                                       int ori_viewport_height,
                                       int image_width,
                                       int image_height,
                                       ViewportMatchMode match_mode);

}//end of namespace luna
