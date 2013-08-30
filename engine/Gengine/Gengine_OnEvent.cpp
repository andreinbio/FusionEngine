#include "Gengine.hpp"

void Gengine::OnEvent(sf::Event & event) {

 switch (event.type)
 {
  case sf::Event::Closed:
   OnExit();
   break;

  case sf::Event::KeyPressed:
   switch(event.key.code) {

    case sf::Keyboard::P:
     ingame == true ? ingame = false : ingame = true;
     window.setMouseCursorVisible(!ingame);
     sf::Mouse::setPosition(sf::Vector2i(width/2, height/2), window);
     break;

    case sf::Keyboard::T:
    //cerr<<"key T true "<<sf::Keyboard::T<<endl;
     //tex_color == 1 ? tex_color = 0 : tex_color = 1;
     //TOTAL_OBJECTS == 1 ? TOTAL_OBJECTS = 0 : TOTAL_OBJECTS = 1;
     break;

    case sf::Keyboard::G:
     if(ingame)
     {
      OctreeCube.KeyGPressed(true);//cerr<<"keyG true "<<OctreeCube.return_keyG()<<endl;
     }
     break;

    case sf::Keyboard::Tab:
     if(ingame)
     {
      OctreeCube.UnmarkSelection();
     }
     break;
   }
   break;

  case sf::Event::KeyReleased:
   switch(event.key.code) {

    case sf::Keyboard::G:
     if(ingame)
     {
      OctreeCube.KeyGPressed(false);//cerr<<"keyG false "<< OctreeCube.return_keyG()<<endl;
     }
    break;

    case sf::Keyboard::T:
    //cerr<<"key T false "<<sf::Keyboard::T<<endl;
    break;
   }
   break;

  case sf::Event::MouseWheelMoved:
   if(ingame && OctreeCube.ReturnKeyG())
   {//cerr<<"update_level "<<OctreeCube.return_keyG()<<endl;
    OctreeCube.update_level(event.mouseWheel.delta);
   }
   else if(ingame && OctreeCube.IsSelected() && !OctreeCube.ReturnKeyG())
   {
    OctreeCube.GenerateOctreeData(event.mouseWheel.delta);
    OctreeMap.MapGeneration(OctreeCube.return_worldroot());
   }
   break;

  case  sf::Event::MouseButtonPressed:
   switch(event.key.code)
   {
    case sf::Mouse::Left:
     if(ingame)
     {
      OctreeCube.MarkSelection();
     }
    break;
   }
  break;

  case  sf::Event::MouseButtonReleased:
   //switch(event.key.code)
   //{
   // case sf::Mouse::Left:
   // on_click = false;
   // break;
   //}
  break;

 }

}

