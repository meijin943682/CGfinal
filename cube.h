:/*void square(vec3 start, vec3 width, vec3 height, vec3 col, float w_r = 0.0f, float w_t = 0.0f, float m = 1.0f){

  vec3 point[3];
  
  point[0] = start, point[1] = start + width, point[2] = start + width + height;
  hitable_list.push_back(new triangle(point, col, w_r, w_t, m));
 
  point[0] = start + width + height, point[1] = start + height, point[2] = start;
  hitable_list.push_back(new triangle(point, col, w_r, w_t, m));
}

void cube(vec3 left_down_point, vec3 width, vec3 height, vec3 depth, vec3 col, float w_r = 0.0f, float w_t = 0.0f, float m = 1.0f){

  vec3 right_up_point = left_down_point + width + height + depth;
  
  square(left_down_point, width, height, col, w_r, w_t, m);
  square(right_up_point, height, width, col, w_r, w_t, m);
  
  square(left_down_point, depth, width, col, w_r, w_t, m);
  square(right_up_point, width, depth, col, w_r, w_t, m);
  
  square(left_down_point, height, depth, col, w_r, w_t, m);
  square(right_up_point, depth, height, col, w_r, w_t, m);
}
*/
