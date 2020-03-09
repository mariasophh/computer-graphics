#include "quad.h"
#include "triangle.h"

#include <limits>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{
  // Replace the return of a NO_HIT by determining the intersection based
  // on the ray and this class's data members.
  Hit min_hit(numeric_limits<double>::infinity(), Vector());
  ObjectPtr obj = nullptr;
  for (unsigned idx = 0; idx != quad_triangles.size(); ++idx) {
      Hit hit(quad_triangles[idx]->intersect(ray));
      if (hit.t < min_hit.t)
      {
          min_hit = hit;
          obj = quad_triangles[idx];
      }
  }

  // No hit? Return background color.
  if (!obj) return Hit::NO_HIT();
  return Hit(min_hit);
}

Quad::Quad(Point const &v0,
           Point const &v1,
           Point const &v2,
           Point const &v3)
{
  // a quad is formed of 2 triangles
  quad_triangles.reserve(2);
  quad_triangles.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
  quad_triangles.push_back(ObjectPtr(new Triangle(v2, v3, v0)));
}
