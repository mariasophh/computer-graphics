#include "mainview.h"
#include "cube.h"
#include "pyramid.h"

#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));

    // initialise QMatrix4x4 data-members for the cube pyramid and sphere
    // model transformation
    cubeMatrix = QMatrix4x4();
    pyramidMatrix = QMatrix4x4();
    sphereMatrix = QMatrix4x4();

    // set values for objects' translation
    cubeMatrix.translate(2, 0, -6);
    pyramidMatrix.translate(-2, 0, -6);
    sphereMatrix.translate(0, 0, -10);

    // initialise QMatrix4x34 data-member
    // projection transformation
    projectionTransf = QMatrix4x4();
    projectionTransf.perspective(60, 1, 1, 100);
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    qDebug() << "MainView destructor";

    // cleaning up the allocations
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // cleaning up the vbo and vao arrays
    glDeleteBuffers(3, vbo);
    glDeleteVertexArrays(3, vao);

    makeCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage)),
            this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color to be used by glClear. This is, effectively, the background color.
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    // initialise vbo, vao and ibo as arrays (to fit more objects)
    glGenBuffers(3, vbo);
    glGenVertexArrays(3, vao);
    glGenBuffers(2, ibo);

    // CUBE

    // bind cube array and buffer
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    // bind indices array buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);

    // load cube (vertices + indices) data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // cube coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // cube colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3*sizeof(float)));

    // PYRAMID

    // bind pyramid array and buffer
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    // bind indices array buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[1]);

    // load pyramid (vertices + indices) data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    // pyramid coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // pyramid colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3*sizeof(float)));

    // SPHERE

    // load sphere from model
    Model sphereModel = Model(":/models/sphere.obj");
    // unitize model upon loading
    sphereModel.unitize();
    // retrieve scaled vertices
    QVector<QVector3D> sphereVertices = sphereModel.getVertices();

    // populate the sphere array with the vertices' coordinates of the sphere
    sizeSphere = sphereVertices.size();
    Vertex sphere[sizeSphere];
    QVector<QVector3D>::iterator it = sphereVertices.begin();
    while( it != sphereVertices.end()) {
        // assign random colours for the current vertex
        float r = rand() / (float) RAND_MAX;
        float g = rand() / (float) RAND_MAX;
        float b = rand() / (float) RAND_MAX;

        int counter = std::distance(sphereVertices.begin(), it);
        sphere[counter] = {(*it).x(), (*it).y(), (*it).z(), r, g, b};

        ++it;
    }

    // bind sphere array and buffer
    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

    // load sphere data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);

    // sphere coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // sphere colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3*sizeof(float)));

}

void MainView::createShaderProgram() {
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    // Extract the locations of the uniforms
    modelLocation = shaderProgram.uniformLocation("modelTransform");
    projectionLocation = shaderProgram.uniformLocation("projectionTransform");
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (GLfloat*) projectionTransf.data());

    // Draw cube
    glBindVertexArray(vao[0]);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*) cubeMatrix.data());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (GLfloat*)0);

    // Draw pyramid
    glBindVertexArray(vao[1]);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*) pyramidMatrix.data());
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (GLfloat*)0);

    // Draw sphere
    glBindVertexArray(vao[2]);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*) sphereMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, sizeSphere);

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    // Update projection to fit the new aspect ratio
    float newRatio = (float) newWidth / (float) newHeight;
    // Update the values of the projection transformation
    projectionTransf.setToIdentity(); // reset
    projectionTransf.perspective(60, newRatio, 1, 100); // set projection to new ratio
    // Update the screen after changes
    update();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";

    // update x,y,z coordinates (that are "globally" seen)
    // note that only one value actually changes at a time (rotation wrt one axis)
    x = rotateX;
    y = rotateY;
    z = rotateZ;

    // translate and scale objects
    transScaleObjects();

    // rotate objects
    rotateObjects();
    // update the screen after changes
    update();
}

void MainView::setScale(int newScale) {
    qDebug() << "Scale changed to " << newScale;

    // update scale (that is "globally" seen)
    // (using percetages)
    scale = (float) newScale / 100.0f;

    // translate and scale objects
    transScaleObjects();

    // rotate objects
    rotateObjects();
    // update the screen after changes
    update();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/* This function rotates all the objects using the "global" scale value
 * Note that on scaling the value is updated */
void MainView::transScaleObjects() {
    // cube
    cubeMatrix.setToIdentity(); // reset
    cubeMatrix.translate(2, 0, -6); // initial position
    cubeMatrix.scale(scale); // set global scale

    // pyramid
    pyramidMatrix.setToIdentity(); // reset
    pyramidMatrix.translate(-2, 0, -6); // initial position
    pyramidMatrix.scale(scale); // set global scale

    // sphere
    sphereMatrix.setToIdentity(); // reset
    sphereMatrix.translate(0, 0, -10); // initial position
    sphereMatrix.scale(scale); // set global scale
}

/* This function rotates all the objects*/
void MainView::rotateObjects() {
    rotation(&cubeMatrix); // rotate cube
    rotation(&pyramidMatrix); // rotate pyramid
    rotation(&sphereMatrix); // rotate sphere
}

/* This function rotates the passed matrix using the "global" x, y, z coordinates
 * Note that on rotation these coordinates are updated */
void MainView::rotation(QMatrix4x4 *matrix) {
    (*matrix).rotate(x, 1, 0, 0); // x-axis
    (*matrix).rotate(y, 0, 1, 0); // y-axis
    (*matrix).rotate(z, 0, 0, 1); // z-axis
}

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
