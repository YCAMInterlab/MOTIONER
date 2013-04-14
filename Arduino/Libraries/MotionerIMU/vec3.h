#pragma once

#include "Arduino.h"

class vec3 {
public:
  float x, y, z;  
  vec3():
  x(0), y(0), z(0) {
  }
  vec3(float x, float y, float z) :
  x(x), y(y), z(z) {
  }
  void set(const float* v) {
    x = v[0], y = v[1], z = v[2];
  }
  void set(float vx, float vy, float vz) {
    x = vx, y = vy, z = vz;
  }
  void set(const vec3& v) {
    x = v.x, y = v.y, z = v.z;
  }
  void zero() {
    x = 0, y = 0, z = 0;
  }
  float length() {
    return sqrt(x * x + y * y + z * z);
  }
  boolean operator==(const vec3& v) {
    return x == v.x && y == v.y && z == v.z;
  }
  boolean operator!=(const vec3& v) {
    return x != v.x || y != v.y || z != v.z;
  }
  void operator+=(const vec3& v) {
    x += v.x, y += v.y, z += v.z;
  }
  void operator-=(const vec3& v) {
    x -= v.x, y -= v.y, z -= v.z;
  }
  void operator/=(float v) {
    x /= v, y /= v, z /= v;
  }
  void operator*=(float v) {
    x *= v, y *= v, z *= v;
  }
  vec3 operator+(const vec3& v) {
    vec3 c = *this; 
    c += v; 
    return c;
  }
  vec3 operator-(const vec3& v) {
    vec3 c = *this; 
    c -= v; 
    return c;
  }
  vec3 operator/(float v) {
    vec3 c = *this; 
    c /= v; 
    return c;
  }
  vec3 operator*(float v) {
    vec3 c = *this; 
    c *= v; 
    return c;
  }
  operator float*() {
    return (float*) this;
  }
    
  float length() const {
    return sqrt(x*x + y*y + z*z);
  }
};
