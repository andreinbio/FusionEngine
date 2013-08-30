#include "Main.hpp"

void Gengine::OnLoop() {

 if(ingame)
 {
  RealTimeImput(window, width, height);

  SetModelViewProjection();

  OctreeCube.updateCubeFace(position, direction, width, height, matrix);
 }

}

