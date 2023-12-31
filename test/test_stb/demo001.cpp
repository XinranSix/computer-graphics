#include "image/image.h"
#include <iostream>

int main() {

    ht::Image image1 { "./asset/image/yy.png" };
    image1.write("./test001.jpg", ht::Image::ImageFormat::PNG);
    if (!image1.resize(image1.getWidth() / 2, image1.getHeight() / 2)) {
        std::cerr << "ERROR\n";
    }
    image1.write("./test002.jpg", ht::Image::ImageFormat::PNG);

    return 0;
}
