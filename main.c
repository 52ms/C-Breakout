#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.h"

#include <stdio.h>

// TO COMPILE: gcc main.c glad.c -o main -lglfw -lGL -ldl && ./main

#define SCR_WIDTH 600
#define SCR_HEIGHT 600

void processInput(GLFWwindow* window);

Shader shader;

typedef struct {
    float x;
    float y;
    float width;
    float height;
} Object;

// void draw(float* vertices, float x, float y, float width, float height, int r, int g, int b);

void update_ball_pos(GLFWwindow* window);

Object player = {-1.0, -1.0, 0.3, 0.05};
Object ball = {-0.02f, -0.02f, 0.04f, 0.04f};
Object block = {-1.0, 1.0, 0.2, 0.05};

float playerOffsetX = 1.0f - (0.25 / 2);
float playerOffsetY = 0.1;
const float playerSpeed = 0.02f;

float ballOffsetX = 0, ballOffsetY = 0;
float ballSpeedX = 0.007f, ballSpeedY = 0.01f;

#define NUM_Y_BLOCKS 6
#define NUM_X_BLOCKS 9
float blocks[NUM_Y_BLOCKS][NUM_X_BLOCKS][2];
void init_blocks() {
    float stepX = block.width + (block.width / NUM_X_BLOCKS);
    float stepY = 0.1;
    for (int i = 0; i < NUM_Y_BLOCKS; ++i) {
        for (int k = 0; k < NUM_X_BLOCKS; ++k) {
            blocks[i][k][0] = (stepX * k) + 0.01;
            blocks[i][k][1] = -(stepY * i) - block.height - 0.1;
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Breakout", NULL, NULL);
    if (window == NULL) {
        printf("Unable to create window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    init_blocks();


    shader = init_shader("vertexShader.glsl", "fragmentShader.glsl");


    float vertices[] = {
        // player
        player.x, player.y,
        player.x, player.y + player.height,
        player.x + player.width, player.y,
        player.x + player.width, player.y + player.height,

        // Ball
        ball.x, ball.y,
        ball.x, ball.y + ball.height,
        ball.x + ball.width, ball.y,
        ball.x + ball.width, ball.y + ball.height,

        // block
        block.x, block.y,
        block.x, block.y + block.height,
        block.x + block.width, block.y,
        block.x + block.width, block.y + block.height,
    };

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);

        update_ball_pos(window);

        // Player
        shader.set_float("offsetX", playerOffsetX);
        shader.set_float("offsetY", playerOffsetY);
        shader.set_float("colorR", 0.1);
        shader.set_float("colorG", 0.2);
        shader.set_float("colorB", 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Ball
        shader.set_float("offsetX", ballOffsetX);
        shader.set_float("offsetY", ballOffsetY);
        shader.set_float("colorR", 1.0);
        shader.set_float("colorG", 1.0);
        shader.set_float("colorB", 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

        for (int i = 0; i < NUM_Y_BLOCKS; ++i) {
            float colorR = 1.0, colorG = 1.0, colorB = 1.0;
            switch (i) {
                case 0:
                case 1:
                    colorR = 1.0;
                    colorG = 0.0;
                    colorB = 0.0;
                    break;
                case 2:
                case 3:
                    colorR = 1.0;
                    colorG = 1.0;
                    colorB = 0.0;
                    break;
                case 4:
                case 5:
                    colorR = 0.0;
                    colorG = 1.0;
                    colorB = 0.0;
                    break;
            }
            for (int k = 0; k < NUM_X_BLOCKS; ++k) {
                shader.set_float("offsetX", blocks[i][k][0]);
                shader.set_float("offsetY", blocks[i][k][1]);

                shader.set_float("colorR", colorR);
                shader.set_float("colorG", colorG);
                shader.set_float("colorB", colorB);

                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    shader.delete();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (playerOffsetX + player.x > -1) {
            playerOffsetX -= playerSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (playerOffsetX + player.x + player.width < 1) {
            playerOffsetX += playerSpeed;
        }
    }
}

void update_ball_pos(GLFWwindow* window) {
    static int revX = 0, revY = 0;

    const float ballLeft = (ballOffsetX + ball.x);
    const float ballRight = (ballOffsetX + ball.x) + (ball.width);
    const float ballTop = (ballOffsetY + ball.y) + (ball.height);
    const float ballBottom = (ballOffsetY + ball.y);


    // Wall collision
    if (ballTop > 1) revY = 0;
    if (ballBottom < -1) revY = 1; // need to change this to do something

    if (ballLeft < -1) revX = !revX;
    if (ballRight > 1) revX = !revX;

    
    // Player collsion
    const float playerLeft = playerOffsetX + player.x;
    const float playerRight = playerOffsetX + player.x + player.width;
    const float playerTop = playerOffsetY + player.y + player.height;
    const float playerBottom = playerOffsetY + player.y;

    if (ballBottom <= playerTop && ballTop >= playerBottom) {
        if (ballRight >= playerLeft && ballLeft <= playerRight) {
            revY = 1;
            revX = (ballRight > playerLeft + (player.width / 1.5)) ? 0 : (ballLeft < playerRight - (player.width / 1.5)) ? 1 : revX;
            ballOffsetY += ballSpeedY;
        }
    }


    // Block collision
    int blockCounter = 0;
    for (int i = 0; i < NUM_Y_BLOCKS; ++i) {
        for (int k = 0; k < NUM_X_BLOCKS; ++k) {
            if (blocks[i][k][0] < -1) continue;

            ++blockCounter;

            const float blockLeft = blocks[i][k][0] + block.x;
            const float blockRight = blocks[i][k][0] + block.width + block.x;
            const float blockTop = blocks[i][k][1] + block.height + block.y;
            const float blockBottom = blocks[i][k][1] + block.y;

            if (ballBottom <= blockTop && ballTop >= blockBottom) {
                if (ballRight >= blockLeft && ballLeft <= blockRight) {
                    revY = !revY;
                    blocks[i][k][0] = -2.0;
                    break;
                }
            }
        }
    }

    if (blockCounter == 0) glfwSetWindowShouldClose(window, GL_TRUE);


    ballOffsetX += (revX) ? (-ballSpeedX) : ballSpeedX;
    ballOffsetY -= (revY) ? (-ballSpeedY) : ballSpeedY;
}
