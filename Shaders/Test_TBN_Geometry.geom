#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 Position;
out vec3 Normal;

out vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
out vec3 Texture_Tangent;		// Tangent, based on texture space
out vec3 Texture_Bitangent;		// Bitangent, based on texture space
out vec2 Texture_Coordinates;
out vec2 Lightmap_Coordinates;

// We'll also output the triple-lightmapping vectors
out vec3 Lightmap_Vector_0;
out vec3 Lightmap_Vector_1;
out vec3 Lightmap_Vector_2;



void Output_Lightmap_Vectors(vec3 Tangent, vec3 Normal, vec3 Bitangent)
{
	const float Sqrt_Sixth = sqrt(1.0f / 6.0f);
	const float Sqrt_Half = sqrt(0.5f);
	const float Sqrt_Third = sqrt(1.0f / 3.0f);
	const float Sqrt_Three_Halves = sqrt(2.0f / 3.0f);

	Normal *= Sqrt_Third;
	Bitangent *= Sqrt_Half;

	Lightmap_Vector_0 = Normal;
	Lightmap_Vector_1 = Normal;
	Lightmap_Vector_2 = Normal;

	Lightmap_Vector_2 += Sqrt_Three_Halves * Tangent;

	Tangent *= Sqrt_Sixth;

	Lightmap_Vector_0 -= Tangent + Bitangent;
	Lightmap_Vector_1 += Bitangent - Tangent;
}

in DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 Lightmap_Coordinates;

	vec2 UV;
} Vertex[];

void main()
{
	vec2 UV_A = Vertex[2].UV - Vertex[0].UV;
	vec2 UV_B = Vertex[1].UV - Vertex[0].UV;

	vec3 Edge_A;
	vec3 Edge_B;

	Edge_A = Vertex[2].Position.xyz - Vertex[0].Position.xyz;
	Edge_B = Vertex[1].Position.xyz - Vertex[0].Position.xyz;

	float Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

	vec3 Tangent = -normalize(Inv * (UV_B.y * Edge_A - UV_A.y * Edge_B));

	Texture_Bitangent = -normalize(Inv * (UV_A.x * Edge_B - UV_B.x * Edge_A));

	//

	for(int W = 0; W < 3; W++)
	{
		gl_Position = gl_in[W].gl_Position;
		Position = Vertex[W].Position;
		Normal = Vertex[W].Normal;
		Texture_Coordinates = Vertex[W].UV;
		Texture_Tangent = Tangent;
		//Texture_Bitangent = cross(Tangent, Normal);

		Geometry_Normal = Normal;

		Output_Lightmap_Vectors(Tangent, Normal, Texture_Bitangent);

		Lightmap_Coordinates = Vertex[W].Lightmap_Coordinates;

		EmitVertex();
	}

	EndPrimitive();
}