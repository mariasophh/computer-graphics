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
    double a, b, c, discr, t1, t2;
    
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

    Vector N /* = ... */;

    // One or two solutions, compute t
    t1 = (-b + sqrt(discr)) / (2*a);
    t2 = (-b - sqrt(discr)) / (2*a);
    
    if (t1 >=0 && t2 >= 0) {
        return Hit(fmin(t1, t2), N);
    } else if (t1 < 0 && t2 >= 0) {
        return Hit(t2, N);
    } else if (t2 < 0 && t1 >= 0) {
        return Hit(t1, N);
    } else {
        return Hit::NO_HIT();
    }
    
    
//    if (t1 <= t2 && t1 >= 0) {
//        return Hit(t1, N);
//    } else if (t2 < t1 && t2 >= 0) {
//        return Hit(t2, N);
//    } else {
//        return Hit::NO_HIT();
//    }
//
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
