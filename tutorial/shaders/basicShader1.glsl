#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform vec4 lightColor;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform vec4 lightDirection;

out vec4 Color;

void main()
{
	Color = ( texture(sampler1, texCoord0)*0.6 + texture(sampler2, texCoord0)*0.4 )* vec4(color0,1.0); //you must have Color
}
