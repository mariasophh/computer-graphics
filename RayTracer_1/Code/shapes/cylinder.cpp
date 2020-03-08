#include "cylinder.h"

#include <cmath>

using namespace std;

Hit Cylinder::intersect(Ray const &ray)
{
  Vector OC = (ray.O - position);

  // compute z_min and z_max
  double z_min = position.z;
  double z_max = z_min + direction.z;

  // Quadratic equation terms
  double a, b, c, discr;

  a = ray.D.x * ray.D.x + ray.D.y * ray.D.y;
  b = 2*(OC.x * ray.D.x + OC.y * ray.D.y);
  c = OC.x * OC.x + OC.y * OC.y - radius * radius;

  // compute the discriminant, check if it's negative
  discr = b*b - 4*a*c;

  // NO INTERSECTION
  if(discr < 0) return Hit::NO_HIT();

  // Compute t roots
  double t1 = (-b + sqrt(discr)) / (2*a);
  double t2 = (-b - sqrt(discr)) / (2*a);

  // Compute z1 and z2
  double z1 = OC.z + t1 * ray.D.z;
  double z2 = OC.z + t2 * ray.D.z;

  // Compute t
  double t = 0;

  if(z_min < z1 && z1 < z_max && t1 > 0) {
    if(z_min < z2 && z2 < z_max && t2 > 0) {
      t = fmin(t1, t2);
    } else {
      t = t1;
    }
  } else if(z_min < z2 && z2 < z_max && t2 < 0) {
    t = t2;
  }

  if(t==0) return Hit::NO_HIT();

  Point intersection = ray.at(t);
  Vector N  = (intersection - position).normalized();
  // Normal: directed towards ray origin
  N = (N.dot(ray.D) < 0) ? N : -N;

  return Hit(t, N);
}

Cylinder::Cylinder(Point const &pos, Vector const &direction, double radius)
:
    position(pos),
    direction(direction),
    radius(radius)
{}
