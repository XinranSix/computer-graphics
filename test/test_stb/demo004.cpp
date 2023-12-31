#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 指定图像文件路径
    const char *imagePath = "./asset/image/yy.png";

    // 使用 stb_image 加载图像
    int imageWidth, imageHeight, imageChannels;
    unsigned char *imageData =
        stbi_load(imagePath, &imageWidth, &imageHeight, &imageChannels, 0);

    if (imageData) {
        std::cout << "Image loaded successfully.\n";
        std::cout << "Width: " << imageWidth << ", Height: " << imageHeight
                  << ", Channels: " << imageChannels << std::endl;

        // 将 stb_image 加载的图像数据转换为 OpenCV 的 cv::Mat 对象
        cv::Mat image(imageHeight, imageWidth, CV_8UC3, imageData);

        // 使用 cv::imshow 显示图像
        cv::imshow("Loaded Image", image);
        cv::waitKey(0); // 等待按键

        // 释放 stb_image 加载的图像数据
        stbi_image_free(imageData);
    } else {
        std::cerr << "Error loading image: " << stbi_failure_reason()
                  << std::endl;
    }

    return 0;
}
