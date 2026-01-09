#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in uint Joint_Weight;
layout(location = 4) in uint Joint_Index;

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

out DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 UV;
} Vertex;

void main()
{
	Vertex.UV = In_Texture_Coordinates;

	Vertex.Position = (  Model_Matrix * vec4(In_Position, 1)  ).xyz;
	Vertex.Normal = mat3(Model_Matrix) * In_Normal;

	//Vertex.Position = vec3(
	//	Model_Matrix * vec4(vec3(Joint_Matrices[Joint_Index] * vec4(In_Position, 1.0f)), 1)		// 4d vector is required to apply translation to position
	//);

	//Vertex.Normal = mat3(Model_Matrix) * (mat3(Joint_Matrices[Joint_Index]) * normalize(In_Normal)); // This provides only the ROTATION to the normals (which is what we want)

	gl_Position = Projection_Matrix * vec4(Vertex.Position, 1);
}