#ifndef JAGUAR_OPENGL_HANDLING
#define JAGUAR_OPENGL_HANDLING

#define CRT_SECURE_NO_WARNINGS 1

#define TRIPLE_LIGHTMAPPING 1

#include "../Collada_Loader/Collada_Loader.h"

namespace Jaguar
{

	class Shader
	{
	public:
		GLuint Program_ID;
	};

	/*template<typename Vertex_Attributes, typename Uniform_Attributes>
	class Shader_Type : public Shader
	{

	};*/

	void Create_Shader(const char* Fragment, const char* Vertex, Shader* Target_Shader, const char* Geometry = nullptr);
	void Destroy_Shader(Shader* Target_Shader);
	void Use_Shader(const Shader Target_Shader);

	void Throw_Error(const char* Formatted_String);

	// For now, I'll just do a test shader to see if I've got things loading correctly

	struct Vertex_Buffer
	{
		//const void* Data;
		//size_t Size;

		// This means that another object/wrapper needs to keep track of the allocated memory. (This just uses a reference to it)

		GLuint Vertex_Buffer_ID;
		GLuint Vertex_Attribute_ID;

		size_t Vertex_Count;
	};

	//template<class Mesh_Type>
	//void Update_Vertex_Buffer_Data(Mesh_Type* Mesh, Vertex_Buffer* Target_Buffer);
	void Update_Vertex_Buffer_Data(const Collada::Collada_Mesh* Mesh, Vertex_Buffer* Target_Buffer);

	//template<class Mesh_Type>
	//void Create_Vertex_Buffer(Mesh_Type* Mesh, Vertex_Buffer* Target_Buffer);
	void Create_Vertex_Buffer(const Collada::Collada_Mesh* Mesh, Vertex_Buffer* Target_Buffer);

	void Bind_Vertex_Buffer(const Vertex_Buffer& Buffer);
	void Destroy_Vertex_Buffer(Vertex_Buffer* Target_Buffer);

	void Initialise_Static_Lightmap_Vertex_Attributes(Vertex_Buffer* Target_Buffer);
	void Initialise_Joint_Vertex_Attributes(Vertex_Buffer* Target_Buffer);

	float Clamp(float Min, float Value, float Max);
}

#endif