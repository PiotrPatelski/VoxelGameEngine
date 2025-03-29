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
    renderer = std::make_unique<Renderer>(SCR_WIDTH, SCR_HEIGHT, *gameWorld);
    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        const auto currentFps = frameTimeClock.calculateFps();

        // USER INPUT
        processInput();
        renderer->updateShaders(camera);
        renderer->render(currentFps, *gameWorld);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
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

void App::mouse_button_callback(GLFWwindow* targetWindow, int button,
                                int action, int mods) {
    if (action != GLFW_PRESS) return;

    // Left click: place a cube (sand) on the face of the hit block.
    // Right click: remove the hit cube.
    glm::vec3 rayOrigin = camera.getPosition();
    glm::vec3 rayDirection = glm::normalize(camera.getFront());
    auto hitOpt = gameWorld->raycast(rayOrigin, rayDirection, 5.0f);
    if (hitOpt.has_value()) {
        auto hit = hitOpt.value();
        //-----------------------------
        // TODO ALIGN WITH CHUNK::ChunkSize
        const auto chunkSize = 64;
        //--------------------------------
        // For demonstration, we use left click to add and right click to
        // remove.
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            // Place cube: assume we place it adjacent to the hit block.
            // For simplicity, add in the same chunk using hit position +
            // (0,1,0) as offset.
            glm::ivec3 placeLocalPos =
                hit.position; // block position in world coordinates
            // Determine chunk indices and local coordinates.
            int chunkX = placeLocalPos.x / chunkSize;
            int chunkZ = placeLocalPos.z / chunkSize;
            glm::ivec3 localPos = {placeLocalPos.x % chunkSize, placeLocalPos.y,
                                   placeLocalPos.z % chunkSize};
            // For a better solution, use the hit face normal; here we simply
            // add one block above.
            localPos.y += 1;
            if (Chunk* chunk = gameWorld->getChunk(chunkX, chunkZ)) {
                if (chunk->addCube(localPos, CubeType::SAND))
                    std::cout << "Added cube at " << localPos.x << ", "
                              << localPos.y << ", " << localPos.z << "\n";
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // Remove cube at hit position.
            glm::ivec3 removeLocalPos = hit.position;
            int chunkX = removeLocalPos.x / chunkSize;
            int chunkZ = removeLocalPos.z / chunkSize;
            glm::ivec3 localPos = {removeLocalPos.x % chunkSize,
                                   removeLocalPos.y,
                                   removeLocalPos.z % chunkSize};
            if (Chunk* chunk = gameWorld->getChunk(chunkX, chunkZ)) {
                if (chunk->removeCube(localPos))
                    std::cout << "Removed cube at " << localPos.x << ", "
                              << localPos.y << ", " << localPos.z << "\n";
            }
        }
    }
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