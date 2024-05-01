#pragma once

#include <iostream>
#include <glad/glad.h>

namespace ht {
    class FrameBuffer {
    public:
        /**
         * @brief Construct a new Frame Buffer object
         *
         * @param width 当前帧缓存纹理对象的宽度
         * @param height 当前帧缓存纹理对象的宽度
         */
        FrameBuffer(float width, float height);

        ~FrameBuffer();

        /**
         * @brief Get the Frame Texture object
         *
         * @return unsigned int
         */
        GLuint getFrameTexture();

        /**
         * @brief 拉伸 ImGui 的窗口后图像的 width 和 height 都会有变化，需要重新分配
         *
         * @param width 新窗口的高宽度
         * @param height 新窗口的高度
         */
        void RescaleFrameBuffer(float width, float height);

        void Bind() const;

        void UnBind() const;

    private:
        unsigned int fbo;
        unsigned int texture;
        unsigned int rbo;
    };
} // namespace ht
