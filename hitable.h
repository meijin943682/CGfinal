#ifndef HITABLEH
#define HITABLEH

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct hit_record {
  hit_record(){}
  hit_record(float T, vec3 P, vec3 Normal) : t(T), p(P), normal(Normal){}
  float t;
  vec3 p;
  vec3 normal;
};

class hitable {
public:
  hitable() {};
  hitable(float w_ri, float w_ti, float m) : w_r(w_ri), w_t(w_ti), material(m) {};
  virtual bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) = 0;
  virtual vec3 colors(hit_record record) = 0;

  float w_r;
  float w_t;
  float material;
};

bool compare(hit_record& a, hit_record& b){
  return a.t <= b.t;
}
#endif

