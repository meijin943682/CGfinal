#ifndef CONEH
#define CONEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

class cone : public hitable {
public:
  cone() {}
  cone(vec3 p, vec3 n, float r, float h, vec3 col, float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) 
    : hitable(col, w_ri, w_ti, m), radius(r), height(h) {
      normal = unit_vector(n);
      point[0] = p;
      point[1] = p + normal * h;
      radian = atan(r / h);
  }
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  void move(vec3 dir, float length){ for(int i = 0; i < 2; i++) point[i] += unit_vector(dir) * length; }

private:
  vec3 point[2];
  vec3 normal;
  float radius;
  float height;
  double radian;
};

bool cone :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  double a, b, c;
  double t[3] = {tmax, tmax, tmax};
  vec3 deltaP = r.origin() - this -> point[0];

  vec3 rdir = r.direction() - dot(r.direction(), this -> normal) * this -> normal, 
       rpoint = deltaP - dot(deltaP, this -> normal) * this -> normal, 
       hdir = dot(r.direction(), this -> normal) * this -> normal, 
       hpoint = dot(deltaP, this -> normal) * this -> normal;

  a = pow(cos(radian), 2) * rdir.squared_length() - pow(sin(radian), 2) * hdir.squared_length();
  b = 2 * pow(cos(radian), 2) * dot(rdir, rpoint) - 2.0 * pow(sin(radian), 2) * dot(hdir, hpoint); 
  c = pow(cos(radian), 2) * rpoint.squared_length() - pow(sin(radian), 2) * hpoint.squared_length();
  
  if(b * b - 4 * a * c >= 0){
    t[0] = (-1 * b - sqrt(b * b - 4 * a * c)) / (2 * a);
    t[1] = (-1 * b + sqrt(b * b - 4 * a * c)) / (2 * a);
    
    t[0] = t[0] > tmin && t[0] < tmax? t[0] : tmax;
    t[1] = t[1] > tmin && t[1] < tmax? t[1] : tmax;
    
    vec3 pa = r.origin() + t[0] * r.direction();
    vec3 pb = r.origin() + t[1] * r.direction();

    t[0] = dot(this -> normal, pa - this -> point[0]) > 0? t[0] : tmax;
    t[1] = dot(this -> normal, pb - this -> point[0]) > 0? t[1] : tmax;
    t[0] = dot(this -> normal, pa - this -> point[0]) > 0 && dot(this -> normal, pa - this -> point[1]) < 0? t[0] : tmax;
    t[1] = dot(this -> normal, pb - this -> point[0]) > 0 && dot(this -> normal, pb - this -> point[1]) < 0? t[1] : tmax;
  }

  if(dot(r.direction(), this -> normal) != 0){
    t[2] = dot(this -> point[1] - r.origin(), this -> normal) / dot(r.direction(), this -> normal);
    
    t[2] = t[2] > tmin && t[2] < tmax? t[2] : tmax;

    vec3 pc = r.origin() + t[2] * r.direction();
    
    t[2] = (pc - point[1]).length() <= radius? t[2] : tmax;
  }

  for(int i = 0; i < 3; i++){
    if(tmin < t[i] && t[i] < tmax){
      vec3 hit_point = r.origin() + t[i] * r.direction();
      hit_record rec(t[i], hit_point, this -> normal, this -> color, this -> w_r, this -> w_t, this -> material);
      vec3 hitp_origin = rec.p - this -> point[0];
      rec.normal = i < 2? unit_vector(dot(hitp_origin, point[1] - point[0]) * hitp_origin + point[0] - point[1]) : this -> normal;
      rec.normal = rec.p == this -> point[0]? this -> normal * -1 : rec.normal;
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
