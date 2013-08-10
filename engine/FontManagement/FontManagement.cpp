#include "FontManagement.hpp"

void FontManagement::LoadFont() {

 FT_Library library;
 if (FT_Init_FreeType(&library))
 {
   cerr << "Could not init freetype library" << endl;
   return;
 }

 if (FT_New_Face(library, "fonts/Play-Regular.ttf", 0, &face))
 {
   cerr << "Could not open font" << endl;
   return;
 }

 if (FT_Set_Pixel_Sizes(face, 0, 16))
 {
   cerr << "Could not set size of the font" << endl;
 }

 //Creating a short cut for face->glyph
 g = face->glyph;

 //cout << "num_faces " << face->num_glyphs << endl;

 //==============================================
 //some examples of information that will be used
 //==============================================
 //g->bitmap.buffer
 // Pointer to the 8-bit greyscale image of the glyph, rendered at the previously selected font size.
 //g->bitmap.width
 // Width of the bitmap, in pixels.
 //g->bitmap.height
 // Height of the bitmap, in pixels.
 //g->bitmap_left
 // Horizontal position relative to the cursor, in pixels.
 //g->bitmap_top
 // Vertical position relative to the baseline, in pixels.
 //g->advance.x
 // How far to move the cursor horizontally for the next character, in 1/64 pixels.
 //g->advance.y
 // How far to move the cursor vertically for the next character, in 1/64 pixels, almost always 0.
 //==============================================

}

/*void FontManagement::CreateFontShaders() {

 //load and create shaders

 FontShader[1] = LoadShader("Shaders/FontShader.fragment.glsl", GL_FRAGMENT_SHADER);
 FontShader[2] = LoadShader("Shaders/FontShader.vertex.glsl", GL_VERTEX_SHADER);


 FontShader[0] = glCreateProgram();
 ExitOnGLError("ERROR: Could not create the shader program");
 glAttachShader(FontShader[0], FontShader[1]);
 glAttachShader(FontShader[0], FontShader[2]);

 glLinkProgram(FontShader[0]);
 ExitOnGLError("ERROR: Could not link the shader program");

}*/

void FontManagement::SetFontuniformLocation() {
 //glGetError();
 uniform_tex = glGetUniformLocation(ShaderIds[0], "tex");
 ExitOnGLError("ERROR: uniform_tex ");
 uniform_color = glGetUniformLocation(ShaderIds[0], "color");
 ExitOnGLError("ERROR: uniform_color ");
 attribute_coord = glGetAttribLocation(ShaderIds[0], "coord");
 ExitOnGLError("ERROR: attribute_coord ");

}

void FontManagement::SetGLFontData() {

 wTexture = 0;
 hTexture = 0;

 for(int i = 32; i < 128; i++)
 {
  if(FT_Load_Char(face, i, FT_LOAD_RENDER))
  {
    cerr << "Loading character " << i << " failed!" << endl;
    continue;
  }

  wTexture += g->bitmap.width;
  hTexture = max(hTexture, g->bitmap.rows);
 }

 glUseProgram(ShaderIds[0]);
 glActiveTexture(GL_TEXTURE0);
 glGenTextures(1, &TextureIds);
 glBindTexture(GL_TEXTURE_2D, TextureIds);
 glUniform1i(uniform_tex, 0);

 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, wTexture, hTexture, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


 int x = 0;
 for(int i = 32; i < 128; i++)
 {
  if(FT_Load_Char(face, i, FT_LOAD_RENDER))
  {
    cerr << "Loading character " << i << " second time failed!" << endl;
    continue;
  }

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);



  c[i].ax = g->advance.x >> 6;
  c[i].ay = g->advance.y >> 6;

  c[i].bw = g->bitmap.width;
  c[i].bh = g->bitmap.rows;

  c[i].bl = g->bitmap_left;
  c[i].bt = g->bitmap_top;

  c[i].tx = (float)x / wTexture;

  x += g->bitmap.width;

 }


 glGenBuffers(1, &vboVerticesID);

 glBindTexture(GL_TEXTURE_2D, 0);

 glUseProgram(0);


}

