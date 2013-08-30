#include "Main.hpp"

int main(int argc, char* argv[]) {


    Gengine theApp;


    return theApp.OnExecute();
}

Gengine::Gengine() {

    Running = true;
    ingame = false;
    width = 1024;
    height = 768;

    tex_color = 1;
}

int Gengine::OnExecute() {

	if (OnInit() == false) {
	     return -1;
	}

	sf::Event event;

	while (Running) {
	     while (window.pollEvent(event)) {
	           OnEvent(event);
	     }

	     OnLoop();
	     RenderManager();
	     Framerate();
	}

	OnCleanup();

	return 0;
}

void Gengine::OnExit() {

    Running = false;
}

