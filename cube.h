#ifndef CUBEH
#define CUBEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

void square(vec3 start, vec3 width, vec3 height, vec3 col, vector<triangle>& faces, float w_r = 0.0f, float w_t = 0.0f, float m = 1.0f){
  vec3 point[3];
  
  point[0] = start, point[1] = start + width, point[2] = start + width + height;
  faces.push_back(triangle(point, col, w_r, w_t, m));
 
  point[0] = start + width + height, point[1] = start + height, point[2] = start;
  faces.push_back(triangle(point, col, w_r, w_t, m));
}

class cube : public hitable {
public:
  cube() {}
  cube(vec3 ld_point, vec3 w, vec3 h, float d, vec3 col, float w_r = 0.0f, float w_t = 0.0f, float m = 1.0f)
  : left_down_point(ld_point), width(w), height(h), color(col){
    depth = d * unit_vector(cross(h, w));
    vec3 right_up_point = left_down_point + width + height + depth;

    square(left_down_point, width, height, col, faces, w_r, w_t, m);
    square(right_up_point, height * -1, width * -1, col, faces, w_r, w_t, m);
  
    square(left_down_point, depth, width, col, faces, w_r, w_t, m);
    square(right_up_point, width * -1, depth * -1, col, faces, w_r, w_t, m);
  
    square(left_down_point, height, depth, col, faces, w_r, w_t, m);
    square(right_up_point, depth * -1, height * -1, col, faces, w_r, w_t, m);
  }
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);

private:
  vector<triangle> faces;
  vec3 left_down_point;
  vec3 width;
  vec3 height;
  vec3 depth;
  vec3 color;
};

bool cube :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list){
  for(int i = 0; i < int(faces.size()); i++){
    faces[i].hit(r, tmin, tmax, rec_list);
  }
  if(!rec_list.empty()){
    sort(rec_list.begin(), rec_list.end(), compare);
    return true;
  }
  return false;
}

#endif



