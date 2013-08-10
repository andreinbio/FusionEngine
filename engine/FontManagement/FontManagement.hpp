#include <ft2build.h>
#include FT_FREETYPE_H

#include "../utils/utils.hpp"


typedef struct {
  float ax; // advance.x
  float ay; // advance.y

  float bw; // bitmap.width;
  float bh; // bitmap.rows;

  float bl; // bitmap_left;
  float bt; // bitmap_top;

  float tx; // x offset of glyph in texture coordinates
} character_info;

class FontManagement: public utils {

 private:
  GLuint uniform_tex,
         uniform_color,
         attribute_coord;

  FT_Face face;
  FT_GlyphSlot g;

  character_info c[128];

  int wTexture;
  int hTexture;

  int window_W;
  int window_H;

 public:
  void LoadFont();
  void CreateFontShaders();
  void SetFontuniformLocation();
  void SetGLFontData();
  void SetUpFonts(int width, int height);
  void Render_text(string text, float x, float y, float sx, float sy);
  void Display(string text, int xpos, int ypos, GLfloat color[4]);
  void Getwindow_WH(int width, int height);

  //void DeleteFontShader();
  //void DeleteFontTexture();

};

