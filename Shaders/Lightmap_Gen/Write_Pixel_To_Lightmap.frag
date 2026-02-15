#version 440

layout(location = 0) out vec3 Out_Component_0;
layout(location = 1) out vec3 Out_Component_1;
layout(location = 2) out vec3 Out_Component_2;

uniform sampler2D Lightmap_0_Texture;
uniform sampler2D Lightmap_1_Texture;
uniform sampler2D Lightmap_2_Texture;

void main()
{
	Out_Component_0 = textureLod(Lightmap_0_Texture, vec2(0.0), 10).xyz * 9.0;
	Out_Component_1 = textureLod(Lightmap_1_Texture, vec2(0.0f), 10).xyz * 9.0;
	Out_Component_2 = textureLod(Lightmap_2_Texture, vec2(0.0f), 10).xyz * 9.0;
}