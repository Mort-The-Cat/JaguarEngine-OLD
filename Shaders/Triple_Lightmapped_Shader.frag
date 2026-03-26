#version 440

out vec4 Out_Colour;

uniform sampler2D Albedo_Texture;
uniform sampler2D Normal_Texture;

uniform sampler2D Lightmap_0_Texture;
uniform sampler2D Lightmap_1_Texture;
uniform sampler2D Lightmap_2_Texture;

uniform samplerCube Environment_Cubemap;
uniform vec3 Cubemap_Origin;
uniform vec3 Cubemap_AABB[2];	// 0 is A, 1 is B

uniform float Inverse_Lightmap_Size;

uniform vec3 Camera_Position;

in vec2 Texture_Coordinates;
in vec3 Position;
in vec3 Normal;

in vec3 Geometry_Normal;		// Note that the geometry normal may not equal the otherwise shaded normal
in vec3 Texture_Tangent;		// Tangent, based on texture space
in vec3 Texture_Bitangent;		// Bitangent, based on texture space

in vec3 Lightmap_Vector_0;
in vec3 Lightmap_Vector_1;
in vec3 Lightmap_Vector_2;

in vec2 Lightmap_Coordinates;

vec3 Final_Normal;

float square(float value)
{
	return value * value;
}

vec3 Parallax_Corrected_Reflection_Vector(vec3 Reflection_Vector)
{
	vec3 Side;

	vec3 Lambdas;

	for(uint Face = 0; Face < 3; Face++)
		Side[Face] = Cubemap_AABB[ int(Reflection_Vector[Face] > 0.0f) ][Face];

	Lambdas.x = (Side.x - Position.x) / Reflection_Vector.x;
	Lambdas.y = (Side.y - Position.y) / Reflection_Vector.y;
	Lambdas.z = (Side.z - Position.z) / Reflection_Vector.z;

	float L = min(min(Lambdas.x, Lambdas.y), Lambdas.z); // If we get a negative value, it's because we're outside the cube... That's okay it'll just be an erroneous value

	if(L < 0.0f)
	{
		Reflection_Vector *= -1;
	}

	vec3 Origin = Cubemap_AABB[0] + Cubemap_AABB[1];

	Origin *= 0.5f;

	//Origin = Cubemap_Origin;

	vec3 Intersection_Vector = Reflection_Vector * L + Position - Origin;

	for(uint Face = 0; Face < 3; Face++)
		Side[Face] = Cubemap_AABB[ int(Intersection_Vector[Face] > 0.0f) ][Face];

	//Origin = Cubemap_AABB[0] + Cubemap_AABB[1];
	//Origin *= 0.5f;

	return Intersection_Vector / abs(Side - Origin);
}

void Get_Final_Normal()
{
	mat3 TBN_Matrix = mat3(Texture_Tangent, Texture_Bitangent, Geometry_Normal);

	vec3 Sampled_Normal = vec3(-2, -2, 1) * texture(Normal_Texture, Texture_Coordinates).xyz + vec3(1, 1, 0);

	Final_Normal = normalize(TBN_Matrix * Sampled_Normal);
}

void main()
{
	// float Inverse_Length = inversesqrt(dot(Position, Position));

	vec2 LM_Coordinates = Lightmap_Coordinates * vec2(Inverse_Lightmap_Size);

	Get_Final_Normal();

	vec3 Sum_Lighting = 
		texture(Lightmap_0_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_0)) + 
		texture(Lightmap_1_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_1)) +
		texture(Lightmap_2_Texture, LM_Coordinates).rgb * max(0, dot(Final_Normal, Lightmap_Vector_2));

	vec3 Camera_To_Pixel = normalize(Position - Camera_Position);

	vec3 Reflection_Vector = Parallax_Corrected_Reflection_Vector(
						reflect(
							Camera_To_Pixel,
							Final_Normal
						));

	vec3 Reflection = texture(
						Environment_Cubemap, 
						Reflection_Vector
					).xyz;

	Reflection *= sqrt(1 - square(dot(Camera_To_Pixel, Final_Normal))) * 0.25f;

	Out_Colour = 
				vec4(
					pow(Sum_Lighting, vec3(1.0f)), 
				1) * 
				vec4(
					texture(Albedo_Texture, Texture_Coordinates).rgb, 
				1) +
				vec4(Reflection, 1.0f);

	// Out_Colour = vec4(Sum_Lighting, 1);

	// Out_Colour = vec4(Final_Normal * vec3(2) + vec3(1), 1.0f);

	//gl_FragColor = texture(Lightmap_Texture, LM_Coordinates);

	//vec4(Normal * 0.5 + 0.5, 1);
}