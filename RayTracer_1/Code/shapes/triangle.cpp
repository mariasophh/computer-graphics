#include "triangle.h"

#include <cmath>

Hit Triangle::intersect(Ray const &ray)
{    // Implementation of Möller-Trumbore algorithm
    double eps = std::numeric_limits<double>::epsilon();
    double t, u, v;

    // Edges of the trianlge (with vertices v0, v1, v2)
    Triple edge1 = v1-v0;
    Triple edge2 = v2-v0;

    // Corss product between vector direction D and edge 2
    Vector h = ray.D.cross(edge2);
    // Calculate determinant
    double det = (h).dot(edge1);
    // Inverse of determinant
    double inv_det = 1 / det;
    //Check if determinant is close to 0 =>> ray is parallel to triangle
    if ( det > -eps && det < eps) return Hit::NO_HIT();

    // Vector from first vertex of triangle to origin of ray: s
    Triple s = ray.O - v0;
    // Cross product between triangle's first edge and s: q
    Triple q = s.cross(edge1);

    // Calculate u, v and t
    u = s.dot(h) * inv_det;
    if (u < 0.0 || u > 1.0) return Hit::NO_HIT();

    v = ray.D.dot(q) * inv_det;
    if (v < 0.0 || v + u > 1.0) return Hit::NO_HIT();

    // Calculate intersection point
    t = edge2.dot(q) * inv_det;

    // Normal: directed towards ray origin
    N = (N.dot(ray.D) < 0) ? N : -N;

    if (t > eps) {
            return Hit(t,N);
    } else {
        return Hit::NO_HIT();
    }
}

Triangle::Triangle(Point const &v0,
                   Point const &v1,
                   Point const &v2)
:
    v0(v0),
    v1(v1),
    v2(v2),
    N()
{
    // Calculate the surface normal here and store it in the N,
    // which is declared in the header. It can then be used in the intersect function.

    // Components of normal lie in the normal plane, can be calc. by doing the
    // cross product of two vectors which span the plane
    N = (v1-v0).cross(v2-v0).normalized();
}
