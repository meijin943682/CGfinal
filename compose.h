#ifndef COMPOSEH
#define COMPOSEH

#include <math.h>
#include "vec3.h"
#include "hitable.h"
#include "ray.h"

vector<pair<hit_record, hit_record>> rec_pair_make(vector<hit_record>& rec_list, const ray& r, hit_record ht_min, hit_record ht_max){
  vector<pair<hit_record, hit_record> > pair_list;
  for(int i = 0; i < int(rec_list.size()); i += 1){
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

bool union_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record> com){
  return false;
}

bool difference_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record> com){
  return false;
}
bool intersection_rec(vector<pair<hit_record, hit_record> >& prime, vector<pair<hit_record, hit_record> >& sub, vector<hit_record> com){ 
  int prime_now = 0, sub_now = 0;
  while(prime_now < int(prime.size()) && sub_now < int(sub.size())){
    hit_record inter_begin, inter_end;
    if(prime[prime_now].first.t < sub[sub_now].first.t) inter_begin = sub[sub_now].first;
    else inter_begin = prime[prime_now].first;

    if(prime[prime_now].second.t < sub[sub_now].second.t) inter_end = prime[prime_now].second, prime_now += 1;
    else inter_end = sub[sub_now].second, sub_now += 1;

    if(inter_begin.t < inter_end.t)
      com.push_back(inter_begin), com.push_back(inter_end);
  }
  if(com.size() != 0) return true;
  return false;
}

class compose : public hitable {
public:
  compose() {}
  compose(hitable* obj_prime, hitable* obj_sub, int op) : object_prime(obj_prime), object_sub(obj_sub), operation(op){ 
  }
  bool hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list);
  vec3 colors(hit_record record){ return vec3(1, 1, 1); }
  

private:
  hitable* object_prime;
  hitable* object_sub;
  int operation;
};

bool compose :: hit(const ray& r, float tmin, float tmax, vector<hit_record>& rec_list) {
  hit_record ht_min(tmin, r.origin() + tmin * r.direction(), r.direction()), ht_max(tmax, r.origin() + tmax * r.direction(), r.direction());
  vector<hit_record> prime_rec_list, sub_rec_list;

  bool (*func[])(vector<pair<hit_record, hit_record> >&, vector<pair<hit_record, hit_record> >&, vector<hit_record>) = 
    {union_rec, difference_rec, intersection_rec};

  if(this -> object_prime -> hit(r, tmin, tmax, prime_rec_list) || this -> object_sub -> hit(r, tmin, tmax, sub_rec_list)){
    vector<pair<hit_record, hit_record> > prime_pair_list = rec_pair_make(prime_rec_list, r, ht_min, ht_max),
                                          sub_pair_list = rec_pair_make(prime_rec_list, r, ht_min, ht_max);

    if(func[this -> operation](prime_pair_list, sub_pair_list, rec_list))
      return true;
  }
  return false;
}

#endif