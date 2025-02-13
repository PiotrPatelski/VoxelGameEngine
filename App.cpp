#include "App.hpp"
#include <glm/glm.hpp>
#include "World.hpp"

namespace {

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

GLFWwindow* createAppWindow(unsigned int width, unsigned int height) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MAC OS ONLY!
    auto* window = glfwCreateWindow(width, height, "OpenGL Pioter Test Window",
                                    NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::abort();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // setting mouse handling
    // --------------------
    glfwSetInputMode(window, GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED); // HIDE THE CURSOR

    return window;
}

} // namespace

App::App() {
    std::cout << "App::Init!" << std::endl;

    // glfw window creation
    // --------------------
    window = createAppWindow(SCR_WIDTH, SCR_HEIGHT);
    lastX = static_cast<float>(SCR_WIDTH / 2);
    lastY = static_cast<float>(SCR_HEIGHT / 2);

    // set input callbacks
    //-------------------
    // Associate the App instance with the GLFW window
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow* targetWindow, double xpos,
                                        double ypos) {
        App* app = static_cast<App*>(glfwGetWindowUserPointer(targetWindow));
        app->mouse_callback(targetWindow, xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* targetWindow, double xoffset,
                                     double yoffset) {
        App* app = static_cast<App*>(glfwGetWindowUserPointer(targetWindow));
        app->scroll_callback(targetWindow, xoffset, yoffset);
    });

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer = std::make_unique<Renderer>(SCR_WIDTH, SCR_HEIGHT);
}

App::~App() { std::cout << "App::Shutdown!" << std::endl; }

void App::run() {
    auto gameWorld = std::make_unique<World>();
    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        const auto currentFps = frameTimeClock.calculateFps();

        // USER INPUT
        processInput();
        renderer->updateShaders(camera);
        renderer->render(currentFps, *gameWorld);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

//--------------------------------------------------------------------------
// window param present eventhough unused only for signature of dependent lib
void App::mouse_callback(GLFWwindow* targetWindow, double xposIn,
                         double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

//--------------------------------------------------------------------------
// window param present eventhough unused only for signature of dependent lib
void App::scroll_callback(GLFWwindow* targetWindow, double xoffset,
                          double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

void App::processInput() {
    const auto cameraSpeed = 2.5f * frameTimeClock.getDeltaTime();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(FORWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, cameraSpeed);
    }
}