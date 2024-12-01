#include "stb_image.h"
#include "types.hpp"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <thread>

struct Image {
  i32 width, height, channels;
  u8 *data;
};

static GLuint texture;
static f64 angle;
static Image image;
static std::filesystem::path imagePath;

static void setup();
static void update();
static void draw();
static void setupTexture();
static void cleanup();
static void usage(const char *program);

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }

  imagePath = argv[1];

  if (glfwInit() == GLFW_FALSE) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
  glfwWindowHintString(GLFW_WAYLAND_APP_ID, "OpenGL");

  GLFWwindow *window =
      glfwCreateWindow(600, 600, "OpenGL Texture", nullptr, nullptr);

  if (window == nullptr) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  setupTexture();

  using std::chrono::milliseconds;
  const milliseconds deltaTimeMs(u32(1e3 / 60));

  setup();

  while (glfwWindowShouldClose(window) == 0) {
    update();
    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();

    using std::this_thread::sleep_for;
    sleep_for(deltaTimeMs);
  }

  cleanup();
}

void setup() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
}

void update() { angle += 0.01; }

void draw() {
  static constexpr f64 p = .5;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, texture);

  glPushMatrix();
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
  {
    glColor3ub(255, 255, 255);

    glRotated(angle, 0, 1, 0);

    glNormal3d(0, 0, 1);
    glBegin(GL_QUADS);
    {
      glTexCoord2d(0, 0);
      glVertex2d(-p, p);

      glTexCoord2d(0, 1);
      glVertex2d(-p, -p);

      glTexCoord2d(1, 1);
      glVertex2d(p, -p);

      glTexCoord2d(1, 0);
      glVertex2d(p, p);
    }
    glEnd();
  }
  glPopMatrix();
  glPopAttrib();
}

void setupTexture() {
  image.data = stbi_load(imagePath.c_str(), &image.width, &image.height,
                         &image.channels, 0);

  glGenTextures(1, &texture);

  // NOTE: Active and Bind Texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  // NOTE: Texture Settings
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // NOTE: Texture Mapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  /*f32 flatColor[] = {1, 1, 1, 1};*/
  /*glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);*/

  i32 format = 0;
  switch (image.channels) {
  case 3:
    format = GL_RGB8;
    break;
  case 4:
  default:
    format = GL_RGBA8;
    break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image.data);
  /*glGenerateMipMap();*/

  glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanup() {
  glDeleteTextures(1, &texture);
  stbi_image_free(image.data);
  image = {
      .width = 0,
      .height = 0,
      .channels = 0,
      .data = nullptr,
  };
}

void usage(const char *program) {
  if (program == nullptr) {
    exit(EXIT_FAILURE);
  }

  std::cerr << "Usage " << program << ": " << "<image path>" << std::endl;
  exit(EXIT_FAILURE);
}
