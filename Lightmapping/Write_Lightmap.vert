#version 440

uniform vec3 Positions[3];
uniform vec3 Triple_Vector;


uniform vec2 Lightmap_UVs_0;
uniform vec2 Lightmap_UVs_1;
uniform vec2 Lightmap_UVs_2;

out vec3 Position;
out vec3 Light_Position;
out vec3 Normal;

void main()
{
	vec2 Lightmap_UVs[3] = { Lightmap_UVs_0, Lightmap_UVs_1, Lightmap_UVs_2 };
	

	//Output_Lightmap_Vectors(TBN[0], TBN[2], TBN[1]);

	Position = Positions[gl_VertexID % 3];
	Normal = Triple_Vector;
	gl_Position = vec4(Lightmap_UVs[gl_VertexID % 3], 0.0, 1);
}