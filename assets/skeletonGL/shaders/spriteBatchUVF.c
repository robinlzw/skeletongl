#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform float deltaTime;
uniform vec2 spriteDimensions;

void main()
{
  //TexCoords.y = 1.0f - TexCoords.y;
  color = spriteColor * texture(image, TexCoords);

}
