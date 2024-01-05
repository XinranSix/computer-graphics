#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "color.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"

#include <memory>

int main() {

    hittable_list world {};

    auto red { std::make_shared<lambertian>(color { 0.65, 0.05, 0.05 }) };
    auto white { std::make_shared<lambertian>(color { 0.73, 0.73, 0.73 }) };
    auto green { std::make_shared<lambertian>(color { 0.12, 0.45, 0.15 }) };
    auto light { std::make_shared<lambertian>(color { 15, 15, 15 }) };

    // Cornell box sides
    world.add(std::make_shared<quad>(point3(555, 0, 0), vec3(0, 0, 555),
                                     vec3(0, 555, 0), green));
    world.add(std::make_shared<quad>(point3(0, 0, 555), vec3(0, 0, -555),
                                     vec3(0, 555, 0), red));
    world.add(std::make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0),
                                     vec3(0, 0, 555), white));
    world.add(std::make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0),
                                     vec3(0, 0, -555), white));
    world.add(std::make_shared<quad>(point3(555, 0, 555), vec3(-555, 0, 0),
                                     vec3(0, 555, 0), white));

    // Light
    world.add(std::make_shared<quad>(point3 { 213, 554, 227 },
                                     vec3 { 130, 0, 0 }, vec3 { 0, 0, 105 },
                                     light));

    // Box 1
    std::shared_ptr<hittable> box1 { box({ 0, 0, 0 }, { 165, 330, 165 },
                                         white) };
    box1 = std::make_shared<rotate_y>(box1, 15);
    box1 = std::make_shared<translate>(box1, vec3 { 265, 0, 295 });
    world.add(box1);

    // Box 2
    std::shared_ptr<hittable> box2 { box({ 0, 0, 0 }, { 165, 165, 165 },
                                         white) };
    box2 = std::make_shared<rotate_y>(box2, -18);
    box2 = std::make_shared<translate>(box2, vec3 { 130, 0, 65 });
    world.add(box2);

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 64;
    cam.max_depth = 50;
    cam.background = { 0, 0, 0 };

    cam.vfov = 40;
    cam.lookfrom = { 278, 278, -800 };
    cam.lookat = { 278, 278, 0 };
    cam.vup = { 0, 1, 0 };

    cam.defocus_angle = 0;

    cam.render(world);

    return 0;
}