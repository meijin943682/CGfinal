#include <iostream>
#include <fstream>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "cylinder.h"
#include "cone.h"
#include "compose.h"
#include "camera.h"

#define TMIN 0.001f
#define TMAX 10000
#define TRACEMAX 1

using namespace std;
vector<hitable*> hitable_list;

vec3 refract(vec3& I, vec3& N, float eta, bool inner){
  eta = inner? 1 / eta : eta;
  N = N * (inner? -1 : 1);
  float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
  vec3 R;
    if (k < 0.0)
      R = vec3(0, 0, 0);
    else
      R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;
  return unit_vector(R);
}

vec3 shading(vec3 lightSource, vec3 lightIntensity, hit_record ht, vec3 Kd, bool inner)
{
  vec3 lightDir = unit_vector(lightSource - ht.p);
  vec3 lightOffset = 0.5 * unit_vector(vec3(0, 1, 0) - dot(vec3(0, 1, 0), lightDir) * lightDir);
  ray shadowRay[3];
  shadowRay[0] = ray(ht.p, lightDir);
  shadowRay[1] = ray(ht.p, unit_vector(lightSource + lightOffset - ht.p));
  shadowRay[2] = ray(ht.p, unit_vector(lightSource - lightOffset - ht.p));
  
  vector<hit_record> new_record_list;
  hitable* it;
  vec3 color(0, 0, 0);
  float shadowRatio[3] = {0.5, 0.25, 0.25};

  for(int i = 0; i < 3; i++){
    bool flag = false;
    vec3 color_new;
    for(vector<hitable*> :: iterator iter = hitable_list.begin(); 
        iter != hitable_list.end(); iter++){
      new_record_list.clear();
      if((*iter) -> hit(shadowRay[i], TMIN, TMAX, new_record_list)){
        flag = true;
        it = *iter;
      }
    }
    if(!flag){
	    color_new = max(float(0), dot(ht.normal * (inner? -1 : 1), shadowRay[i].direction())) * lightIntensity * Kd;
    }
    else{
	    color_new = (it -> w_r + it -> w_t) / 2 * max(float(0), dot(ht.normal * (inner? -1 : 1), shadowRay[i].direction())) * lightIntensity * Kd 
        + (1 - (it -> w_r + it -> w_t) / 2) * vec3(0, 0, 0);
    }
    color += (shadowRatio[i] * color_new);
  }
  return color;
}

vec3 trace(vec3 p, vec3 d, int step, bool inner, float lightRatio){
  vec3 local(0, 0, 0), reflected(0, 0, 0), transmitted(0, 0, 0);
  ray r(p, d);
  hitable* it;
  
  hit_record record(TMAX, vec3(0, 0, 0), vec3(0, 0, 0));
  vector<hit_record> new_record_list;

  vec3 lightPosition(0, 10, 10);
  vec3 lightIntensity = vec3(1, 1, 1); 
  
  if(step >= TRACEMAX || lightRatio < 0){
    return vec3(0, 0, 0); // black or background
  }

  for(vector<hitable*> :: iterator iter = hitable_list.begin(); 
      iter != hitable_list.end(); iter++){
    new_record_list.clear();
    if((*iter) -> hit(r, TMIN, TMAX, new_record_list) && record.t > new_record_list[0].t){
      record = new_record_list[0];
      it = *iter;
    }
  }

  if(record.t != TMAX){
    float length = record.t * r.direction().length();
    local = shading(lightPosition, lightIntensity, record, it -> colors(record), inner);
    reflected = trace(record.p, unit_vector(d - 2 * dot(d, record.normal) * record.normal), step + 1, inner, lightRatio - length / 50);
    transmitted = trace(record.p, refract(d, record.normal, 1.0f / it -> material, inner), step + 1, !inner, lightRatio - length / 50);
  }
  else{
    float t = 0.5 * (d.y() + 1.0);
    local = (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1.0);
    return local;
  }

  return  local 
          + it -> w_r * reflected + it -> w_t * transmitted;
}

