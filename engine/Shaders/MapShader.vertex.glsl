#version 330

layout(location = 0) in vec3 vsVertex;
//layout(location = 1) in vec2 vsUV;

//out vec2 UV;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    gl_Position = Projection * View * Model * vec4(vsVertex, 1.0);

    //UV = vsUV;
}



/*
layout(location = 0) in vec3 Position;
//layout(location = 1) in vec2 vertexUV;
uniform mat4 MVP;
//out vec2 UV;

void main(){

    vec4 pos = vec4(Position,1);

    gl_Position = MVP * pos;



    //UV = vertexUV;
}
*/
