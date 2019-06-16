//================================================================================================= 
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <limits>
#include <iostream>

class vec3 {

public:
	vec3() {}
	vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
	float x() const { return e[0]; }
	float y() const { return e[1]; }
	float z() const { return e[2]; }
	float r() const { return e[0]; }
	float g() const { return e[1]; }
	float b() const { return e[2]; }

  inline vec3 operator +(const vec3 &v2) const { return vec3(this -> e[0] + v2.e[0], this -> e[1] + v2.e[1], this -> e[2] + v2.e[2]); }
  inline vec3 operator -(const vec3 &v2) const { return vec3(this -> e[0] - v2.e[0], this -> e[1] - v2.e[1], this -> e[2] - v2.e[2]); }
  inline vec3 operator *(const vec3 &v2) const { return vec3(this -> e[0] * v2.e[0], this -> e[1] * v2.e[1], this -> e[2] * v2.e[2]); }

	inline vec3& operator +=(const vec3 &v2){ for(int i = 0; i < 3; i++) e[i] += v2.e[i]; return *this; }
	inline vec3& operator -=(const vec3 &v2){ for(int i = 0; i < 3; i++) e[i] -= v2.e[i]; return *this; }
  inline bool operator ==(const vec3 &v2){ return  (abs(this -> e[0] - v2.e[0]) + abs(this -> e[1] - v2.e[1]) + abs(this -> e[2] - v2.e[2])) < std::numeric_limits<float>::epsilon(); }
	inline vec3 operator *(const float t) const { return vec3(this -> e[0] * t, this -> e[1] * t, this -> e[2] * t); }
	inline vec3 operator /(const float t) const{ return vec3(this -> e[0] / t, this -> e[1] / t, this -> e[2] / t); }

	inline vec3& operator *=(const float t){ for(int i = 0; i < 3; i++) e[i] *= t; return *this; }
	inline vec3& operator /=(const float t){ for(int i = 0; i < 3; i++) e[i] /= t; return *this; }
  
	inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	inline void make_unit_vector();

	float e[3];
};

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) 
{
  os << t.x() << " " << t.y() << " " << t.z();
  return os;
}


inline void vec3::make_unit_vector() 
{
  for(int i = 0; i < 3; i++){
    e[i] /= length();
  }
}

inline float dot(const vec3 &v1, const vec3 &v2) 
{
  return v1.e[0] * v2.e[0] +  v1.e[1] * v2.e[1] +  v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) 
{
  return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1], v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2], v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3 unit_vector(vec3 v) 
{
  return vec3(v.e[0] / v.length(), v.e[1] / v.length(), v.e[2] / v.length());
}

inline vec3 operator *(const float t, const vec3 &v){
  return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}


#endif
