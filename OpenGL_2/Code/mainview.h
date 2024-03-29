#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <QMatrix4x4>
#include <memory>


class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    enum ShadingMode : GLuint {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);
    QVector<quint8> imageToBytes(QImage image);

    // Different shades
    GLuint shade;
    // Texture ptr
    GLuint texture;
    GLuint samplerUniform;

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged(QOpenGLDebugMessage Message);

private:
    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    // Three shaders, each with its own uniforms
    QOpenGLShaderProgram shaderPrograms[3];
    GLint uniformModelViewTransforms[3];
    GLint uniformProjectionTransforms[3];
    GLint uniformNormalTransforms[3];

    // light position, material and color uniforms
    GLint uniformLightColors[3];
    GLint uniformLightPositions[3];
    GLint uniformMaterialColors[3];
    GLint uniformMaterialKs[3];

    // Gouraud + Phong model constants

    // light position
    GLfloat lightPosition[3] = {150, 200, 180};
    // random color of the material
    GLfloat materialColor[3] = {rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX};
    // material kA, kD, kS values
    GLfloat materialKs[3] = {0.2, 0.3, 0.4};
    // white light color
    GLfloat lightColor[3] = {1, 1, 1};

    // Mesh values
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshSize;
    QMatrix4x4 meshTransform;

    // Transforms
    float scale = 1.f;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;

    void createShaderProgram();
    void loadMesh();
    void loadTexture(QString file, GLuint texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();
};

#endif // MAINVIEW_H
