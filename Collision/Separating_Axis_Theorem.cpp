#include "Hitboxes.h"

namespace Jaguar
{
	long Check_Existing_Vector(const std::vector<glm::vec3>& Vectors, glm::vec3 Vector)
	{
		for (size_t Index = 0; Index < Vectors.size(); Index++)
			if (Vectors[Index] == Vector)
				return Index;

		return Vectors.size();
	}

	long Check_Existing_Face(const std::vector<Mesh_Hitbox::SAT_Face>& Faces, glm::vec3 Normal)
	{
		for (size_t Index = 0; Index < Faces.size(); Index++)
			if (Faces[Index].Normal == Normal)
				return Index;

		return -1;
	}

	void Create_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh, Mesh_Hitbox* Target_Hitbox)
	{
		for (size_t Index = 0; Index < Model_Mesh->Vertices.size(); Index += 3)
		{
			glm::vec3 Normal = glm::normalize(glm::cross(
				Model_Mesh->Vertices[Index + 1].Position - Model_Mesh->Vertices[Index].Position,
				Model_Mesh->Vertices[Index + 2].Position - Model_Mesh->Vertices[Index].Position));

			long Existing_Index;

			for (size_t Point = 0; Point < 3; Point++)
				if (
					(Existing_Index = Check_Existing_Vector(Target_Hitbox->Points, Model_Mesh->Vertices[Index + Point].Position)) ==
					Target_Hitbox->Points.size()
					)
					Target_Hitbox->Points.push_back(Model_Mesh->Vertices[Index + Point].Position);

			if (Check_Existing_Face(Target_Hitbox->Faces, Normal))
			{

				Mesh_Hitbox::SAT_Face New_Face;

				New_Face.Point_Index = Existing_Index;
				New_Face.Normal = Normal;
				
				Target_Hitbox->Faces.push_back(New_Face);
			}

			// Great! Then, check edges...
		}
	}

}