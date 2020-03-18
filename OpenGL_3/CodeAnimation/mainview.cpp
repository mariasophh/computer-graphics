#include "mainview.h"

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

    for(sceneObject obj : sceneObjects) {
        glDeleteTextures(1, &obj.textureName);
    }

    destroyModelBuffers();
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

    QString glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.2F, 0.5F, 0.7F, 0.0F);

    createShaderProgram();

    // initialize the scene objects
    sceneObject cat_diff;
    cat_diff.objectPath = ":/models/cat.obj";
    cat_diff.texturePath = ":/textures/cat_diff.png";
    loadMesh(&cat_diff);
    cat_diff.position = QVector3D(0, 0, -6);
    cat_diff.speed = QVector3D(0, 0.005, 0);
    sceneObjects.push_back(cat_diff);

    sceneObject cat_norm;
    cat_norm.objectPath = ":/models/cat.obj";
    cat_norm.texturePath = ":/textures/cat_norm.png";
    loadMesh(&cat_norm);
    cat_norm.position = QVector3D(0, 0, -6);
    cat_norm.speed = QVector3D(0, -0.005, 0);
    sceneObjects.push_back(cat_norm);

    sceneObject planet;
    planet.objectPath = ":/models/sphere.obj";
    planet.texturePath = ":/textures/earthmap1k.png";
    loadMesh(&planet);
    planet.position = QVector3D(2, 0, -6);
    planet.speed = QVector3D(0, 0, 0);
    sceneObjects.push_back(planet);

    sceneObject ball;
    ball.objectPath = ":/models/sphere.obj";
    ball.texturePath = ":/textures/basketball.png";
    loadMesh(&ball);
    ball.position = QVector3D(-2, 0, -6);
    ball.speed = QVector3D(-0.01, 0, 0);
    sceneObjects.push_back(ball);

    loadTextures();

    // Initialize transformations.
    updateProjectionTransform();
    updateModelTransforms();

    // render 60 frames per second
    timer.start(1000.0 / 60.0);
}

void MainView::createShaderProgram() {
    // Create Phong shader program.
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Get the uniforms for the Phong shader program.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColorPhong          = phongShaderProgram.uniformLocation("lightColor");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");
}

/* The new loadMesh function has a pointer to the scene object
 * as it is called for each object
 */
void MainView::loadMesh(sceneObject *obj) {
    Model model(obj->objectPath);
    model.unitize(1);
    QVector<float> meshData = model.getVNTInterleaved();

    obj->meshSize = model.getVertices().size();

    // Generate VAO
    glGenVertexArrays(1, &obj->meshVAO);
    glBindVertexArray(obj->meshVAO);

    // Generate VBO
    glGenBuffers(1, &obj->meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, obj->meshVBO);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(GL_FLOAT), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::loadTextures() {
    // load the texture for each object
    for(sceneObject &obj : sceneObjects) {
        glGenTextures(1, &obj.textureName);
        loadTexture(obj.texturePath, obj.textureName);
    }
}

void MainView::loadTexture(QString file, GLuint textureName) {
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
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

    // Update the animation
    updateAnimationObjects();

    // Update the model transforms before drawing
    updateModelTransforms();

    // Choose the selected shader.
    switch (currentShader) {
    case NORMAL:
        qDebug() << "Normal shader program not implemented";
        break;
    case GOURAUD:
        qDebug() << "Gouraud shader program not implemented";
        break;
    case PHONG:
        phongShaderProgram.bind();
        updatePhongUniforms();
        break;
    }

    // Set the texture
    glActiveTexture(GL_TEXTURE0);

    // Draw each scene object
    for(sceneObject &obj : sceneObjects) {
        paintObject(obj);
    }

    phongShaderProgram.release();
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

void MainView::updatePhongUniforms() {
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColorPhong, lightColor.x(), lightColor.y(), lightColor.z());

    glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateProjectionTransform() {
    float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60.0F, aspectRatio, 0.2F, 20.0F);
}

void MainView::updateModelTransforms() {
    // update every object
    for(sceneObject &obj : sceneObjects) {
        obj.meshTransform.setToIdentity();
        obj.meshTransform.translate(obj.position);
        // rotate with the sum of the object's rotation and the global rotation
        obj.meshTransform.rotate(obj.rotation.x() + rotation.x(), {1.0F, 0.0F, 0.0F});
        obj.meshTransform.rotate(obj.rotation.y() + rotation.y(), {0.0F, 1.0F, 0.0F});
        obj.meshTransform.rotate(obj.rotation.z() + rotation.z(), {0.0F, 0.0F, 1.0F});

        obj.meshTransform.scale(obj.scale * scale);
        obj.meshNormalTransform = obj.meshTransform.normalMatrix();
    }

    update();
}

/* This function updates the animation of an object */
void MainView::updateAnimationObjects() {
    // animate each object differently

    // CAT_DIFF

    if(sceneObjects[0].position.y() > 3 || sceneObjects[0].position.y() < 0) {
        sceneObjects[0].speed *= -1; // change direction to respect boundaries
    }
    sceneObjects[0].position += sceneObjects[0].speed; //translation with its speed
    sceneObjects[0].rotation += QVector3D(0, 6, 0); // rotate on the y-axis

    // CAT_NORMAL
    if(sceneObjects[1].position.y() < -3 || sceneObjects[1].position.y() > 0) {
        sceneObjects[1].speed *= -1; // change direction to respect boundaries
    }
    sceneObjects[1].position += sceneObjects[1].speed; //translation with its speed
    sceneObjects[1].rotation += QVector3D(10, 0, 0); // rotate on the x-axis

    // PLANET
    sceneObjects[2].rotation += QVector3D(1, 1, 1); // rotate on all the axis
    // increase its size until it reaches a scale of 1.5
    if(sceneObjects[2].scale < 1.5) {
        sceneObjects[2].scale *= 1.001;
    } else if(sceneObjects[3].scale < 1.5){
        // start to also increase the ball until it reaches a scale of 1.5
        sceneObjects[3].scale *= 1.001;
    }

    // BALL
    if(sceneObjects[3].position.x() < -5 || sceneObjects[3].position.x() > -2) {
        sceneObjects[3].speed *= -1; // change direction to respect boundaries
    }
    sceneObjects[3].position += sceneObjects[3].speed; // translation with its speed

}

/* This function paints one object */
void MainView::paintObject(sceneObject obj) {
    // phong uniforms specific to current object
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, obj.meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, obj.meshNormalTransform.data());

    glBindTexture(GL_TEXTURE_2D, obj.textureName);

    glBindVertexArray(obj.meshVAO);
    glDrawArrays(GL_TRIANGLES, 0, obj.meshSize);
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers() {
    for(sceneObject &obj : sceneObjects) {
        glDeleteBuffers(1, &obj.meshVBO);
        glDeleteVertexArrays(1, &obj.meshVAO);
    }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    rotation = { static_cast<float>(rotateX),
                 static_cast<float>(rotateY),
                 static_cast<float>(rotateZ) };

    updateModelTransforms();
}

void MainView::setScale(int newScale) {
    scale = static_cast<float>(newScale) / 100.0F;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    currentShader = shading;
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
