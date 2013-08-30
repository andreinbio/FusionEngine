#include "Main.hpp"

void Gengine::OnRender() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 //glUseProgram(ShaderIds[0]);

 //glBindTexture(GL_TEXTURE_2D, TextureIds);
 //for (int i=0;i<TOTAL_OBJECTS;i++)
 //{

  //glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(Projection));
  //glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(View));
  //glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(Model[i]));

  //glBindVertexArray(vaoID[i]);

  //glDrawArrays(GL_TRIANGLES, 0, 3*object[i].nr_polygons);
  //*glDrawElements(GL_TRIANGLES, 3*object[i].nr_polygons, GL_UNSIGNED_BYTE, 0);
  //glBindVertexArray(0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
 //}

 //glBindTexture(GL_TEXTURE_2D, 0);
 //glUseProgram(0);
    if (ingame)
    {
        OctreeCube.Draw(SimpleShader, uniform, matrix);
        ExitOnGLError("ERROR: Octree.Draw ");
        OctreeMap.Render(SimpleShader, uniform, matrix);
        ExitOnGLError("ERROR: OctreeMap");
    }
    //draw the text/fps/other things...
    GLfloat color[4] = {0, 1, 1, 1};
    FpsFont.Display("Fps : " +Fps, 8, 20, color);
    GLfloat color2[4] = {1, 1, 0, 1};
    MpfFont.Display("Mpf : "+Mpf, 8, 40, color2);
    //end call

    window.display();

}

