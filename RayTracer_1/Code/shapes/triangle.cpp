#include "triangle.h"

#include <cmath>

Hit Triangle::intersect(Ray const &ray)
{
  // dot product between N and ray's direction
  double dotProdND = N.dot(ray.D);

  // if the plane and the ray are parallel (i.e. dotProdND -> 0) return no hit
  if(fabs(dotProdND) < std::numeric_limits<double>::epsilon())
  {
    return Hit::NO_HIT();
  }

  // plane equation Ax + By + Cz + D = 0 where N = (A, B, C)
  double D = N.dot(v1);
  // substitute P = ray.O + t * ray.D in plane equation above
  // compute t
  double t =  (N.dot(ray.O) - D) / N.dot(ray.D);

  // triangle behind the ray (no hit)
  if(t < 0) return Hit::NO_HIT();

  // compute P
  Vector P = ray.O + t * ray.D;

  // check P's position wrt triangle
  Vector v0p, v1p, v2p, F;
  // edge 1 : v0v1
  v0p = P - v0;
  F = x_edge.cross(v0p);
  if(N.dot(F) < 0) return Hit::NO_HIT();

  // edge 2: v2v0
  v2p = P - v2;
  F = y_edge.cross(v2p);
  if(N.dot(F) < 0) return Hit::NO_HIT();

  // edge 3: v1v2
  v1p = P - v1;
  F = z_edge.cross(v1p);
  if(N.dot(F) < 0) return Hit::NO_HIT();

  // direct the normal towards ray's origin
  N = (N.dot(ray.D) < 0) ? N : -N;

  return Hit(t, N);
}

Triangle::Triangle(Point const &v0,
                   Point const &v1,
                   Point const &v2)
:
    v0(v0),
    v1(v1),
    v2(v2),
    N(),
    x_edge(),
    y_edge(),
    z_edge()
{
  x_edge = v1-v0; // edge along x-axis
  y_edge = v0-v2; // edge along y-axis
  z_edge = v2-v1; // edge along z-axis
  // normalized normal vector: parallel to z-axis
  N = ((v1-v0).cross(v2-v0)).normalized();
}
