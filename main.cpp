#include <iostream>
#include <fstream>
#include <vector>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "cube.h"
#include "cylinder.h"
#include "cone.h"
#include "compose.h"
#include "camera.h"

#define TMIN 0.001f
#define TMAX 10000
#define TRACEMAX 5
#define SPECULAR 49
#define WIDTH 600
#define HEIGHT 300
#define TIME 100

using namespace std;
vector<hitable*> hitable_list;
vector<pair<vec3, vec3> > light_list;

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

vec3 shading(hit_record ht, bool inner, vec3 reflect_ray, float lightRatio)
{
  vec3 color(0, 0, 0);
  for(int i = 0; i < int(light_list.size()); i += 1){
    vec3 lightSource = light_list[i].first;
    vec3 lightIntensity = light_list[i].second;

    vec3 lightDir = unit_vector(lightSource - ht.p), Kd = ht.color;
    vec3 lightOffset = 0.5 * unit_vector(vec3(0, 1, 0) - dot(vec3(0, 1, 0), lightDir) * lightDir);
    hit_record record(TMAX, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0));

    ray shadowRay[3];
    shadowRay[0] = ray(ht.p, lightDir);
    shadowRay[1] = ray(ht.p, unit_vector(lightSource + lightOffset - ht.p));
    shadowRay[2] = ray(ht.p, unit_vector(lightSource - lightOffset - ht.p));

    vector<hit_record> new_record_list;
    float shadowRatio[3] = {0.5, 0.25, 0.25};

    for(int i = 0; i < 3; i++){
      bool flag = false;
      vec3 color_new;
      for(vector<hitable*> :: iterator iter = hitable_list.begin(); 
          iter != hitable_list.end(); iter++){
        new_record_list.clear();
        if((*iter) -> hit(shadowRay[i], TMIN, TMAX, new_record_list) && record.t > new_record_list[0].t){
          flag = true;
          record = new_record_list[0];
        }
      }
      if(!flag){
        lightRatio = lightRatio > 0.5 ? lightRatio : 0;
        color_new = max(float(0), dot(ht.normal * (inner? -1 : 1), shadowRay[i].direction())) * lightIntensity * Kd
                    + max(double(0), pow(dot(reflect_ray, lightDir * -1), SPECULAR)) * lightIntensity * lightRatio;
      }
      else{
        float alpha = (1 - record.w_r) * (1 - record.w_t);
        color_new = (1 - alpha) * max(float(0), dot(ht.normal * (inner? -1 : 1), shadowRay[i].direction())) * lightIntensity * Kd 
          + alpha * vec3(0, 0, 0);
      }
      color += (shadowRatio[i] * color_new);
    }
  }
  return color;
}

vec3 trace(vec3 p, vec3 d, int step, bool inner, float lightRatio, vec3 init){
  vec3 local(0, 0, 0), reflected(0, 0, 0), transmitted(0, 0, 0);
  ray r(p, d);
  
  hit_record record(TMAX, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0));
  vector<hit_record> new_record_list;
  
  if(step >= TRACEMAX || lightRatio < 0){
    return vec3(0, 0, 0); // black or background
  }

  for(vector<hitable*> :: iterator iter = hitable_list.begin(); 
      iter != hitable_list.end(); iter++){
    new_record_list.clear();
    if((*iter) -> hit(r, TMIN, TMAX, new_record_list) && record.t > new_record_list[0].t)
      record = new_record_list[0];
  }

  if(record.t != TMAX){
    float length = record.t * r.direction().length();
    lightRatio -= length / 20;
    vec3 reflect_ray = unit_vector(d - 2 * dot(d, record.normal) * record.normal);
    local = shading(record, inner, reflect_ray, lightRatio);
    reflected = trace(record.p, reflect_ray, step + 1, inner, lightRatio, init);
    transmitted = trace(record.p, refract(d, record.normal, 1.0f / record.material, inner), step + 1, !inner, lightRatio, init);
    /*if(vec3(0.698593, -4.34437, -10.2169).approch(init)){
      cout << step << endl;
      cout << "object = " << record.p << "color = " << record.color << endl;
      cout << "local = " << local << endl;
      cout << "reflected = " << reflected << endl;
      cout << "trnsmitted = " << transmitted << endl << endl;
    }
    */
  }
  else{
    float t = 0.5 * (d.y() + 1.0);
    local = (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1.0);
    /*if(vec3(0.698593, -4.34437, -10.2169).approch(init)){
      cout << step << " " << t << " " << d.y() << endl;
      cout << "local = " << local << endl << endl;
    }
    */
    return local;
  }
  return  (1.0 - record.w_t) * ((1.0 - record.w_r) * local + record.w_r * reflected) + record.w_t * transmitted;
}

