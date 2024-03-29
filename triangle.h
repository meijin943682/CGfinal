#ifndef TRIANGLEH
#define TRIANGLEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

class triangle : public hitable {
public:
  triangle() {}
  triangle(vec3 p[], vec3 col, float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) : hitable(col, w_ri, w_ti, m) { 
    for(int i = 0; i < 3; i++){
      point[i] = p[i];
      edge[i] = p[(i + 1) % 3] - p[i];
    }
    normal = unit_vector(cross(point[1] - point[0], point[2] - point[0]));
  }
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  void move(vec3 dir, float length){ for(int i = 0; i < 3; i++) point[i] += unit_vector(dir) * length; }

private:
  vec3 point[3];
  vec3 edge[3];
  vec3 normal;
};

bool triangle :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  if(dot(r.direction(), this -> normal) != 0){
    float t = dot(this -> point[0] - r.origin(), this -> normal) / dot(r.direction(), this -> normal);
    if(t > tmin && t < tmax){
      vec3 hit_point = r.origin() + t * r.direction();
      vec3 C[3];
      
      for(int i = 0; i < 3; i++){
        C[i] = hit_point - point[i];
      }

      if(dot(this -> normal, cross(this -> edge[0], C[0])) >= 0 && 
         dot(this -> normal, cross(this -> edge[1], C[1])) >= 0 && 
         dot(this -> normal, cross(this -> edge[2], C[2])) >= 0){
        hit_record rec(t, hit_point, unit_vector(this -> normal), this -> color, this -> w_r, this -> w_t, this -> material);
        rec_list.push_back(rec);
        return true;
      }
    }
  }
  return false;
}

#endif