int main()
{
  int width = 200 * 3;
  int height = 200 * 2;

  fstream file;
  file.open("ray.ppm", ios::out);

  vec3 lower_left_corner(-2, -1, -1);
  vec3 origin(0, 0, 1);
  vec3 horizontal(4, 0, 0);
  vec3 vertical(0, 2, 0);
  camera cam(vec3(0, 0, 10), vec3(0, 0, -1), vec3(0, 1, 0), 60,  float(width) / float(height), 0.1, 11);

  file << "P3\n" << width << " " << height << "\n255\n";

  //hitable_list.push_back(new plane(vec3(0, -2, 0), vec3(0, 1, 0), vec3(1.0f, 0.0f, 0.0f))); //ground
  //hitable_list.push_back(new sphere(vec3(0, 0, -2), 0.5, vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.9f, 1.46f));
  //hitable_list.push_back(new sphere(vec3(1, 0, -1.75), 0.5, vec3(1.0f, 1.0f, 1.0f), 0.9f));
  //hitable_list.push_back(new sphere(vec3(-1, 0, -2.25), 0.5, vec3(1.0f, 0.7f, 0.3f)));
  //hitable_list.push_back(new cylinder(vec3(2, 0.5, -2), vec3(0, -1, 0), 0.5, 0.5, vec3(1.0f, 1.0f, 1.0f)));
  //hitable_list.push_back(new cone(vec3(3, 0.5, -2), vec3(0, 1, 0), 5, 1, vec3(1.0f, 1.0f, 1.0f)));
  //hitable_list.push_back(new cone(vec3(5, 0, -10), vec3(0, -1, 0), 3, 1, vec3(1.0f, 1.0f, 1.0f)));
  hitable_list.push_back(new compose(new sphere(vec3(5.5, 0.5, -10), 1, vec3(1.0f, 1.0f, 1.0f)), 
                                     new compose(new sphere(vec3(5, 0, -10), 1, vec3(1.0f, 1.0f, 1.0f)), 
                                                 new sphere(vec3(6, 0, -10), 1, vec3(1.0f, 1.0f, 1.0f)), 2), 2));
  //cube(vec3(0, 0, -2), vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, -1), vec3(1.0f, 1.0f, 0.0f));
  srand(1234);

  vec3 colorlist[8] = { vec3(0.8, 0.3, 0.3), vec3(0.3, 0.8, 0.3), vec3(0.3, 0.3, 0.8),
    vec3(0.8, 0.8, 0.3), vec3(0.3, 0.8, 0.8), vec3(0.8, 0.3, 0.8),
    vec3(0.8, 0.8, 0.8), vec3(0.3, 0.3, 0.3) };

  /*for (int i = 0; i < 48; i++) {
    float xr = ((float)rand() / (float)(RAND_MAX)) * 6.0f - 3.0f;
    float zr = ((float)rand() / (float)(RAND_MAX)) * 3.0f - 1.5f;
    int cindex = rand() % 8;
    float rand_reflec = ((float)rand() / (float)(RAND_MAX));
    float rand_refrac = ((float)rand() / (float)(RAND_MAX));
    hitable_list.push_back(new sphere(vec3(xr, -0.45, zr - 2), 0.1, colorlist[cindex], rand_reflec, 0));
  }*/

  for (int j = height - 1; j >= 0; j--) {
    for (int i = 0; i < width; i++) {
      float u[2], v[2];
      u[0] = float(i) / float(width);
      v[0] = float(j) / float(height);
      u[1] = (float(i) + 0.5)/float(width);
      v[1] = (float(j) - 0.5) / float(height);

      vec3 color_mix(0, 0, 0);
      for(int h = 0; h < 2; h++){
        for(int k = 0; k < 2; k++){
          ray r = cam.get_ray(u[h], v[k]);
          color_mix = color_mix + trace(r.origin(), unit_vector(r.direction()), 0, false, 1.0);
        }
      }
      vec3 mean_color = color_mix / 4;
      mean_color = vec3(min(1.0f, mean_color.x()), min(1.0f, mean_color.y()), min(1.0f, mean_color.z()));
      file << int(mean_color.r() * 255) << " " << int(mean_color.g()  * 255) << " " << int(mean_color.b() * 255) << "\n";
    }
  }
  return 0;
}
