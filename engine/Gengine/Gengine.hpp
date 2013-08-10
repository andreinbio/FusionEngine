#include "../utils/utils.hpp"
#include "../FontManagement/FontManagement.hpp"
#include "../OctreeManagement/OctreeManagement.hpp"

class Gengine: public utils {

 private:
  bool Running;
  //bool ingame;
  int tex_color;

  sf::Window window;
  short int width;
  short int height;

  FontManagement FpsFont;
  FontManagement MpfFont;
  OctreeManagement OctreeCube;
  OctreeManagement OctreeMap;

 private:
  GLuint SimpleShader;
  GLuint MapShader;


 public:

  Gengine();

  int OnExecute();
  void RenderManager();

  bool OnInit();

  void OnEvent(sf::Event & event);

  void OnExit();

  void OnLoop();
  void OnRender();
  void OnCleanup();


};