void FontManagement::Render_text(string text, float x, float y, float sx, float sy) {

  glBindTexture(GL_TEXTURE_2D, TextureIds);
  glUniform1i(uniform_tex, 0);

  glEnableVertexAttribArray(attribute_coord);
  glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
  glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

  struct point {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
  } coords[6 * text.length()];

  int n = 0;

  for(char & p : text) {
    float x2 =  x + c[p].bl * sx;
    float y2 = -y - c[p].bt * sy;
    float w = c[p].bw * sx;
    float h = c[p].bh * sy;

    //Advance the cursor to the start of the next character
    x += c[p].ax * sx;
    y += c[p].ay * sy;

    //Skip glyphs that have no pixels
    if(!w || !h)
    {
      continue;
    }

    coords[n++] = (point){x2,     -y2 - h, c[p].tx,                c[p].bh / hTexture};
    coords[n++] = (point){x2 + w, -y2    , c[p].tx + c[p].bw / wTexture, 0};
    coords[n++] = (point){x2,     -y2    , c[p].tx,                0};
    coords[n++] = (point){x2 + w, -y2    , c[p].tx + c[p].bw / wTexture, 0};
    coords[n++] = (point){x2,     -y2 - h, c[p].tx,                c[p].bh / hTexture};
    coords[n++] = (point){x2 + w, -y2 - h, c[p].tx + c[p].bw / wTexture, c[p].bh / hTexture};

  }


  glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, n);


  glBindTexture(GL_TEXTURE_2D, 0);
  glDisableVertexAttribArray(attribute_coord);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FontManagement::Display(string text, int xpos, int ypos, GLfloat color[4]) {

  float sx = 2.0 / window_W;
  float sy = 2.0 / window_H;

  glUseProgram(ShaderIds[0]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUniform4fv(uniform_color, 1, color);



  //Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
  //            -1 + 8 * sx,   1 - 50 * sy,    sx, sy);

  Render_text(text,
              -1 + xpos * sx,   1 - ypos * sy,    sx, sy);
/*
  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 80 * sy,    sx, sy);

  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 140 * sy,    sx, sy);

  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 200 * sy,    sx, sy);

  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 260 * sy,    sx, sy);

  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 320 * sy,    sx, sy);

  Render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
              -1 + 8 * sx,   1 - 380 * sy,    sx, sy);

  Render_text("Fps : "+Fps,
              -1 + 8 * sx,   1 - 440 * sy,    sx, sy);

  Render_text("Fps : "+Fps,
              -1 + 8 * sx,   1 - 500 * sy,    sx, sy);
*/

  glDisable(GL_BLEND);

  glUseProgram(0);
}

void FontManagement::SetUpFonts(int width, int height) {

 LoadFont();
 ExitOnGLError("ERROR: loadfont ! ");
 CreateShaders("Shaders/FontShader.fragment.glsl", "Shaders/FontShader.vertex.glsl");
 ExitOnGLError("ERROR: createShaders ! ");
 SetFontuniformLocation();
 ExitOnGLError("ERROR: fontuniformlocation ! ");
 SetGLFontData();
 ExitOnGLError("ERROR: GLfontData ! ");
 Getwindow_WH(width, height);

}

void FontManagement::Getwindow_WH(int width, int height) {
 window_W = width;
 window_H = height;
}

/*void FontManagement::DeleteFontShader() {

 glDetachShader(FontShader[0], FontShader[1]);
 glDetachShader(FontShader[0], FontShader[2]);
 glDeleteShader(FontShader[1]);
 glDeleteShader(FontShader[2]);
 glDeleteProgram(FontShader[0]);
 ExitOnGLError("ERROR: Could not delete FontShader");

}*/

/*void FontManagement::DeleteFontTexture() {

 glBindTexture(GL_TEXTURE_2D, 0);
 glDeleteTextures(1, &FontTexture);
 ExitOnGLError("ERROR: Could not delete FontTexture");

}*/

