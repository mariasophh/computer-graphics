#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex
{
    QVector3D coords;
    QVector3D normals;

    Vertex(QVector3D coords, QVector3D normals)
        :
          coords(coords),
          normals(normals)
    {}
};

#endif // VERTEX_H

