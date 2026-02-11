#version 440

uniform vec2 Pixel;

void main()
{
	gl_Position = vec4(Pixel * 2 - vec2(1), 0, 1);
}