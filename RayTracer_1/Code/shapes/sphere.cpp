#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    Vector OC = (ray.O - position);

    // Quadratic equation terms
    double a, b, c, discr, t1, t2, t;

    a = ray.D.dot(ray.D);
    b = 2*(ray.D.dot(OC));
    c = OC.dot(OC) - r * r;

    // Compute the discriminant, check if it's negative
    discr = b*b - 4*a*c;

    if (discr < 0) {
        // Square root is imaginary => no intersection
        return Hit::NO_HIT();
    }
    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    // One or two solutions, compute t
    t1 = (-b + sqrt(discr)) / (2*a);
    t2 = (-b - sqrt(discr)) / (2*a);

    // Interection occurs when the solution is positive
    // In case of two solutions, we take the least positive one
    if (t1 >= 0 && t2 >= 0) {
        t = fmin(t1,t2);
    } else if (t1 < 0 && t2 >= 0) {
        t = t2;
    } else if (t2 < 0 && t1 >= 0) {
        t = t1;
    } else {
        return Hit::NO_HIT();
    }

    Point intersection = ray.at(t);
    Vector N  = (intersection - position).normalized();
    // Normal: directed towards ray origin
    N = (N.dot(ray.D) < 0) ? N : -N;

    return Hit(t, N);

}

Sphere::Sphere(Point const &pos, double radius)
:
position(pos),
r(radius)
{}
