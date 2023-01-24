#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdio.h>

GLFWwindow* _window = nullptr;
GLuint vertex_buffer = 0;
GLuint uv_buffer = 0;
GLuint shaderProgram = 0;

void SetupWindow() {
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(1280, 720, "PaletteProblem", NULL, NULL);

    if (_window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(_window);

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        glfwTerminate();
    }
}

void SetupVertexandUV() {
    GLfloat VertexBuffer[12]{};

    VertexBuffer[0] = -0.5f;
    VertexBuffer[1] =  0.5f;

    VertexBuffer[2] =  0.5f;
    VertexBuffer[3] =  0.5f;

    VertexBuffer[4] =  0.5f;
    VertexBuffer[5] = -0.5f;

    VertexBuffer[6] =  0.5f;
    VertexBuffer[7] = -0.5f;

    VertexBuffer[8] = -0.5f;
    VertexBuffer[9] = -0.5f;

    VertexBuffer[10] = -0.5f;
    VertexBuffer[11] =  0.5f;

    GLfloat UVBuffer[12]{};

    UVBuffer[0] = 0.0f;
    UVBuffer[1] = 0.0f;

    UVBuffer[2] = 1.0f;
    UVBuffer[3] = 0.0f;

    UVBuffer[4] = 1.0f;
    UVBuffer[5] = 1.0f;

    UVBuffer[6] = 1.0f;
    UVBuffer[7] = 1.0f;

    UVBuffer[8] = 0.0f;
    UVBuffer[9] = 1.0f;

    UVBuffer[10] = 0.0f;
    UVBuffer[11] = 0.0f;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * 4, VertexBuffer, GL_STATIC_DRAW);

    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * 4, UVBuffer, GL_STATIC_DRAW);
}

GLuint TextureSampler = 0;
GLuint PaletteSampler = 0;

const char* vertex_shader =
"#version 330 core\n"
"in vec2 vertexPosition;\n"
"in vec2 uvPosition;\n"
"out vec2 UV;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(vertexPosition.x ,vertexPosition.y, 0.0f, 1.0f);\n"
"	UV = uvPosition;\n"
"};\0";

const char* fragment_shader =
"#version 330 core\n"
"in vec2 UV;\n"
"out vec4 FragColor;\n"
"uniform sampler2D myTextureSampler;\n"
"uniform sampler1D myPaletteSampler;\n"
"void main()\n"
"{\n"
"	vec4 index = texture2D(myTextureSampler, UV);\n"
"   vec4 texel = texelFetch(myPaletteSampler, int(index.r * 255),0);\n"
"	FragColor = texel;\n"
"};\0";

void SetupSpriteandPalette() {
    //Drawing a smiley face where: the body should be red, eyes should be green and mouth should be blue
    unsigned char indexedsprite[8 * 8]{
        3,3,3,3,3,3,3,3,
        3,3,2,3,3,2,3,3,
        3,3,2,3,3,2,3,3,
        3,3,3,3,3,3,3,3,
        3,1,3,3,3,3,1,3,
        3,3,0,0,0,0,3,3,
        3,3,3,0,0,3,3,3,
        3,3,3,3,3,3,3,3,
    };

    //For testing the sprite without using the palette, only un-comment when FragColor = texture2D(myTextureSampler, UV);
    //for (int i = 0; i < 64; i++) {
    //    indexedsprite[i] *= 85;
    //}

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &TextureSampler);
    glBindTexture(GL_TEXTURE_2D, TextureSampler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, indexedsprite);
    glUniform1i(glGetUniformLocation(shaderProgram, "myTextureSampler"), 0);
    
    //Setting up a palette of four colors in an RGBA format
    unsigned char PaletteColors[4 * 4]{
        //RED
        255,
        0,
        0,
        255,

        //GREEN
        0,
        255,
        0,
        255,

        //BLUE
        0,
        0,
        255,
        255,

        //YELLOW
        255,
        255,
        0,
        255,
    };

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &PaletteSampler);
    glBindTexture(GL_TEXTURE_1D, PaletteSampler);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, PaletteColors);
    glUniform1i(glGetUniformLocation(shaderProgram, "myPaletteSampler"), 1);
}

void SetupShaders() {
    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, &vertex_shader, nullptr);
    glCompileShader(VertexShader);

    // Check Vertex Shader
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShader, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShader, 1, &fragment_shader, nullptr);
    glCompileShader(FragmentShader);

    // Check Fragment Shader
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShader, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, VertexShader);
    glAttachShader(shaderProgram, FragmentShader);
    glLinkProgram(shaderProgram);

    // Check the program
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Result);
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(shaderProgram, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    glUseProgram(shaderProgram);

    GLuint Init;
    glGenVertexArrays(1, &Init);
    glBindVertexArray(Init);
}

void DrawSprite() {

    GLuint vertexPositionID = glGetAttribLocation(shaderProgram, "vertexPosition");
    glEnableVertexAttribArray(vertexPositionID);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        vertexPositionID,   // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    GLuint uvPositionID = glGetAttribLocation(shaderProgram, "uvPosition");
    glEnableVertexAttribArray(uvPositionID);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(
        uvPositionID,   // attribute 0. No particular reason for 0, but must match the layout in the shader.
        2,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, TextureSampler);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, PaletteSampler);
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(vertexPositionID);
    glDisableVertexAttribArray(uvPositionID);
}

int main(void) {
    //Setting up the window, along with safety checks
    SetupWindow();

    //Setting up the shaders, along with safety checks
    SetupShaders();

    //Setting up the vertices and UVs for the sprite
    SetupVertexandUV();

    //Setting up the sprite and palette data
    SetupSpriteandPalette();

    do {

        glClearColor(0.1f, 0.4f, 0.8f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Drawing the sprite and setting up the Vertex-Position and UV-Position
        DrawSprite();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(_window));
	return 0;
}
