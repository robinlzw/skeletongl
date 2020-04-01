#version 330 core

layout (location = 0) in vec4 vertex; //vec2 pos, vec texcoord
out vec2 TexCoords;


uniform mat4 model;
uniform mat4 projection;

void main()
{
  TexCoords = vertex.zw;
  //flips image veritcally, opengl flips teh y axis, fixed in ortho matrix
  //TexCoords = vec2(vertex.z, 1.0f - vertex.w);

  gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}

