#version 440

layout(location = 0) out vec3 Out_Component_0;
layout(location = 1) out vec3 Out_Component_1;
layout(location = 2) out vec3 Out_Component_2;

uniform sampler2D Albedo_Texture;

uniform vec3 Light_Colour;

uniform vec3 Lightmap_Surface_Normal;
uniform vec3 Lightmap_Surface_Tangent;
uniform vec3 Lightmap_Surface_Bitangent;

uniform vec3 Camera_Position;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

void main()
{
	vec3 Vector = normalize(Position - Camera_Position);

	Out_Component_0 = vec3( max(0, dot(Vector, Lightmap_Surface_Normal)) ) * Light_Colour;
	Out_Component_1 = vec3( max(0, dot(Vector, Lightmap_Surface_Tangent)) ) * Light_Colour;
	Out_Component_2 = vec3( max(0, dot(Vector, Lightmap_Surface_Bitangent)) ) * Light_Colour;
}