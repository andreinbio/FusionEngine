#version 330

//in vec2 UV;
//in vec2 Colors;
out vec4 fsColor;

uniform sampler2D myTextureSampler;
uniform vec4 color;

void main() {

    //fsColor = texture(myTextureSampler, UV);
    fsColor = color;
    //fsColor = Colors;

}

