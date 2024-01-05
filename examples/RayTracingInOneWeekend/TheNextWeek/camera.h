#pragma once

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include <sstream>
#include <fstream>
#include "material.h"

class camera {
public:
    double aspect_ratio { 1.0 };  // Ratio of image width over height
    int image_width { 100 };      // Rendered image width in piexel count
    int samples_per_pixel { 10 }; // Count of random samples for each pixel
    int max_depth { 10 };         // Maximum number of ray bounces into scene
    color background;             // Scene background color

    double vfov = 90;             // Vertical view angle (field of view)
    point3 lookfrom { 0, 0, -1 }; // Point camera is looking from
    point3 lookat { 0, 0, 0 };    // Point camera is looking at
    vec3 vup { 0, 1, 0 };         // Camera-relative "up" direction

    double defocus_angle { 0 }; // Variation angle of rays through each pixel
    double focus_dist {
        10
    }; // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable &world) {
        initialize();

        std::ofstream ofs { "./image.ppm" };
        std::ostringstream oss {};

        oss << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j {}; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' '
                      << std::flush;
            for (int i {}; i < image_width; ++i) {
                color pixel_color { 0, 0, 0 };
                for (int sample {}; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(oss, pixel_color, samples_per_pixel);
            }
        }

        ofs << oss.str();
        std::clog << "\rDone.                 \n";
    }

private:
    int image_height;    // Rendered image the height
    point3 center;       // Camera center
    point3 pixel00_loc;  // Location of pixel 0, 0
    vec3 pixel_delta_u;  // Offset to pixel to the right
    vec3 pixel_delta_v;  // Offset to pixel below
    vec3 u, v, w;        // Camera frame basis vectors
    vec3 defocus_disk_u; // Defocus disk horizontal radius
    vec3 defocus_disk_v; // Defocus dist vertical radius

    void initialize() {
        // 计算出图像的高度，且确保高度至少为 1
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions.
        // auto focal_length { (lookfrom - lookat).length() }; // 焦距
        auto theta { degrees_to_radians(vfov) };
        auto h { std::tan(theta / 2) };
        auto viewport_height { 2 * h * focus_dist }; // 视口高度
        // 通过 viewport_height 计算出 viewport_width，不使用 aspect_ration
        // 来计算是因为 image_width 和 image_height
        // 都是整数，有舍入。
        auto viewport_width {
            viewport_height * (static_cast<double>(image_width) / image_height)
        };

        // Calculate the u, v, w unit basis vectors for the camera coordinate
        // frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical
        // viewport edges.
        // 计算出表示图像从左到右、从上到下的矢量
        vec3 viewport_u { viewport_width * u };
        vec3 viewport_v { -viewport_height * v };

        // Calculate the horizontal and vertical delta vectors from pixel to
        // pixel.
        // 计算出像素之间的坐标增量
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        // 计算出图像左上角的坐标
        auto viewport_upper_left { center - (focus_dist * w) - viewport_u / 2 -
                                   viewport_v / 2 };
        // 由于我们会在图像边缘加上一圈，水平加的是 pixel_delta_u /
        // 2，垂直加的是
        // pixel_delta_v / 2，所以最左上角的像素坐标为 viewport_upper_left +
        // pixel_delta_u / 2 + pixel_delta_v / 2
        pixel00_loc =
            viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius { focus_dist *
                              std::tan(degrees_to_radians(defocus_angle / 2)) };
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i, j,
        // originating from the camera defocus disk.

        auto pixel_center { pixel00_loc + (i * pixel_delta_u) +
                            (j * pixel_delta_v) };
        auto pixel_sample { pixel_center + pixel_sample_square() };

        auto ray_origin { (defocus_angle <= 0) ? center
                                               : defocus_disk_sample() };
        auto ray_direction { pixel_sample - ray_origin };
        auto ray_time { random_double() };

        return ray { ray_origin, ray_direction, ray_time };
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the
        // origin.
        auto px { -0.5 + random_double() };
        auto py { -0.5 + random_double() };
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p { random_in_unit_disk() };
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray &r, int depth, const hittable &world) {
        hit_record rec {};

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0) {
            return color { 0, 0, 0 };
        }

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, { 0.001, infinity }, rec)) {
            return background;
        }

        ray scattered;
        color attenuation;
        color color_from_emission { rec.mat->emitted(rec.u, rec.v, rec.p) };

        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
            return color_from_emission;
        }

        auto color_from_scatter { attenuation *
                                  ray_color(scattered, depth - 1, world) };

        return color_from_emission + color_from_scatter;
    }
};
