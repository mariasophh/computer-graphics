#include "mainview.h"
#include "model.h"
#include "vertex.h"

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

    makeCurrent();

    destroyModelBuffers();

    glDeleteTextures(1, &texture);
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

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
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

    glGenTextures(1, &texture);

    loadMesh();
    loadTexture(":/textures/cat_diff.png", texture);

    // Initialize transformations
    updateProjectionTransform();
    updateModelTransforms();
}

void MainView::createShaderProgram() {
    // Create Normal shader program
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_normal.glsl");
    shaderProgram[NORMAL].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_normal.glsl");
    shaderProgram[NORMAL].link();

    // Create Phong shader program
    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    shaderProgram[PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    shaderProgram[PHONG].link();

    // Create Gouraud shader program
    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_gouraud.glsl");
    shaderProgram[GOURAUD].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_gouraud.glsl");
    shaderProgram[GOURAUD].link();

    // Get the uniforms for all 3 shaders
    for (int i=0; i<3; i++) {
        uniformModelViewTransform[i] = shaderProgram[i].uniformLocation("modelViewTransform");
        uniformProjectionTransform[i] = shaderProgram[i].uniformLocation("projectionTransform");
        uniformNormalTransform[i] = shaderProgram[i].uniformLocation("normalTransform");

        uniformLightPosition[i] = shaderProgram[i].uniformLocation("lightPos");
        uniformLightColor[i] = shaderProgram[i].uniformLocation("lightCol");
        uniformMaterialColor[i] = shaderProgram[i].uniformLocation("materialCol");
        uniformMaterialK[i] = shaderProgram[i].uniformLocation("materialK");
    }

    uniformSampler = shaderProgram[PHONG].uniformLocation("samplerUniform");
    uniformSampler = shaderProgram[GOURAUD].uniformLocation("samplerUniform");
}

void MainView::loadMesh() {
    Model model(":/models/cat.obj");
    // unitize model upon retrieval to fit a cube with side length 2
    model.unitize(2);

    QVector<QVector3D> vertexCoords = model.getVertices();

    // retrieve the normals of the object
    QVector<QVector3D> vertexNormals = model.getNormals();

    // retrieve the texture coords of the object
    QVector<QVector2D> vertexTextureCoords = model.getTextureCoords();

    // store the size
    meshSize = vertexCoords.size();

    QVector<float> meshData;
    meshData.reserve(8 * vertexCoords.size());

    for (GLuint i=0; i<meshSize; i++)
    {
        meshData.append(vertexCoords[i].x());
        meshData.append(vertexCoords[i].y());
        meshData.append(vertexCoords[i].z());
        meshData.append(vertexNormals[i].x());
        meshData.append(vertexNormals[i].y());
        meshData.append(vertexNormals[i].z());
        meshData.append(vertexTextureCoords[i].x());
        meshData.append(vertexTextureCoords[i].y());
    }

    meshSize = vertexCoords.size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);

    // Generate VBO
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Function used to bind and push data to texture
void MainView::loadTexture(QString file, GLuint texturePtr) {
    glBindTexture(GL_TEXTURE_2D, texturePtr);

    // load image
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // anisotropic filtering
    GLfloat f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT, f);

    // Upload the image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());

    glGenerateMipmap(GL_TEXTURE_2D);
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

    shaderProgram[shade].bind();

    glUniformMatrix4fv(uniformProjectionTransform[shade], 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransform[shade], 1, GL_FALSE, meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransform[shade], 1, GL_FALSE, meshTransform.normalMatrix().data());

    glUniform3fv(uniformLightPosition[shade], 1, lightPos);
    glUniform3fv(uniformLightColor[shade], 1, lightCol);
    glUniform3fv(uniformMaterialColor[shade], 1, materialCol);
    glUniform3fv(uniformMaterialK[shade], 1, materialK);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // PHONG OR GOURAUD SHADER
    if(shade != 1) {
         glUniform1i(uniformSampler, 0);
    }

    glBindVertexArray(meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, meshSize);

    shaderProgram[shade].release();
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
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}

void MainView::updateProjectionTransform() {
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60, aspect_ratio, 0.2, 20);
}

void MainView::updateModelTransforms() {
    meshTransform.setToIdentity();
    meshTransform.translate(0, 0, -10);
    meshTransform.scale(scale);
    meshTransform.rotate(QQuaternion::fromEulerAngles(rotation));

    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers() {
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &meshVAO);
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    rotation = { static_cast<float>(rotateX), static_cast<float>(rotateY), static_cast<float>(rotateZ) };
    updateModelTransforms();
}

void MainView::setScale(int newScale) {
    scale = static_cast<float>(newScale) / 100.f;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    shade = shading;
}

// --- Private helpers

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

