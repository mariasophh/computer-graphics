#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

pair<ObjectPtr, Hit> Scene::castRay(Ray const &ray) const
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    return pair<ObjectPtr, Hit>(obj, min_hit);
}

Color Scene::trace(Ray const &ray, unsigned depth)
{
    pair<ObjectPtr, Hit> mainhit = castRay(ray);
    ObjectPtr obj = mainhit.first;
    Hit min_hit = mainhit.second;

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);

    Material const &material = obj->material;
    Point hit = ray.at(min_hit.t);
    Vector V = -ray.D;

    // Pre-condition: For closed objects, N points outwards.
    Vector N = min_hit.N;

    // The shading normal always points in the direction of the view,
    // as required by the Phong illumination model.
    Vector shadingN;
    if (N.dot(V) >= 0.0)
        shadingN = N;
    else
        shadingN = -N;

    // Account for shadow acne
    Point hit_acne = hit + epsilon * shadingN;

    Color matColor = material.color;

    // Add ambient once, regardless of the number of lights.
    Color color = material.ka * matColor;

    // Add diffuse and specular components.
    for (auto const &light : lights)
    {
        Vector L = (light->position - hit).normalized();

        // Cast shadow ray
        Ray shadow(hit_acne, L);
        pair<ObjectPtr, Hit> shadowHit = castRay(shadow);
        ObjectPtr obj_shadow = shadowHit.first;
        Hit hit_shadow = shadowHit.second;

        // Compute dist. from shadow to light source, used to check if
        // the intersected object is farther than the light
        double distSL = (shadow.O - light->position).length();

        // No intersection was found for shadow ray or the light is closer than
        // the intersection => the object does not have a shadow
        if(!renderShadows || !obj_shadow || (hit_shadow.t > distSL)) {

            // Add diffuse.
            double diffuse = std::max(shadingN.dot(L), 0.0);
            color += diffuse * material.kd * light->color * matColor;

            // Add specular.
            Vector reflectDir = reflect(-L, shadingN);
            double specAngle = std::max(reflectDir.dot(V), 0.0);
            double specular = std::pow(specAngle, material.n);

            color += specular * material.ks * light->color;
        }
    }

    if (depth > 0 and material.isTransparent)
    {
        // The object is transparent, and thus refracts and reflects light.
        // Use Schlick's approximation to determine the ratio between the two.
        // Assumption: the outside material is air, thus ni = 1.0
        double kr_0 = pow((1.0 - material.nt)/(1.0 + material.nt), 2);
        double kr = kr_0 + (1-kr_0) * pow(1 - shadingN.dot(-ray.D), 5);
        double kt = 1 - kr;

        // Reflection ray
        Vector reflectionD = reflect(ray.D, shadingN);
        Ray reflectionRay(hit_acne, reflectionD);
        color += kr * trace(reflectionRay, depth-1);


        // Refraction ray
        Vector refractionD;
        // Avoid intersection by subtracting epsilon
        hit_acne = hit - epsilon * shadingN;
        if (N.dot(V) >= 0) { // N and V go in the same direction => outside object
            refractionD = refract(ray.D, shadingN, 1.0, material.nt);
        } else { // N and V go in opposite directions => inside the object
            refractionD = refract(ray.D, shadingN, material.nt, 1.0);
        }
        Ray refractionRay(hit_acne, refractionD);
        color += kt * trace(refractionRay, depth-1);

    }
    else if (depth > 0 and material.ks > 0.0)
    {
        // The object is not transparent, but opaque.
        Vector reflectionD = reflect(ray.D, shadingN);
        Ray reflectionRay(hit_acne, reflectionD);
        // Recursively trace a new ray in this direction with decresed depth
        color += material.ks * trace(reflectionRay, depth-1);
    }

    return color;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();

    for (unsigned y = 0; y < h; ++y)
        for (unsigned x = 0; x < w; ++x)
        {
            Color color;
            for(int i = 1; i<= supersamplingFactor; i++)
            {
              double c = (double) i / supersamplingFactor;
              Point pixel(x + c, h - 1 - y + c, 0);
              Ray ray(eye, (pixel - eye).normalized());
              color += trace(ray, recursionDepth);
            }

            color /= supersamplingFactor;
            color.clamp();
            img(x, y) = color;
        }
}

// --- Misc functions ----------------------------------------------------------

// Defaults
Scene::Scene()
:
    objects(),
    lights(),
    eye(),
    renderShadows(false),
    recursionDepth(0),
    supersamplingFactor(1)
{}

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}

void Scene::setRenderShadows(bool shadows)
{
    renderShadows = shadows;
}

void Scene::setRecursionDepth(unsigned depth)
{
    recursionDepth = depth;
}

void Scene::setSuperSample(unsigned factor)
{
    supersamplingFactor = factor;
}
