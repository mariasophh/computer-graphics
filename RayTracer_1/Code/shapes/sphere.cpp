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

    // Placeholder for actual intersection calculation.
    Vector OC = (position - ray.O).normalized();
    
    // Quadratic equation terms
    double a, b, c, discr, t1, t2, t;
    
    a = ray.dot(ray);
    b = 2*(ray.dot(OC));
    c = OC - radius * radius;
    
    // Compute the discriminant, check if it's negative
    discr = b*b - 4*a*c;
    
    if (discr < 0) {
        // Square root is imaginary => no intersection
        return Hit::NO_HIT();
    } else {
        // One or two solutions, compute t
        t1 = fabs((-b + sqrt(discr)) / 2*a);
        t2 = fabs((-b - sqrt(discr)) / 2*a);
        
        t = fmin(t1,t2);
    }
    
//    if (OC.dot(ray.D) < 0.999) {
//        return Hit::NO_HIT();
    }
//    double t = 1000;

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Vector N /* = ... */;

    return Hit(t, N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
