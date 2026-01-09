#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_Texture_Coordinates;
layout(location = 3) in uint Joint_Weight;
layout(location = 4) in uint Joint_Index;

// NOTE that the joint values are packed in such a way that

// Joint_Index = Index_0 | (Index_1 << 16)
// and that the joint-weights are two unsigned 16-bit values, representing a value between 0.0 - 1.0, which sum to 1.0

// This allows for each vertex to make use of two bones without any additional overhead 

uniform mat4 Projection_Matrix;
uniform mat4 Model_Matrix;

uniform mat4 Joint_Matrices[64];

out DATA
{
	vec3 Position;
	vec3 Normal;
	
	vec2 UV;
} Vertex;

void main()
{
	Vertex.UV = In_Texture_Coordinates;

	vec3 Local_Position = vec3(0);
	vec3 Local_Normal = vec3(0);

	for(int Bone = 0; Bone < 2; Bone++)
	{
		float Weight = float((Joint_Weight >> (16 * Bone)) & 65535) / 65535.0;
		uint Index = (Joint_Index >> (16 * Bone)) & 65535;

		Local_Position += Weight * vec3(
		Model_Matrix * vec4(vec3(Joint_Matrices[Index] * vec4(In_Position, 1.0f)), 1)		// 4d vector is required to apply translation to position
		);

		Local_Normal += Weight * (mat3(Model_Matrix) * (mat3(Joint_Matrices[Index]) * In_Normal));
	}

	Vertex.Position = Local_Position;
	Vertex.Normal = normalize(Local_Normal);
	
	//Vertex.Position = vec3(
	//	Model_Matrix * vec4(vec3(Joint_Matrices[Joint_Index] * vec4(In_Position, 1.0f)), 1)		// 4d vector is required to apply translation to position
	//);

	//Vertex.Normal = mat3(Model_Matrix) * (mat3(Joint_Matrices[Joint_Index]) * normalize(In_Normal)); // This provides only the ROTATION to the normals (which is what we want)

	gl_Position = Projection_Matrix * vec4(Vertex.Position, 1);
}