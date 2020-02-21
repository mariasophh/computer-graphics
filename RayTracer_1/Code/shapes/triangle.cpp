#include "triangle.h"

#include <cmath>

Hit Triangle::intersect(Ray const &ray)
{
    /*
    // If the dot product of N and D is 0, then the ray is parallel to
    // the plane => no intersection
    if (N.dot(ray.D) == 0) return Hit::NO_HIT();

    double t = (N.dot(v0) - N.dot(ray.O)) / N.dot(ray.D);

    // Plane lies behind ray origin
    if (t < 0) return Hit::NO_HIT();

    // Ray and plane intersection point (p)
    Point p = (ray.O + t*ray.D).normalized();

    // Check if intersection point is inside the triangle
    // p and v0 on same side of v1v2, p and v1 on same side of v0v2, p and v2 on same side of v0v1
    Triple cross1 = p.cross(v0).normalized();
    Triple cross2 = v1.cross(v2).normalized();

    Triple cross3 = p.cross(v1).normalized();
    Triple cross4 = v0.cross(v2).normalized();

    Triple cross5 = p.cross(v2).normalized();
    Triple cross6 = v0.cross(v1).normalized();

    if (cross1.dot(cross2) > 0 && cross3.dot(cross4) >0 && cross5.dot(cross6) >0) {
        return Hit(t,N);
    } else {
        return Hit::NO_HIT();
    }

    // Normal: directed towards ray origin
    N = (N.dot(ray.D) < 0) ? N : -N;
    */


    // Check if intersection point is within triangle: A = A1 + A2 + A3
    // Area of triangle = half of parallelogram's area (magnitude of cross product)
    // A = 1/2*|v0v1 x v0v2|
    /*double area = fabs((v1-v0).cross(v2-v0)) / 2;
    double area_1 = fabs((v1-intersection).cross(v2-intersection)) / 2;
    double area_2 = fabs((v1-intersection).cross(v0-intersection)) / 2;
    double area_3 = fabs((v2-intersection).cross(v0-intersection)) / 2;

    if (area == area_1 + area_2 + area_3) {
        return Hit(t,N);
    } else {
        return Hit::NO_HIT();
    }*/

    // If the dot product of N and D is close to 0, then the ray is parallel to
    // the plane => no intersection
    double eps = std::numeric_limits<double>::epsilon();
    if (fabs(N.dot(ray.D)) < eps) return Hit::NO_HIT();

    // Implementation of Möller-Trumbore algorithm
    double t, u, v;

    // Edges of the trianlge (with vertices v0, v1, v2)
    Triple edge1 = v1-v0;
    Triple edge2 = v2-v0;

    // Corss product between D and edge 2
    Vector h = ray.D.cross(edge2);
    // Calculate determinant
    double det = (h).dot(edge1);
    // Inverse of determinant
    double inv_det = 1 / det;
    //Check if determinant is close to 0 =>> ray is parallel to triangle
    if ( det > -eps && det < eps) return Hit::NO_HIT();

    // Vector from first vertex of triangle to origin of ray: s
    Triple s = ray.O - v0;
    Triple q = s.cross(edge1);

    // Calculate u, v and t
    u = s.dot(h) * inv_det;
    if (u < 0.0 || u > 1.0) return Hit::NO_HIT();

    v = ray.D.dot(q) * inv_det;
    if (v < 0.0 || v + u > 1.0) return Hit::NO_HIT();

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
