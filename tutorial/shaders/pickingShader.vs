#version 330

in vec3 position;
in vec3 normal;
in vec3 color;
in vec4 Ka;
in vec4 Kd;
in vec4 Ks;
in vec2 texCoords;


uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

//out vec3 color0;

void main()
{
	//color0 = color;
	gl_Position = Proj *View * Model* vec4(position, 1.0);
}
