#pragma once

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <cmath>

#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <GL/glew.h>

#include <FreeImagePlus.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

typedef struct {
 glm::mat4 Projection, View, Model;
}MVPmatrices;

typedef struct {
 GLuint Proj, View, Model;
}MVPuniforms;

class utils {

 protected:
  GLuint ShaderIds[3];
   GLuint vaoID,
           vboVerticesID,
           vboTexCoordID,
           vboNormalsID,
           iboVerticesID;
           //vboIndicesID;
            //GLuint vaoID[MAX_OBJECTS],
                            //vboVerticesID[MAX_OBJECTS],
                                //vboTexCoordID[MAX_OBJECTS],
                                //vboNormalsID[TOTAL_OBJECTS],
                                //vboIndicesID[MAX_OBJECTS];
            GLuint TextureIds;

  FIBITMAP* bitmap;
  int bitmap_width, bitmap_height;

 protected:
  MVPmatrices matrix;
  MVPuniforms uniform;
  glm::mat4 Translate;
  glm::mat4 Rotate;
  glm::mat4 Scale;

  glm::mat4 Matrices[3];

  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 right;
  glm::vec3 up;
  glm::vec3 jump;

  float horizontalAngle;
  float verticalAngle;
  float FOV;
  float speed;
  float mouseSpeed;

  bool ingame;

  sf::Clock clock;
  sf::Time elapsed;
  float deltatime;
  float sumtime;
  int frame;

  string Fps;
  string Mpf;

 public:
  void ExitOnGLError(const char* error_message);
  GLuint LoadShader(const char* filename, GLenum shader_type);
  void CreateTextures();
  GLuint CreateShaders(const char* fragment, const char* vertex);

  void DeleteTextures();
  void DeleteShaders();

  void Framerate();

  void SetUpMatrices();
  void SetModelViewProjection();
  void SetMVPuniformLocation(GLuint shader);

  void RealTimeImput(sf::Window &window, short int width, short int height);
};

