#include <opencv2/opencv.hpp>

int main() {
    // 读取图像
    cv::Mat inputImage = cv::imread("./asset/image/yy.png");

    // 检查图像是否成功加载
    if (inputImage.empty()) {
        std::cerr << "Error: Could not open or find the image.\n";
        return -1;
    }

    // 设置新的宽度和高度
    int newWidth = 300;
    int newHeight = 200;

    // 调整图像大小
    cv::Mat resizedImage;
    cv::resize(inputImage, resizedImage, cv::Size(newWidth, newHeight));

    // 显示原始图像和调整大小后的图像
    cv::imshow("Original Image", inputImage);
    cv::imshow("Resized Image", resizedImage);

    // 等待按键
    cv::waitKey(0);

    return 0;
}
