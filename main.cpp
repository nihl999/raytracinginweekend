#include "camera.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "rtweekend.h"
#include "sphere.h"
#include "vec3.h"

#include <cmath>
#include <iostream>
#include <memory>

double hit_sphere(const point3 &center, double radius, const ray &r) {
  vec3 oc = r.origin() - center;
  auto a = r.direction().length_squared();
  auto half_b = dot(oc, r.direction());
  auto c = oc.length_squared() - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-half_b - sqrt(discriminant)) / a;
  }
}

color ray_color(const ray &r, const hittable &world) {
  hit_record rec;
  if (world.hit(r, interval(0, infinity), rec)) {
    return 0.5 * (rec.normal + color(1, 1, 1));
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
  hittable_list world;

  auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
  auto material_left = std::make_shared<dieletric>(1.5);
  auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

  world.add(std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0,
                                     material_ground));
  world.add(
      std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
  world.add(
      std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(
      std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
  world.add(
      std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vfov = 90;
  cam.lookfrom = point3(-2, 2, 1);
  cam.lookat = point3(0, 0, -1);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 10.0;
  cam.focus_dist = 1;

  cam.render(world);
}
