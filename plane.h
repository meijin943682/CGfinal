#ifndef PLANEH
#define PLANEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

class plane: public hitable {
public:
  plane() {}
  plane(vec3 p, vec3 n, vec3 col1, vec3 col2 = vec3(1, 1, 1), float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) :
    hitable(w_ri, w_ti, m), point(p), normal(n), color1(col1), color2(col2) {};
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  
  vec3 point;
  vec3 normal;
  vec3 color1;
  vec3 color2;
};

bool plane::hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  if(dot(r.direction(), this -> normal) != 0){
    float t = dot(this -> point - r.origin(), this -> normal) / dot(r.direction(), this -> normal);
    if(t > tmin && t < tmax){
      vec3 hit_point = r.origin() + t * r.direction();
      hit_record rec(t, hit_point, unit_vector(this -> normal), vec3(0, 0, 0), this -> w_r, this -> w_t, this -> material);
      rec.color = ((int)(floor(rec.p.x()) + floor(rec.p.z())) % 2 == 0)? this -> color1 : this -> color2;
      rec_list.push_back(rec);
      return true;
    }
  }
  return false;
}


#endif
