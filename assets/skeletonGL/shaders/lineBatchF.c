#version 330 core

out vec4 color;

uniform vec4 lineColor;
uniform float deltaTime;

void main()
{
  //TexCoords.y = 1.0f - TexCoords.y;
  color = lineColor;
}
