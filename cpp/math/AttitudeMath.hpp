#pragma once
#include <cmath>
namespace satgcs::math {
struct Euler { double roll; double pitch; double yaw; };
inline double clampDegrees(double v) { while(v > 180.0) v -= 360.0; while(v < -180.0) v += 360.0; return v; }
Euler syntheticAttitude(double tSeconds);
}