void scene_create(){
  
  light_list.push_back(make_pair(vec3(-10, 0, 0), vec3(1, 1, 1)));
  light_list.push_back(make_pair(vec3(10, 0, 0), vec3(1, 1, 1)));
  
  
  hitable_list.push_back(new plane(vec3(0, -5, 0), vec3(0, 1, 0), vec3(1.0f, 0.0f, 0.0f))); //ground
  
  hitable_list.push_back(new compose(
                         new compose(
                         new compose(
                         new compose(new cube(vec3(-2, -2, -2), vec3(4, 0, 0), vec3(0, 4, 0), 4, vec3(1.0f, 1.0f, 0.0f)),
                                     new cube(vec3(-1.5, -1.5, -1), vec3(3, 0, 0), vec3(0, 3, 0), 6, vec3(1.0f, 0.0f, 0.0f)), 1),
                                     new cube(vec3(-3, -1.5, -2.5), vec3(6, 0, 0), vec3(0, 3, 0), 3, vec3(0.0f, 0.0f, 1.0f)), 1),
                                     new cube(vec3(-1.5, -3, -2.5), vec3(3, 0, 0), vec3(0, 6, 0), 3, vec3(0.0f, 1.0f, 0.0f)), 1),
                                     new sphere(vec3(0, 0, -4), 1.5, vec3(1, 1, 1), 0.0f, 0.9f, 1.46f), 0)); 

  hitable_list.push_back(new compose(new sphere(vec3(4, 0, -4), 1.5, vec3(1, 1, 1), 0.0f, 0.9f, 1.4), 
                                     new sphere(vec3(5, 0, -4), 1.5, vec3(1, 1, 1), 0.9f), 1));
  
  hitable_list.push_back(new compose(new sphere(vec3(-4, 0, -4), 1.5, vec3(1, 0, 0)), 
                                     new sphere(vec3(-5, 0, -4), 1.5, vec3(0, 1, 0)), 0));
  
  //hitable_list.push_back(new compose(new sphere(vec3(0, 0, -5), 4, vec3(1, 0 , 0), 0.0f, 0.9f, 1.4), 
  //                                   new sphere(vec3(0, 0, -5), 2.5, vec3(0, 0, 1), 0.9f), 1));

  //hitable_list.push_back(new sphere(vec3(-9, 0, -10), 2, vec3(1, 0 , 0)));
  //hitable_list.push_back(new cube(vec3(-6.5, -2, -10), vec3(4, 0, 0), vec3(0, 4, 0), 4, vec3(1, 0 ,0)));
  //hitable_list.push_back(new cylinder(vec3(0, 2, -10), vec3(0, -1, 0), 1, 4, vec3(0, 1, 0)));
  //hitable_list.push_back(new cone(vec3(3, 2, -10), vec3(0, -1, 0), 1, 4, vec3(0, 0, 1)));
  /*
  srand(1234);

  vec3 colorlist[8] = { vec3(0.8, 0.3, 0.3), vec3(0.3, 0.8, 0.3), vec3(0.3, 0.3, 0.8),
    vec3(0.8, 0.8, 0.3), vec3(0.3, 0.8, 0.8), vec3(0.8, 0.3, 0.8),
    vec3(0.8, 0.8, 0.8), vec3(0.3, 0.3, 0.3) };

  for (int i = 0; i < 12; i++) {
    float xr = ((float)rand() / (float)(RAND_MAX)) * 60 - 30;
    float yr = ((float)rand() / (float)(RAND_MAX)) * 30 - 15;
    int cindex = rand() % 8;
    float rand_reflec = ((float)rand() / (float)(RAND_MAX));
    rand_reflec = rand_reflec > 0.5 ? rand_reflec : 0;
    hitable_list.push_back(new sphere(vec3(xr, yr, -15), 3, colorlist[cindex], rand_reflec));
  }
  */
}

int main()
{
  vec3 origin(0, 0, 1);
  vec3 horizontal(4, 0, 0);
  vec3 vertical(0, 2, 0);
  vec3 point(0, 3, 10);
  camera cam(point, vec3(0, 0, -1), vec3(0, 1, 0), 60,  float(WIDTH) / float(HEIGHT), 0.1, 11);
  scene_create();
  
  int interval = TIME;
  bool isMoving = false;

  for(int i = 0; i < TIME; i += interval){
    fstream file;
    file.open("output/ray" + to_string(i) + ".ppm", ios::out);
    file << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
    
    for (int j = HEIGHT - 1; j >= 0; j--) {
      for (int i = 0; i < WIDTH; i++) {
        float u[2], v[2];
        u[0] = float(i) / float(WIDTH);
        v[0] = float(j) / float(HEIGHT);
        u[1] = (float(i) + 0.5) / float(WIDTH);
        v[1] = (float(j) - 0.5) / float(HEIGHT);

        vec3 color_mix(0, 0, 0);
        for(int h = 0; h < 2; h++){
          for(int k = 0; k < 2; k++){
            ray r = cam.get_ray(u[h], v[k]);
            color_mix = color_mix + trace(r.origin(), unit_vector(r.direction()), 0, false, 1.0, r.direction());
          }
        }
        vec3 mean_color = color_mix / 4;
        mean_color = vec3(min(1.0f, mean_color.x()), min(1.0f, mean_color.y()), min(1.0f, mean_color.z()));
        file << int(mean_color.r() * 255) << " " << int(mean_color.g()  * 255) << " " << int(mean_color.b() * 255) << "\n";
      }
    }
    
    file.close();
    if(isMoving == true){
      int dir = (i > 25 && i <= 75) ? 1 : -1;
      for(int i = 1; i < int(hitable_list.size()); i++)
        hitable_list[i] -> move(vec3(dir, 0, 0), 0.4 * interval);
    }
  }
  return 0;
}
