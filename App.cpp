#include "App.hpp"
#include <glm/glm.hpp>
#include "World.hpp"

namespace {
constexpr unsigned int SCR_WIDTH = 1920;
constexpr unsigned int SCR_HEIGHT = 1080;
// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

GLFWwindow* createAppWindow() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //MAC OS ONLY!
    auto* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                    "OpenGL Pioter Test Window", NULL, NULL);
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

App::App() : camera{std::make_unique<Camera>(SCR_WIDTH, SCR_HEIGHT)} {
    std::cout << "App::Init!" << std::endl;

    // glfw window creation
    // --------------------
    window = createAppWindow();
    lastMouseXPos = static_cast<float>(SCR_WIDTH / 2);
    lastMouseYPos = static_cast<float>(SCR_HEIGHT / 2);

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
    glfwSetMouseButtonCallback(window, [](GLFWwindow* targetWindow, int button,
                                          int action, int mods) {
        App* app = static_cast<App*>(glfwGetWindowUserPointer(targetWindow));
        app->mouse_button_callback(targetWindow, button, action, mods);
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
}

App::~App() { std::cout << "App::Shutdown!" << std::endl; }

void App::run() {
    gameWorld = std::make_unique<World>();
    renderer = std::make_unique<Renderer>(SCR_WIDTH, SCR_HEIGHT);
    auto entity = std::make_unique<Entity>();
    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        const auto currentFps = frameTimeClock.calculateFps();

        gameWorld->setCameraPosition(camera->getPosition());
        gameWorld->updateLoadedChunks();

        processInput();
        renderer->updateShaders(*camera);
        entity->update(camera->getViewMatrix(), camera->getProjectionMatrix(),
                       *gameWorld);

        renderer->render(currentFps, *gameWorld);
        entity->render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::mouse_callback([[maybe_unused]] GLFWwindow* targetWindow,
                         float xposIn, float yposIn) {
    if (firstMouse) {
        lastMouseXPos = xposIn;
        lastMouseYPos = yposIn;
        firstMouse = false;
    }

    const auto xoffset = xposIn - lastMouseXPos;
    const auto yoffset =
        lastMouseYPos -
        yposIn; // reversed since y-coordinates go from bottom to top

    lastMouseXPos = xposIn;
    lastMouseYPos = yposIn;

    camera->processMouseMovement(xoffset, yoffset);
}

void App::scroll_callback([[maybe_unused]] GLFWwindow* targetWindow,
                          [[maybe_unused]] float xoffset, float yoffset) {
    camera->processMouseScroll(yoffset);
}

void App::mouse_button_callback([[maybe_unused]] GLFWwindow* targetWindow,
                                int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    bool result = false;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        result = gameWorld->addCubeFromRaycast(*camera, 5.0f, selectedCubeType);
        if (result) printf("Cube added via raycast.\n");
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        result = gameWorld->removeCubeFromRaycast(*camera, 5.0f);
        if (result) printf("Cube removed via raycast.\n");
    }
}

void App::processInput() {
    const auto cameraSpeed = 2.5f * frameTimeClock.getDeltaTime();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->processKeyboard(FORWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->processKeyboard(BACKWARD, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->processKeyboard(LEFT, cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->processKeyboard(RIGHT, cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        selectedCubeType = CubeType::SAND;
        printf("Selected SAND.\n");
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        selectedCubeType = CubeType::DIRT;
        printf("Selected DIRT.\n");
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        selectedCubeType = CubeType::GRASS;
        printf("Selected GRASS.\n");
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        selectedCubeType = CubeType::LOG;
        printf("Selected LOG.\n");
    } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        selectedCubeType = CubeType::LEAVES;
        printf("Selected LEAVES.\n");
    } else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        selectedCubeType = CubeType::TORCH;
        printf("Selected TORCH.\n");
    }
}