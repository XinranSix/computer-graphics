#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {

    std::ostringstream oss {};
    std::ofstream ofs { "./image.ppm" };

    // Image

    constexpr int image_width { 255 };
    constexpr int image_height { 255 };

    // Render

    oss << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j { 0 }; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' '
                  << std::flush;
        for (int i { 0 }; i < image_width; ++i) {
            auto r = static_cast<double>(i) / (image_width - 1);
            auto g = static_cast<double>(j) / (image_height - 1);
            auto b = 0;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            oss << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    ofs << oss.str();
    std::clog << "\rDone.                 \n";
    return 0;
}
