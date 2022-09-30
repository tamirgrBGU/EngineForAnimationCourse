#version 330

in vec3 position;
in vec3 normal;
in vec4 Ka;
in vec4 Kd;
in vec4 Ks;
in vec2 texcoord;

out vec3 normal0;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;


void main()
{
	normal0 = vec3(Model* vec4(normal, 0.0));
	gl_Position = Proj *View * Model* vec4(position, 1.0);
}
