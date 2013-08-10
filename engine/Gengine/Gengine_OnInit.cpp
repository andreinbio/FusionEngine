#include "Gengine.hpp"

bool Gengine::OnInit() {

 window.create(sf::VideoMode(width, height), "SFML Gengine", sf::Style::Close);

 window.setKeyRepeatEnabled(false);

 glewExperimental = GL_TRUE;

 if (glewInit() != GLEW_OK)
 {
  exit( -1 );
 }

 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 glClearDepth(1.0f);

 glViewport(0, 0, width, height);

 glEnable(GL_DEPTH_TEST);
 glDepthFunc(GL_LESS);
 ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

 //glEnable(GL_CULL_FACE);
 //glCullFace(GL_BACK);

 //glFrontFace(GL_CCW);
 //ExitOnGLError("ERROR: Could not set OpenGL culling options");

 FpsFont.SetUpFonts(width, height);
 ExitOnGLError("ERROR: FpsFont ");
 MpfFont.SetUpFonts(width, height);
 ExitOnGLError("ERROR: TEXT ");

 SimpleShader = CreateShaders("Shaders/SimpleShader.fragment.glsl", "Shaders/SimpleShader.vertex.glsl");
 ExitOnGLError("ERROR: Could not create SimpleShader ");
 //MapShader = CreateShaders("Shaders/")

 OctreeCube.setup_octree(SimpleShader);
 ExitOnGLError("ERROR: setup_cube_octree ");
 OctreeMap.setup_octree2(SimpleShader);
 ExitOnGLError("ERROR: setup_map_octree");
 OctreeMap.MapGeneration(OctreeCube.return_worldroot());

 SetUpMatrices();
 ExitOnGLError("ERROR: SetUpMatrices ");
 SetModelViewProjection();
 ExitOnGLError("ERROR: SetModelViewProjection ");
 SetMVPuniformLocation(SimpleShader);
 ExitOnGLError("ERROR: SetMVPuniformLocation ");

 return true;

}

