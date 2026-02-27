#ifndef JAGUAR_SHADER_VERTEX_BUFFER
#define JAGUAR_SHADER_VERTEX_BUFFER

#include "../OpenGL_Handling/OpenGL_Handling.h"

namespace Jaguar
{

	void Bind_Vertex_Buffer(const Vertex_Buffer& Buffer)
	{
		glBindBuffer(GL_ARRAY_BUFFER, Buffer.Vertex_Buffer_ID);
		glBindVertexArray(Buffer.Vertex_Attribute_ID);
	}

	void Initialise_Static_Lightmap_Vertex_Attributes(Vertex_Buffer* Target_Buffer)
	{
		glGenVertexArrays(1, &Target_Buffer->Vertex_Attribute_ID);
		glBindVertexArray(Target_Buffer->Vertex_Attribute_ID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)0); // Position vectors
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(3 * 4));	// Normal vectors
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(6 * 4));	// Texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(8 * 4));	// Lightmap texture coordinates
		glEnableVertexAttribArray(3);
	}

	void Initialise_Joint_Vertex_Attributes(Vertex_Buffer* Target_Buffer)
	{
		glGenVertexArrays(1, &Target_Buffer->Vertex_Attribute_ID);
		glBindVertexArray(Target_Buffer->Vertex_Attribute_ID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)0); // Position vectors
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(3 * 4));	// Normal vectors
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(6 * 4));	// Texture coordinates
		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Collada::Collada_Vertex), (void*)(8 * 4));	// Joint weight
		glEnableVertexAttribArray(3);

		glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Collada::Collada_Vertex), (void*)(9 * 4));		// Joint index
		glEnableVertexAttribArray(4);

		// Handle attribute buffer
	}

	template<typename Mesh_Type>
	void Update_Vertex_Buffer_Data(const Mesh_Type* Mesh, Vertex_Buffer* Target_Buffer)
	{
		const void* Data = Mesh->Vertices.data();
		size_t Size = Mesh->Vertices.size() * sizeof(Mesh_Type);

		glBindBuffer(GL_ARRAY_BUFFER, Target_Buffer->Vertex_Buffer_ID);
		glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW); // The vertex buffer won't be updated once we've fully generated it
	}

	template<typename Mesh_Type>
	void Create_Vertex_Buffer(const Mesh_Type* Mesh, Vertex_Buffer* Target_Buffer)
	{
		//Target_Buffer->Data = Mesh.Vertices.data();
		//Target_Buffer->Size = Mesh.Vertices.size() * sizeof(Collada::Collada_Vertex);

		Target_Buffer->Vertex_Count = Mesh->Vertices.size(); // Number of vertices is used in the draw call (not number of triangles)

		// Assigns data and size

		glGenBuffers(1, &Target_Buffer->Vertex_Buffer_ID);
		Update_Vertex_Buffer_Data(Mesh, Target_Buffer);
	}

	void Destroy_Vertex_Buffer(Vertex_Buffer* Target_Buffer)
	{
		// This'll destroy the OpenGL vertex buffer object, doesn't deallocate the mesh data
		glDeleteBuffers(1, &Target_Buffer->Vertex_Buffer_ID);
		glDeleteVertexArrays(1, &Target_Buffer->Vertex_Attribute_ID);
		Target_Buffer->Vertex_Buffer_ID = -1; // Sets to invalid mask
	}

}

#endif