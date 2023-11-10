#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"
#include "rtweekend.h"
#include "vec3.h"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material {
public:
  lambertian(const color &_albedo) : albedo(_albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const override {
    auto scatter_direction = rec.normal * random_unit_vector();
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;
    scattered = ray(rec.hit_point, scatter_direction);
    attenuation = albedo;
    return true;
  }

private:
  color albedo;
};

class metal : public material {
public:
  metal(const color &_albedo, const float _fuzz)
      : albedo(_albedo), fuzz(_fuzz) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const override {

    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.hit_point, reflected + fuzz * random_unit_vector());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

private:
  color albedo;
  float fuzz;
};

static const double vacuum_ref = 1.0;

class dieletric : public material {
public:
  dieletric(double _ref_idx)
      : ref_idx(_ref_idx), albedo(color(1.0, 1.0, 1.0)) {}

  dieletric(double _ref_idx, const color &_albedo)
      : ref_idx(_ref_idx), albedo(_albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const override {
    attenuation = albedo;
    double ref_ratio = rec.front_face ? (vacuum_ref / ref_idx) : ref_idx;

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ref_ratio * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract)
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, ref_ratio);

    scattered = ray(rec.hit_point, direction);
    return true;
  }

private:
  double ref_idx;
  color albedo;
};

#endif
