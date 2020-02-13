#ifndef PYRAMID_H
#define PYRAMID_H
#include "vertex.h"
#include "mainview.h"

/* Here we declare pyramid vertices */
static Vertex pyramidVertices[] = {
    {0, 1, 0, 0, 1, 1}, // apex
    {-1, -1, 1, 0, 0, 1}, // bottom left
    {1, -1, 1, 0, 1, 0}, // bottom right
    {1, -1, -1, 0, 1, 1}, // upper right
    {-1, -1, -1, 0, 1, 1} // upper left
};

/* Here we declare pyramid indices */
static GLubyte pyramidIndices[] = {
    0, 1, 2, // front face
    0, 3, 4, // back face
    0, 4, 1, // left face
    0, 2, 3, // right face
    1, 4, 2, // left base triangle
    4, 3, 2 // right base triangle
};

#endif // PYRAMID_H
