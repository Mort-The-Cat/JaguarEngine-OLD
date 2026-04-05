#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec3 Lighting_Node_Uniform_0[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_1[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_2[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_3[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_4[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_5[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_6[6];			// this will be interpolated accordingly
uniform vec3 Lighting_Node_Uniform_7[6];			// this will be interpolated accordingly

uniform vec3 Lighting_Node_Position;

uniform vec3 Lighting_Node_Deltas;

out vec3 Lighting_Node[6];

out vec3 Position;
out vec3 Normal;

out vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
out vec3 Texture_Tangent;		// Tangent, based on texture space
out vec3 Texture_Bitangent;		// Bitangent, based on texture space
out vec2 Texture_Coordinates;

in DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 UV;
} Vertex[];

void Interpolate_Lighting_Nodes(vec3 Sample_Position)
{
	vec3 Delta = Sample_Position - Lighting_Node_Position;

	Delta *= Lighting_Node_Deltas;	// If -2.0f, then a delta of -0.5f will become a factor of 1 (because we want to use THAT node instead)

	Delta[0] = clamp(Delta[0], 0, 1);
	Delta[1] = clamp(Delta[1], 0, 1);
	Delta[2] = clamp(Delta[2], 0, 1);

	float Factors[8];

	Factors[0] = 1 - Delta[0];
	Factors[1] = 1 - Delta[2];
	Factors[2] = Factors[0] * Delta[2];
	Factors[3] = Delta[0] * Delta[2];

	Factors[0] *= Factors[1];
	Factors[1] *= Delta[0];

	Factors[4] = Delta[1] * Factors[0];
	Factors[5] = Delta[1] * Factors[1];
	Factors[6] = Delta[1] * Factors[2];
	Factors[7] = Delta[1] * Factors[3];

	Delta[1] = 1 - Delta[1];

	Factors[0] *= Delta[1];
	Factors[1] *= Delta[1];
	Factors[2] *= Delta[1];
	Factors[3] *= Delta[1];

	// That does all of the factors! Great!

	for(uint Index = 0; Index < 6; Index++) // This gets all 6 faces
	{
		Lighting_Node[Index] = 
			Factors[0] * Lighting_Node_Uniform_0[Index] +
			Factors[1] * Lighting_Node_Uniform_1[Index] +
			Factors[2] * Lighting_Node_Uniform_2[Index] +
			Factors[3] * Lighting_Node_Uniform_3[Index] +
			
			Factors[4] * Lighting_Node_Uniform_4[Index] +
			Factors[5] * Lighting_Node_Uniform_5[Index] +
			Factors[6] * Lighting_Node_Uniform_6[Index] + 
			Factors[7] * Lighting_Node_Uniform_7[Index];
	}
}

void main()
{
	vec2 UV_A = Vertex[1].UV - Vertex[0].UV;
	vec2 UV_B = Vertex[2].UV - Vertex[0].UV;

	vec3 Edge_A;
	vec3 Edge_B;

	Edge_A = Vertex[1].Position.xyz - Vertex[0].Position.xyz;
	Edge_B = Vertex[2].Position.xyz - Vertex[0].Position.xyz;

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

		Interpolate_Lighting_Nodes(Vertex[W].Position);

		// Lighting_Node = Lighting_Node_Uniform_0;

		EmitVertex();
	}

	EndPrimitive();
}