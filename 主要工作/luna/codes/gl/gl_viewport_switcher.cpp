/**
 * @author     : Chen Wei
 * @date       : 2023-07-04
 * @description: opengl viewport switcher
 * @version    : 1.0
 */

#include <iostream>
#include <stdexcept>

#include "core/log/log.h"

#include "gl_include.h"

#include "gl_viewport_switcher.h"


namespace luna
{
GLViewportSwitcher::GLViewportSwitcher(int width, int height, int num, bool is_horizontal)
    :GLViewportSwitcher(0, 0, width, height, num, is_horizontal)
{
}

GLViewportSwitcher::GLViewportSwitcher(int x, int y, int width, int height, int num, bool is_horizontal)
{
    bool succ = init(x, y, width, height, num, is_horizontal);
    if (!succ)
    {
        LOGE("error: can not init GLViewportSwitcher");
        throw std::invalid_argument("error: can not init GLViewportSwitcher");
    }
}

bool GLViewportSwitcher::init(int width, int height, int num, bool is_horizontal)
{
    return init(0, 0, width, height, num, is_horizontal);
}

bool GLViewportSwitcher::init(int x, int y, int width, int height, int num, bool is_horizontal)
{
    m_viewports.clear();

    if (is_horizontal)
    {
        for (int i = 0; i < num; ++i)
            addViewport(x + i * width, y, width, height);
    }
    else
    {
        for (int i = 0; i < num; ++i)
            addViewport(x, y + i * height, width, height);
    }

    return true;
}

bool GLViewportSwitcher::addViewport(int x, int y, int width, int height)
{
    m_viewports.push_back({ x, y, width, height });
    return true;
}

const GLViewport& GLViewportSwitcher::getViewport(size_t index) const
{
    if (index >= m_viewports.size())
        throw std::out_of_range("index out of range");

    return m_viewports[index];
}

size_t GLViewportSwitcher::getViewportSize() const
{
    return m_viewports.size();
}

bool GLViewportSwitcher::switchViewport(size_t index) const
{
    if (index >= m_viewports.size())
        return false;

    const auto& vp = m_viewports[index];
    glViewport(vp.x, vp.y, vp.width, vp.height);

    return true;
}

GLViewport computeViewportByMatchImage(int ori_viewport_width,
                                       int ori_viewport_height,
                                       int image_width,
                                       int image_height,
                                       ViewportMatchMode match_mode)
{
    if (image_width == 0 || image_height == 0)
    {
        GLViewport viewport;

        viewport.x = 0;
        viewport.y = 0;
        viewport.width = ori_viewport_width;
        viewport.height = ori_viewport_height;

        return viewport;
    }

    if (match_mode == ViewportMatchMode::NON_FULLSCREEN)
    {
        float viewport_ratio = float(ori_viewport_height) / float(ori_viewport_width);
        float image_ratio = float(image_height) / float(image_width);

        if (viewport_ratio > image_ratio)
        {
            GLViewport viewport;

            viewport.width = ori_viewport_width;
            viewport.height = ori_viewport_width * image_ratio;

            viewport.x = 0;
            viewport.y = (ori_viewport_height - viewport.height) / 2;

            return viewport;
        }
        else
        {
            GLViewport viewport;

            viewport.width = ori_viewport_height / image_ratio;
            viewport.height = ori_viewport_height;

            viewport.x = (ori_viewport_width - viewport.width) / 2;
            viewport.y = 0;

            return viewport;
        }
    }
    else if (match_mode == ViewportMatchMode::FULLSCREEN)
    {
        float viewport_ratio = float(ori_viewport_height) / float(ori_viewport_width);
        float image_ratio = float(image_height) / float(image_width);

        if (viewport_ratio > image_ratio)
        {
            GLViewport viewport;

            viewport.width = ori_viewport_height / image_ratio;
            viewport.height = ori_viewport_height;

            viewport.x = (ori_viewport_width - viewport.width) / 2;
            viewport.y = 0;

            return viewport;
        }
        else
        {
            GLViewport viewport;

            viewport.width = ori_viewport_width;
            viewport.height = ori_viewport_width * image_ratio;

            viewport.x = 0;
            viewport.y = (ori_viewport_height - viewport.height) / 2;

            return viewport;
        }
    }
    else
    {
        throw std::runtime_error("unknown match mode");
    }
}

}//end of namespace luna
