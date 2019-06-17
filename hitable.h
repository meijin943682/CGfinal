#ifndef HITABLEH
#define HITABLEH

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class hit_record {
public:
  hit_record(){}
  hit_record(float T, vec3 P, vec3 Normal, vec3 col, float w_ri = 0.0f, float w_ti = 0.0f, float m = 1.0f) 
    : t(T), p(P), normal(Normal), color(col), w_r(w_ri), w_t(w_ti), material(m){}
  float t;
  vec3 p;
  vec3 normal;
  vec3 color;
  float w_r;
  float w_t;
  float material;
};

class hitable {
public:
  hitable() {};
  hitable(float w_ri, float w_ti, float m) : w_r(w_ri), w_t(w_ti), material(m) {};
  virtual bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) = 0;

  float w_r;
  float w_t;
  float material;
};

bool compare(hit_record& a, hit_record& b){
  return a.t <= b.t;
}
#endif

