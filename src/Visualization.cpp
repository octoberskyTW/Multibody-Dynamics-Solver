#include "Visualization.hpp"

#ifdef ENABLE_VISUALIZATION

    #include <cmath>
    #include <iostream>

    #ifndef M_PI
        #define M_PI 3.14159265358979323846
    #endif

    #ifdef __APPLE__
        #include <OpenGL/gl.h>
        #include <OpenGL/glu.h>
    #else
        #include <GL/gl.h>
        #include <GL/glu.h>
    #endif

Visualization::Visualization(int w, int h)
    : window(nullptr),
      width(w),
      height(h),
      cameraX(0.0f),
      cameraY(0.0f),
      cameraZ(10.0f),
      cameraYaw(-90.0f),
      cameraPitch(0.0f),
      cameraDistance(10.0f),
      cameraSpeed(0.1f),
      mouseSensitivity(0.1f),
      targetX(0.0f),
      targetY(0.0f),
      targetZ(0.0f),
      firstMouse(true),
      lastMouseX(w / 2.0),
      lastMouseY(h / 2.0) {}

Visualization::~Visualization() {
    cleanup();
}

bool Visualization::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(width, height, "Multibody Dynamics Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Set callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        Visualization* viz = static_cast<Visualization*>(glfwGetWindowUserPointer(window));
        viz->mouseCallback(xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        Visualization* viz = static_cast<Visualization*>(glfwGetWindowUserPointer(window));
        viz->scrollCallback(xoffset, yoffset);
    });

    // Capture mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    setupLighting();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    return true;
}

void Visualization::cleanup() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool Visualization::shouldClose() {
    return window ? glfwWindowShouldClose(window) : true;
}

void Visualization::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateView();
}

void Visualization::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Visualization::processInput() {
    if (!window)
        return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Movement controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        targetZ += cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        targetZ -= cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        targetX -= cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        targetX += cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        targetY += cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        targetY -= cameraSpeed;
    }

    // Distance controls
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        cameraDistance -= 0.1f;
        if (cameraDistance < 1.0f)
            cameraDistance = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        cameraDistance += 0.1f;
        if (cameraDistance > 50.0f)
            cameraDistance = 50.0f;
    }

    // Reset view
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        cameraYaw = -90.0f;
        cameraPitch = 0.0f;
        cameraDistance = 10.0f;
        targetX = targetY = targetZ = 0.0f;
    }

    updateCamera();
}

void Visualization::updateView() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0);
}

void Visualization::updateCamera() {
    // Convert spherical coordinates to Cartesian
    float radYaw = cameraYaw * M_PI / 180.0f;
    float radPitch = cameraPitch * M_PI / 180.0f;

    cameraX = targetX + cameraDistance * cos(radPitch) * cos(radYaw);
    cameraY = targetY + cameraDistance * sin(radPitch);
    cameraZ = targetZ + cameraDistance * cos(radPitch) * sin(radYaw);
}

void Visualization::setupLighting() {
    GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void Visualization::drawSphere(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat material_diffuse[] = {0.8f, 0.2f, 0.2f, 1.0f};
    GLfloat material_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, radius, 20, 20);
    gluDeleteQuadric(quad);

    glPopMatrix();
}

void Visualization::drawLine(float x1, float y1, float z1, float x2, float y2, float z2) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();

    glEnable(GL_LIGHTING);
}

void Visualization::drawBody(const arma::vec& position, const arma::vec& rotation, double mass) {
    float radius = 0.1f + mass * 0.05f;
    drawSphere(position(0), position(1), position(2), radius);
}

void Visualization::drawJoint(const arma::vec& pos1, const arma::vec& pos2) {
    drawLine(pos1(0), pos1(1), pos1(2), pos2(0), pos2(1), pos2(2));
}

void Visualization::drawChain(const std::vector<arma::vec>& positions) {
    for (size_t i = 0; i < positions.size(); ++i) {
        drawSphere(positions[i](0), positions[i](1), positions[i](2), 0.1f);

        if (i > 0) {
            drawLine(positions[i - 1](0), positions[i - 1](1), positions[i - 1](2), positions[i](0),
                     positions[i](1), positions[i](2));
        }
    }
}

void Visualization::setCamera(float x, float y, float z) {
    cameraX = x;
    cameraY = y;
    cameraZ = z;
}

void Visualization::mouseCallback(double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastMouseX;
    double yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top
    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    // Constrain pitch
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

void Visualization::scrollCallback(double xoffset, double yoffset) {
    cameraDistance -= yoffset * 0.5f;
    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f;
    if (cameraDistance > 50.0f)
        cameraDistance = 50.0f;
}

#endif // ENABLE_VISUALIZATION