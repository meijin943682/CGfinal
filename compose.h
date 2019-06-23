#ifndef COMPOSEH
#define COMPOSEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

hit_record ht_min, ht_max;

vector<pair<hit_record, hit_record>> rec_pair_make(vector<hit_record>& rec_list, const ray& r){
  vector<pair<hit_record, hit_record> > pair_list;
  for(int i = 0; i < int(rec_list.size()); i+= 1){
    if(i == 0 && dot(rec_list[i].normal, r.direction()) > 0)
      pair_list.push_back(make_pair(ht_min, rec_list[i]));
    else if(i + 1 == int(rec_list.size()))
      pair_list.push_back(make_pair(rec_list[i], ht_max));
    else{
      pair_list.push_back(make_pair(rec_list[i], rec_list[i + 1]));
      i += 1;
    }
  }
  return pair_list;
}

bool union_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record>& com){
  int prime_now = 0, sub_now = 0;
  while(prime_now < int(prime.size()) || sub_now < int(sub.size())){
    if(prime_now < int(prime.size()) && sub_now < int(sub.size())){
      if(prime[prime_now].second.t <= sub[sub_now].first.t){
        com.push_back(prime[prime_now].first), com.push_back(prime[prime_now].second);
        prime_now += 1;
      }
      else if(sub[sub_now].second.t <= prime[prime_now].first.t){
        com.push_back(sub[sub_now].first), com.push_back(sub[sub_now].second);
        sub_now += 1;
      }
      else if(sub[sub_now].second.t <= prime[prime_now].second.t){
        prime[prime_now].first = prime[prime_now].first.t <= sub[sub_now].first.t? prime[prime_now].first : sub[sub_now].first;
        sub_now += 1;
      }
      else if(prime[prime_now].second.t <= sub[sub_now].second.t){
        sub[sub_now].first = sub[sub_now].first.t <= prime[prime_now].first.t? sub[sub_now].first : prime[prime_now].first;
        prime_now += 1;
      }
    }
    else if(prime_now < int(prime.size())){
      com.push_back(prime[prime_now].first), com.push_back(prime[prime_now].second);
      prime_now += 1;
    }
    else if(sub_now < int(sub.size())){
      com.push_back(sub[sub_now].first), com.push_back(sub[sub_now].second);
      sub_now += 1;
    }
  }
  if(com.size() != 0){ 
    if(com[0].t == ht_min.t) 
      com.erase(com.begin());
    return true;
  }
  return false;
}

bool difference_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record>& com){
  int prime_now = 0, sub_now = 0;
  while(prime_now < int(prime.size()) && sub_now < int(sub.size())){
    hit_record record_now = prime[prime_now].first;
    if(prime[prime_now].second.t <= sub[sub_now].second.t){
      if(prime[prime_now].first.t < sub[sub_now].first.t){
        record_now.t = sub[sub_now].first.t, record_now.p = sub[sub_now].first.p, record_now.normal = sub[sub_now].first.normal * -1;
        record_now.color = sub[sub_now].first.color;
        com.push_back(prime[prime_now].first), com.push_back(record_now);
      }
      prime_now += 1;
    }
    else if(sub[sub_now].second.t <= prime[prime_now].second.t){
      if(prime[prime_now].first.t < sub[sub_now].first.t){
        record_now.t = sub[sub_now].first.t, record_now.p = sub[sub_now].first.p, record_now.normal = sub[sub_now].first.normal * -1;
        record_now.color = sub[sub_now].first.color;
        com.push_back(prime[prime_now].first), com.push_back(record_now);
      }
      record_now.t = sub[sub_now].second.t, record_now.p = sub[sub_now].second.p, record_now.normal = sub[sub_now].second.normal * -1;
      record_now.color = sub[sub_now].second.color;
      prime[prime_now].first = record_now;
      sub_now += 1;
    }
  }
  while(prime_now < int(prime.size())){
    com.push_back(prime[prime_now].first), com.push_back(prime[prime_now].second);
    prime_now += 1;
  }
  if(com.size() != 0){ 
    if(com[0].t == ht_min.t) 
      com.erase(com.begin());
    return true;
  }
  return false;
}
bool intersection_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record>& com){ 
  int prime_now = 0, sub_now = 0;
  while(prime_now < int(prime.size()) && sub_now < int(sub.size())){
    hit_record inter_begin, inter_end;

    if(prime[prime_now].first.t < sub[sub_now].first.t) 
      inter_begin = sub[sub_now].first;
    else
      inter_begin = prime[prime_now].first;

    if(prime[prime_now].second.t < sub[sub_now].second.t)
      inter_end = prime[prime_now].second, prime_now += 1;
    else
      inter_end = sub[sub_now].second, sub_now += 1;

    if(inter_begin.t < inter_end.t)
      com.push_back(inter_begin), com.push_back(inter_end);
  }
  if(com.size() != 0){ 
    if(com[0].t == ht_min.t) 
      com.erase(com.begin());
    return true;
  }
  return false;
}

class compose : public hitable {
public:
  compose() {}
  compose(hitable* obj_prime, hitable* obj_sub, int op) : object_prime(obj_prime), object_sub(obj_sub), operation(op){}
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  void move(vec3 dir, float length){ object_prime -> move(unit_vector(dir), length), object_sub -> move(unit_vector(dir), length); }

private:
  hitable* object_prime;
  hitable* object_sub;
  int operation;
};

bool compose :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  hit_record hit_min(tmin, r.origin() + tmin * r.direction(), r.direction(), 
                      object_prime -> color, object_prime -> w_r, object_prime -> w_t, object_prime -> material),
             hit_max(tmax, r.origin() + tmax * r.direction(), r.direction(), 
                      object_prime -> color, object_prime -> w_r, object_prime -> w_t, object_prime -> material);
  vector<hit_record> prime_rec_list, sub_rec_list;
  ht_min = hit_min, ht_max = hit_max;

  bool (*func[])(vector<pair<hit_record, hit_record> >&, vector<pair<hit_record, hit_record> >&, vector<hit_record>&)= 
        {union_rec, difference_rec, intersection_rec};

  bool prime_hit = this -> object_prime -> hit(r, tmin, tmax, prime_rec_list), sub_hit = this -> object_sub -> hit(r, tmin, tmax, sub_rec_list);
  if(prime_hit || sub_hit){
    vector<pair<hit_record, hit_record> > prime_pair_list = rec_pair_make(prime_rec_list, r),
                                          sub_pair_list = rec_pair_make(sub_rec_list, r);
    
    if(func[this -> operation](prime_pair_list, sub_pair_list, rec_list))
      return true;
  }
  return false;
}

#endif

