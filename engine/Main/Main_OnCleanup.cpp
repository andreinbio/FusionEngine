#include "Main.hpp"

void Gengine::OnCleanup() {

 FpsFont.DeleteTextures();
 MpfFont.DeleteTextures();
 FpsFont.DeleteShaders();
 MpfFont.DeleteShaders();

 window.close();

}

