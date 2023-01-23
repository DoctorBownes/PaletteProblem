#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int main(void) {

    GLFWwindow* _window = nullptr;
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(1280, 720, "PaletteProblem", NULL, NULL);

    if (_window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(_window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        glfwTerminate();
    }

    do {

        glClearColor(0.1f, 0.4f, 0.8f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glfwSwapBuffers(_window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(_window));
	return 0;
}