#version 330 core
in  vec2  TexCoords;
out vec4  color;

uniform sampler2D scene;
uniform vec2 textureDimensions;
uniform vec2 mousePosition;

void main()
{
  //NOTE: on a 960GTX, the color is by default initialized to
  //proper values, in some intel HD integrated cards they do not
  //you have to explicitly initialize the color to the value you want
  color = vec4(0.0, 0.0, 0.0, 0.0);
  color = texture(scene, TexCoords);
}
