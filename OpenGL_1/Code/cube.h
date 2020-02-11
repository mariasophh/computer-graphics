#ifndef CUBE_H
#define CUBE_H
#include "vertex.h"

/* Here we declare cube vertices */

static Vertex vc0 = {-1, -1, 1, 0, 1, 0}; // front bottom left
static Vertex vc1 = {1, -1, 1, 0, 0, 0}; // front bottom right
static Vertex vc2 = {1, 1, 1, 0, 0, 1}; // front upper right
static Vertex vc3 = {-1, 1, 1, 1, 0, 0}; // front upper left

static Vertex vc4 = {-1, -1, -1, 0, 1, 1}; // back bottom left
static Vertex vc5 = {1, -1, -1, 1, 0, 0}; // back bottom right
static Vertex vc6 = {1, 1, -1, 1, 1, 0}; // back upper right
static Vertex vc7 = {-1, 1, -1, 1, 1, 1}; // back upper left

#endif // CUBE_H
