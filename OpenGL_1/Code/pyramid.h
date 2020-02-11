#ifndef PYRAMID_H
#define PYRAMID_H
#include "vertex.h"

/* Here we declare pyramid vertices */

static Vertex pv0 = {0, 1, 0, 0, 1, 1}; // apex
static Vertex pv1 = {-1, -1, 1, 0, 1, 1}; // bottom left
static Vertex pv2 = {1, -1, 1, 0, 1, 1}; // bottom right
static Vertex pv3 = {1, -1, -1, 0, 1, 1}; // upper right
static Vertex pv4 = {-1, -1, -1, 0, 1, 1}; // upper left

#endif // PYRAMID_H
