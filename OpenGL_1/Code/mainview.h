#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "vertex.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
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
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram shaderProgram;

    void createShaderProgram();
    void initialiseCube(Vertex *cube);
    void initialisePyramid(Vertex *pyramid);
    void rotateObjects();
    void rotation(QMatrix4x4 *matrix);
    void transScaleObjects();

    // Vertex array obj, vertex buffer obj and index buffer obj
    GLuint vao[3], vbo[3], ibo[2];
    // Transformation matrices
    QMatrix4x4 cubeMatrix, pyramidMatrix, projectionTransf, sphereMatrix;
    // Locations of the model and projection uniforms
    GLint modelLocation, projectionLocation;
    // Declare the initial rotation values (at origin)
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    // Declare the initial scale
    float scale = 1.0f;
    // Declare the number of sphere vertices
    int sizeSphere;
};

#endif // MAINVIEW_H
