#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#ifdef ENABLE_VISUALIZATION

    #include <GLFW/glfw3.h>
    #include <armadillo>
    #include <vector>

class Visualization {
  public:
    Visualization(int width = 800, int height = 600);
    ~Visualization();

    bool initialize();
    void cleanup();
    bool shouldClose();
    void clear();
    void swapBuffers();
    void processInput();

    void drawBody(const arma::vec& position, const arma::vec& rotation, double mass);
    void drawJoint(const arma::vec& pos1, const arma::vec& pos2);
    void drawChain(const std::vector<arma::vec>& positions);

    void setCamera(float x, float y, float z);
    void updateView();
    void updateCamera();

  private:
    GLFWwindow* window;
    int width, height;
    float cameraX, cameraY, cameraZ;
    float cameraYaw, cameraPitch;
    float cameraDistance;
    float cameraSpeed;
    float mouseSensitivity;
    float targetX, targetY, targetZ;
    bool firstMouse;
    double lastMouseX, lastMouseY;

    void drawSphere(float x, float y, float z, float radius);
    void drawLine(float x1, float y1, float z1, float x2, float y2, float z2);
    void setupLighting();

    void mouseCallback(double xpos, double ypos);
    void scrollCallback(double xoffset, double yoffset);
};

#endif // ENABLE_VISUALIZATION

#endif // VISUALIZATION_HPP