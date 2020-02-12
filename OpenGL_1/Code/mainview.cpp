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

    // initialise QMatrix4x4 data-members for the cube and pyramid
    // model transformation
    cubeMatrix = QMatrix4x4();
    pyramidMatrix = QMatrix4x4();

    // set values for objects' translation
    cubeMatrix.translate(2, 0, -6);
    pyramidMatrix.translate(-2, 0, -6);

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
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(2, vao);

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

    // initialise vbo and vao as arrays (to fit more objects)
    glGenBuffers(2, vbo);
    glGenVertexArrays(2, vao);

    // cube
    Vertex cube[36];
    initialiseCube(cube);

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3*sizeof(float)));

    // pyramid
    Vertex pyramid[18];
    initialisePyramid(pyramid);

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

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
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw pyramid
    glBindVertexArray(vao[1]);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*) pyramidMatrix.data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

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
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    Q_UNIMPLEMENTED();
}

void MainView::setScale(int scale) {
    qDebug() << "Scale changed to " << scale;
    Q_UNIMPLEMENTED();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/* This function initialises all the faces of the cube, given the vertices*/
void MainView::initialiseCube(Vertex cube[36]) {
    // front face
    cube[0] = vc0;
    cube[1] = vc1;
    cube[2] = vc3;
    cube[3] = vc1;
    cube[4] = vc2;
    cube[5] = vc3;

    // bottom face
    cube[6] = vc0;
    cube[7] = vc4;
    cube[8] = vc5;
    cube[9] = vc5;
    cube[10] = vc1;
    cube[11] = vc0;

    // back face
    cube[12] = vc7;
    cube[13] = vc6;
    cube[14] = vc5;
    cube[15] = vc5;
    cube[16] = vc4;
    cube[17] = vc7;

    // upper face
    cube[18] = vc7;
    cube[19] = vc3;
    cube[20] = vc2;
    cube[21] = vc6;
    cube[22] = vc7;
    cube[23] = vc2;

    // left face
    cube[24] = vc7;
    cube[25] = vc4;
    cube[26] = vc0;
    cube[27] = vc0;
    cube[28] = vc3;
    cube[29] = vc7;

    // right face
    cube[30] = vc6;
    cube[31] = vc2;
    cube[32] = vc1;
    cube[33] = vc1;
    cube[34] = vc5;
    cube[35] = vc6;
}

/* This function initialises all the faces of the pyramid, given the vertices*/
void MainView::initialisePyramid(Vertex pyramid[18]) {
    // front face
    pyramid[0] = pv0;
    pyramid[1] = pv1;
    pyramid[2] = pv2;

    // back face
    pyramid[6] = pv0;
    pyramid[7] = pv3;
    pyramid[8] = pv4;

    // left face
    pyramid[9] = pv0;
    pyramid[10] = pv4;
    pyramid[11] = pv1;

    // right face
    pyramid[3] = pv0;
    pyramid[4] = pv2;
    pyramid[5] = pv3;

    // pyramid base
    pyramid[12] = pv1;
    pyramid[13] = pv4;
    pyramid[14] = pv2;
    pyramid[15] = pv4;
    pyramid[16] = pv3;
    pyramid[17] = pv2;
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
