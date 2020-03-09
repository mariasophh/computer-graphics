#include "mesh.h"

#include "../objloader.h"
#include "../vertex.h"
#include "triangle.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

Hit Mesh::intersect(Ray const &ray)
{
    // Replace the return of a NO_HIT by determining the intersection based
    // on the ray and this class's data members.
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != d_tris.size(); ++idx) {
        Hit hit(d_tris[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = d_tris[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Hit::NO_HIT();
    return Hit(min_hit);
}

Mesh::Mesh(string const &filename, Point const &position, Vector const &rotation, Vector const &scale)
{
    OBJLoader model(filename);
    d_tris.reserve(model.numTriangles());
    vector<Vertex> vertices = model.vertex_data();
    for (size_t tri = 0; tri != model.numTriangles(); ++tri)
    {
        Vertex one = vertices[tri * 3];
        Point v0(one.x, one.y, one.z);

        Vertex two = vertices[tri * 3 + 1];
        Point v1(two.x, two.y, two.z);

        Vertex three = vertices[tri * 3 + 2];
        Point v2(three.x, three.y, three.z);

        // Apply non-uniform scaling, rotation and translation to the three points
        // of the triangle (v0, v1, and v2) here.

        // Non-uniform scaling
        v0 = v0 * scale;
        v1 = v1 * scale;
        v2 = v2 * scale;

        // Rotation
        // Rotation around the x axis

        double cosine = cos(rotation.x);
        double sine = sin(rotation.x);
        v0 = Point(v0.x, v0.y*cosine - v0.z*sine, v0.y*sine + v0.z*cosine);
        v1 = Point(v1.x, v1.y*cosine - v1.z*sine, v1.y*sine + v1.z*cosine);
        v2 = Point(v2.x, v2.y*cosine - v2.z*sine, v2.y*sine + v2.z*cosine);

        // Rotation around y axis
        cosine = cos(rotation.y);
        sine = sin(rotation.y);
        v0 = Point(v0.x*cosine + v0.z*sine, v0.y, -v0.x*sine + v0.z*cosine);
        v1 = Point(v1.x*cosine + v1.z*sine, v1.y, -v1.x*sine + v1.z*cosine);
        v2 = Point(v2.x*cosine + v2.z*sine, v2.y, -v2.x*sine + v2.z*cosine);

        // Rotation around z axis
        cosine = cos(rotation.z);
        sine = sin(rotation.z);
        v0 = Point(v0.x*cosine - v0.y*sine, v0.x*sine + v0.y*cosine, v0.z);
        v1 = Point(v1.x*cosine - v1.y*sine, v1.x*sine + v1.y*cosine, v1.z);
        v2 = Point(v2.x*cosine - v2.y*sine, v2.x*sine + v2.y*cosine, v2.z);

        // Translation
        v0 = (v0 + position);
        v1 = (v1 + position);
        v2 = (v2 + position);

        d_tris.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
    }

    cout << "Loaded model: " << filename << " with " <<
        model.numTriangles() << " triangles.\n";
}
