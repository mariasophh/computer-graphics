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

    // Variable used to select the shading
    GLuint shade;
    // Variable to remember the texture
    GLuint texture;

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

    // There are 3 shaders
    QOpenGLShaderProgram shaderProgram[3];
    // Each shader has its own uniforms
    GLint uniformModelViewTransform[3];
    GLint uniformProjectionTransform[3];
    GLint uniformNormalTransform[3];

    // Light, material and color uniforms
    GLint uniformLightPosition[3];
    GLint uniformLightColor[3];
    GLint uniformMaterialColor[3];
    GLint uniformMaterialK[3];

    // Sampler uniform
    GLuint uniformSampler;

    // Light and material values
    GLfloat lightPos[3] = {150, 200, 180}; // position of light
    GLfloat lightCol[3] = {1, 1, 1}; // white light color
    //GLfloat materialCol[3] = {rand()/(float) RAND_MAX, rand()/(float) RAND_MAX, rand()/(float) RAND_MAX}; // material color
    GLfloat materialCol[3] = {1, 1, 1};
    GLfloat materialK[3] = {0.2, 0.3, 0.4}; // material coefficients (Ka, Kd, Ks)

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
