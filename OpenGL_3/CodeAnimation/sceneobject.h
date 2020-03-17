#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QVector3D>
#include <QMatrix2x3>
#include <QMatrix4x4>
#include <gltypes.h>

struct sceneObject {
    // Buffers
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;

    // Texture ptr
    GLuint textureName;

    // Transforms
    float scale = 1.0F;
    QVector3D rotation;
    QVector3D position;
    QVector3D speed;
    QMatrix4x4 meshTransform;
    QMatrix3x3 meshNormalTransform;

    // Utils
    QString texturePath;
    QString objectPath;
};

#endif // SCENEOBJECT_H
