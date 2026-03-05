#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in vec2 In_Lightmap_Coordinates;

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

out DATA
{
	vec3 Position;
	vec3 Normal;

	vec2 Lightmap_Coordinates;
	
	vec2 UV;
} Vertex;

void main()
{
	Vertex.Lightmap_Coordinates = In_Lightmap_Coordinates;
	Vertex.UV = In_Texture_Coordinates;

	Vertex.Position = vec3(
		Model_Matrix * vec4(In_Position, 1.0f)		// 4d vector is required to apply translation to position
	);

	//Vertex.Position = vec3(In_Lightmap_Coordinates, )

	Vertex.Normal = mat3(Model_Matrix) * normalize(In_Normal); // This provides only the ROTATION to the normals (which is what we want)

	vec4 Screenspace_Position = Projection_Matrix * vec4(Vertex.Position, 1);

	gl_Position = Screenspace_Position;
}