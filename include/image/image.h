#pragma once

#include <string_view>
#include <string>

namespace ht {
    class Image {
    public:
        enum class ImageFormat { PNG, BMP, JPG, Unknown };

    public:
        Image() = default;
        Image(std::string_view path);
        Image(Image const &src);
        Image &operator=(Image const &rhs);
        Image(Image &&src) noexcept;
        Image &operator=(Image &&rhs) noexcept;
        ~Image();
        void swap(Image &other) noexcept;

    public:
        void loadOrReload(std::string_view path);
        bool resize(int newWidth, int newHeight);
        bool write(std::string_view path,
                   ImageFormat imageFormat = ImageFormat::PNG);

    public:
        int getWidth() const {
            return width;
        }

        int getHeight() const {
            return height;
        }

        int getChannels() const {
            return channels;
        }

        unsigned char *getData() const {
            return data;
        }

    private:
        Image(int width, int height, int channels, unsigned char *data);

    private:
        int width {};
        int height {};
        int channels {};
        unsigned char *data {};
    };
} // namespace ht

void swap(ht::Image &first, ht::Image &second) noexcept;
