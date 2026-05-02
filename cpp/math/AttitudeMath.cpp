#include "math/AttitudeMath.hpp"
namespace satgcs::math {
Euler syntheticAttitude(double t) {
    return { 2.0 * std::sin(t/8.0), 1.2 * std::cos(t/10.0), clampDegrees(90.0 + 0.08*t) };
}
}
