#ifndef CUBE_H
#define CUBE_H
#include "vertex.h"
#include "mainview.h"

/* Here we declare cube vertices */
static Vertex cubeVertices[] = {
    {-1, -1, 1, 0, 1, 0}, // front bottom left
    {1, -1, 1, 0, 0, 0}, // front bottom right
    {1, 1, 1, 0, 0, 1}, // front upper right
    {-1, 1, 1, 1, 0, 0}, // front upper left
    {-1, -1, -1, 0, 1, 1}, // back bottom left
    {1, -1, -1, 1, 0, 0}, // back bottom right
    {1, 1, -1, 1, 1, 0}, // back upper right
    {-1, 1, -1, 1, 1, 1} // back upper left
};

/* Here we declare cube indices */
static GLubyte cubeIndices[] = {
    0, 1, 3, 1, 2, 3, // front face
    0, 4, 5, 5, 1, 0, // bottom face
    7, 6, 5, 5, 4, 7, // back face
    7, 3, 2, 6, 7, 2, // upper face
    7, 4, 0, 0, 3, 7, // left face
    6, 2, 1, 1, 5, 6 // right face
};

#endif // CUBE_H


