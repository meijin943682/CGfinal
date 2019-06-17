#ifndef CYLINDERH
#define CYLINDERH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

class cylinder : public hitable {
public:
  cylinder() {}
  cylinder(vec3 p, vec3 n, float r, float h, vec3 col, float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) 
    : hitable(w_ri, w_ti, m), radius(r), height(h), color(col) {
      normal = unit_vector(n);
      point[0] = p;
      point[1] = p + normal * h;
  }
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);

private:
  vec3 point[2];
  vec3 normal;
  float radius;
  float height;
  vec3 color;
};

bool cylinder :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  float a, b, c;
  float t[4] = {tmax, tmax, tmax, tmax};
  vec3 deltaP = r.origin() - this -> point[0];

  a = (r.direction() - dot(r.direction(), this -> normal) * this -> normal).squared_length();
  b = 2 * dot(r.direction() - dot(r.direction(), this -> normal) * this -> normal, deltaP - dot(deltaP, this -> normal) * this -> normal);
  c = (deltaP - dot(deltaP, this -> normal) * this -> normal).squared_length() - this -> radius * this -> radius;

  if(b * b - 4 * a * c >= 0){
    t[0] = (-1 * b - sqrt(b * b - 4 * a * c)) / (2 * a);
    t[1] = (-1 * b + sqrt(b * b - 4 * a * c)) / (2 * a);
    
    t[0] = t[0] > tmin && t[0] < tmax? t[0] : tmax;
    t[1] = t[1] > tmin && t[1] < tmax? t[1] : tmax;
    
    vec3 pa = r.origin() + t[0] * r.direction();
    vec3 pb = r.origin() + t[1] * r.direction();

    t[0] = dot(this -> normal, pa - this -> point[0]) > 0 && dot(this -> normal, pa - this -> point[1]) < 0? t[0] : tmax;
    t[1] = dot(this -> normal, pb - this -> point[0]) > 0 && dot(this -> normal, pb - this -> point[1]) < 0? t[1] : tmax;
  }
  if(dot(r.direction(), this -> normal) < 0){
    t[2] = dot(this -> point[0] - r.origin(), this -> normal) / dot(r.direction(), this -> normal);
    t[3] = dot(this -> point[1] - r.origin(), this -> normal) / dot(r.direction(), this -> normal);
    
    t[2] = t[2] > tmin && t[2] < tmax? t[2] : tmax;
    t[3] = t[3] > tmin && t[3] < tmax? t[3] : tmax;

    vec3 pc = r.origin() + t[2] * r.direction();
    vec3 pd = r.origin() + t[3] * r.direction();
    
    t[2] = (pc - point[0]).length() <= radius? t[2] : tmax;
    t[3] = (pd - point[1]).length() <= radius? t[3] : tmax;
  }
  for(int i = 0; i < 4; i++){
    if(tmin < t[i] && t[i] < tmax){
      vec3 hit_point = r.origin() + t[i] * r.direction();
      hit_record rec(t[i], hit_point, this -> normal, this -> color, this -> w_r, this -> w_t, this -> material);
      rec.normal = i < 2? unit_vector(rec.p - point[0] - dot(rec.p - point[0], this -> normal) * this -> normal) : this -> normal;
      rec_list.push_back(rec);
    }
  }
  if(!rec_list.empty()){
    sort(rec_list.begin(), rec_list.end(), compare);
    return true;
  }

  return false;
}

#endif
