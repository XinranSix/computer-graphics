#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb_image_write.h"
#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#endif
#include "stb_image_resize.h"

#include "image/image.h"

namespace ht {

    Image::Image(std::string_view path) {
        loadOrReload(path);
    }

    Image::Image(Image const &src) {
        width = src.width;
        height = src.height;
        channels = src.channels;
        auto size { width * height * channels };
        // data = static_cast<unsigned char *>(malloc(size));
        data = new unsigned char[size];
        std::copy(src.data, src.data + size, data);
    }

    Image &Image::operator=(Image const &rhs) {
        Image temp { rhs };
        swap(temp);
        return *this;
    }

    Image::Image(Image &&src) noexcept {
        ::swap(*this, src);
    }

    Image &Image::operator=(Image &&rhs) noexcept {
        ::swap(*this, rhs);
        return *this;
    }

    Image::~Image() {
        if (data != nullptr) {
            stbi_image_free(data);
            data = nullptr;
        }
        width = 0;
        height = 0;
        channels = 0;
    }

    void Image::swap(Image &other) noexcept {
        std::swap(width, other.width);
        std::swap(height, other.height);
        std::swap(channels, other.channels);
        std::swap(data, other.data);
    }

    void Image::loadOrReload(std::string_view path) {
        if (data) {
            Image temp { path };
            ::swap(*this, temp);
        } else {
            data = stbi_load(path.data(), &width, &height, &channels, 0);
        }
    }

    bool Image::resize(int newWidth, int newHeight) {
        auto newData = new unsigned char[newWidth * newHeight * channels];
        if (stbir_resize(data, width, height, 0, newData, newWidth, newHeight,
                         0, STBIR_TYPE_UINT8, channels,
                         STBIR_ALPHA_CHANNEL_NONE, 0, STBIR_EDGE_CLAMP,
                         STBIR_EDGE_CLAMP, STBIR_FILTER_BOX, STBIR_FILTER_BOX,
                         STBIR_COLORSPACE_SRGB, nullptr)) {
            Image temp { newWidth, newHeight, channels, newData };
            ::swap(*this, temp);
            return true;
        }

        delete[] newData;
        return false;
    }

    bool Image::write(std::string_view path, ImageFormat imageFormat) {
        bool success {};
        if (imageFormat == ImageFormat::PNG) {
            success = stbi_write_png(path.data(), width, height, channels, data,
                                     width * channels);
        } else if (imageFormat == ImageFormat::BMP) {
            success =
                stbi_write_bmp(path.data(), width, height, channels, data);
        } else if (imageFormat == ImageFormat::JPG) {
            success =
                stbi_write_jpg(path.data(), width, height, channels, data, 100);
        }
        return success;
    }

    Image::Image(int width, int height, int channels, unsigned char *data)
        : width { width }, height { height }, channels { channels },
          data { data } {
    }

} // namespace ht

void swap(ht::Image &first, ht::Image &second) noexcept {
    first.swap(second);
}
