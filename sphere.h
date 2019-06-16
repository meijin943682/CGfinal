#ifndef SPHEREH
#define SPHEREH

#include <math.h>
#include "vec3.h"
#include "hitable.h"

class sphere: public hitable {
public:
  sphere() {};
  sphere(vec3 c, float r, vec3 col, float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) : 
  hitable(w_ri, w_ti, m), center(c), radius(r), color(col) {};
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  vec3 colors(hit_record record){ return color; }

private:
  vec3 center;
  float radius;
  vec3 color;
};

bool sphere::hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list){
  float a, b, c;
  a = r.direction().squared_length();
  b = dot(r.direction(), r.origin() - this -> center) * 2;
  c = (r.origin() - this -> center).squared_length() - this -> radius * this -> radius;
  
  if(b * b - 4 * a * c >= 0){
    float t[2];
    t[0] = (-1 * b - sqrt(b * b - 4 * a * c)) / (2 * a);
    t[1] = (-1 * b + sqrt(b * b - 4 * a * c)) / (2 * a);
    for(int i = 0; i < 2; i++){
      if(t[i] > tmin && t[i] < tmax){
        hit_record rec;
        rec.t = t[i];
        rec.p = r.origin() + t[i] * r.direction();
        rec.normal = unit_vector(rec.p - this -> center);
        rec_list.push_back(rec);
      }
    }
    if(!rec_list.empty()){
      sort(rec_list.begin(), rec_list.end(), compare);
      return true;
    }
    /*ta = ta > tmin? ta : tmax;
    tb = tb > tmin? tb : tmax;
    if((ta > tmin && ta < tmax) || (tb > tmin && tb < tmax)){
      float t = ta > tb? tb : ta;
      rec.t = t;
      rec.p = r.origin() + t * r.direction();
      rec.normal = unit_vector(rec.p - this -> center);
      return true;
    }*/
  }
  return false;
}

#endif
