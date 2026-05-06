#include "Hitboxes.h"

#include "../OpenGL_Handling/Scene.h"

namespace Jaguar
{
	// Collision_Info 

	void Mesh_Hitbox::Update_Hitbox()
	{
		// This will update the 'transformed_points' list
		// as well as the transformed normals and edges for the face and edge list

		glm::mat3 Matrix = Get_Model_Matrix(Object);

		Transformed_Points.resize(Points.size());

		for (size_t Point = 0; Point < Transformed_Points.size(); Point++)
		{
			Transformed_Points[Point] = Matrix * Points[Point];
		}

		for (size_t Face = 0; Face < Faces.size(); Face++)
		{
			Faces[Face].Transformed_Normal = Matrix * Faces[Face].Normal;
		}

		for (size_t Edge = 0; Edge < Edges.size(); Edge++) // oh good heavens
		{
			Edges[Edge].Transformed_Edge = Matrix * Edges[Edge].Edge;
		}
	}

	Collision_Info SAT_Collision_Check_Face(Mesh_Hitbox* Hitbox_A, Mesh_Hitbox* Hitbox_B)
	{
		const float Large_Number = 999999.0f;

		Collision_Info Info;

		Info.Delta = Large_Number;

		// std::vector<glm::vec3> A_Points;

		for (size_t Face = 0; Face < Hitbox_A->Faces.size(); Face++)
		{
			Collision_Info New_Info;

			New_Info.Delta = -Large_Number;

			//New_Info.A_Points.resize(1);

			for (size_t Point = 0; Point < Hitbox_B->Transformed_Points.size(); Point++)
			{
				// transforms the point

				// does the normal calculations...

				float Local_Delta = -glm::dot(Hitbox_A->Faces[Face].Transformed_Normal, 
					Hitbox_B->Object->Position + Hitbox_B->Transformed_Points[Point] -
					Hitbox_A->Transformed_Points[Hitbox_A->Faces[Face].Point_Index] - Hitbox_A->Object->Position
				);

				New_Info.Delta = std::fmaxf(New_Info.Delta, Local_Delta);
				//if(Local_Delta == New_Info.Delta)
				//	New_Info.A_Points[0] = Hitbox_B->Transformed_Points[Point] + Hitbox_B->Object->Position;

				if (Local_Delta >= 0.0f)
					New_Info.A_Points.push_back(Hitbox_B->Transformed_Points[Point] + Hitbox_B->Object->Position);
			}

			if (New_Info.Delta < Info.Delta)
			{
				Info.Delta = New_Info.Delta;
				Info.Normal = Hitbox_A->Faces[Face].Transformed_Normal;
				Info.A_Points.clear();
				Info.A_Points = std::move(New_Info.A_Points);
			}
		}

		return Info;
	}

	// for AABB hitboxes, just create a mesh and use that

	Mesh_Hitbox Mesh_Hitbox_From_AABB(AABB_Hitbox* AABB)
	{
		Mesh_Hitbox Hitbox;

		Hitbox.Object = AABB->Object;

		Hitbox.Transformed_Points = std::vector<glm::vec3>{
			glm::vec3(AABB->A.x, AABB->A.y, AABB->A.z),
			glm::vec3(AABB->A.x, AABB->A.y, AABB->B.z),
			glm::vec3(AABB->A.x, AABB->B.y, AABB->A.z),
			glm::vec3(AABB->A.x, AABB->B.y, AABB->B.z),
			glm::vec3(AABB->B.x, AABB->A.y, AABB->A.z),
			glm::vec3(AABB->B.x, AABB->A.y, AABB->B.z),
			glm::vec3(AABB->B.x, AABB->B.y, AABB->A.z),
			glm::vec3(AABB->B.x, AABB->B.y, AABB->B.z),
		};	// 8 points for 1 cube

		Hitbox.Faces = std::vector<Mesh_Hitbox::SAT_Face>{
			Mesh_Hitbox::SAT_Face(0, glm::vec3(0, 0, -1)),
			Mesh_Hitbox::SAT_Face(0, glm::vec3(0, -1, 0)),
			Mesh_Hitbox::SAT_Face(0, glm::vec3(-1, 0, 0)),

			Mesh_Hitbox::SAT_Face(7, glm::vec3(0, 0, 1)),
			Mesh_Hitbox::SAT_Face(7, glm::vec3(0, 1, 0)),
			Mesh_Hitbox::SAT_Face(7, glm::vec3(1, 0, 0))
		};

		// I won't bother with edges for now

		return Hitbox;
	}

	//

	Collision_Info SAT_Collision(Mesh_Hitbox* Hitbox_A, Mesh_Hitbox* Hitbox_B)
	{
		// First, it will handle the faces

		// If there's a separation? End early with no collision

		// Otherwise? Also check edges

		// THEN create the collision info and return if appropriate

		Collision_Info Tests[3]; // 0 and 1 are faces, 2 is edge cross products

		Tests[0] = SAT_Collision_Check_Face(Hitbox_A, Hitbox_B);
		Tests[1] = SAT_Collision_Check_Face(Hitbox_B, Hitbox_A);
		
		Tests[1].Normal *= -1;
		Tests[0].A = Hitbox_B;
		Tests[0].B = Hitbox_A;

		Tests[0].B_Points = std::move(Tests[0].A_Points);
		Tests[1].A = Hitbox_B;
		Tests[1].B = Hitbox_A;

		// User lesser of the two

		// positive = collision
		// negative = separation

		bool Index = Tests[1].Delta < Tests[0].Delta;

		// printf(" >> %f\n", Tests[Index].Delta);

		if (Tests[Index].Delta > 0.0f)
		{
			// collision...

			//bool Point_Index = Hitbox_B->Object->Get_Physics_Object() != nullptr;
			//Point_Index ^= Hitbox_A->Object->Get_Physics_Object() != nullptr;

			//Point_Index = Point_Index ? Hitbox_B->Object->Get_Physics_Object() == nullptr : Index;

			//Tests[Index].A_Points//std::move(Tests[Point_Index].Points);

			Tests[Index].A_Points = std::move(Tests[1].A_Points);
			Tests[Index].B_Points = std::move(Tests[0].B_Points);

			return Tests[Index];
		}
		else
			return Collision_Info(); // no collision!
	}

	//

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

	long Check_Existing_Edge(const std::vector<Mesh_Hitbox::SAT_Edge>& Edges, glm::vec3 Vector)	// Don't like how I'm repeating this code but it's whatever
	{
		for (size_t Index = 0; Index < Edges.size(); Index++)
			if (Edges[Index].Edge == Vector || Edges[Index].Edge == -Vector)
				return Index;

		return -1;
	}

#define Vert_Fix(Vertex) (Vertex * glm::vec3(-1.0f, 1.0f, 1.0f))

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
					(Existing_Index = Check_Existing_Vector(Target_Hitbox->Points, Model_Mesh->Vertices[Index + Point].Position))  ==
					Target_Hitbox->Points.size()
					)
					Target_Hitbox->Points.push_back( Model_Mesh->Vertices[Index + Point].Position );

			if (Check_Existing_Face(Target_Hitbox->Faces, Normal) == -1)
			{

				Mesh_Hitbox::SAT_Face New_Face;

				New_Face.Point_Index = Existing_Index;
				New_Face.Normal = Normal;
				
				Target_Hitbox->Faces.push_back(New_Face);
			}

			// Great! Then, check edges...

			for (size_t Edge = 0; Edge < 3; Edge++)
			{
				// 3 edges per face

				Normal = glm::normalize(Model_Mesh->Vertices[Index + ((Edge + 1) % 3)].Position - Model_Mesh->Vertices[Index + Edge].Position);

				if (Check_Existing_Edge(Target_Hitbox->Edges, Normal) == -1) // check for redundancy 
				{
					Mesh_Hitbox::SAT_Edge New_Edge;

					New_Edge.Edge = Normal;
					New_Edge.Point_Index = Index + Edge;
					
					Target_Hitbox->Edges.push_back(New_Edge);
				}
			}
		}

		// after that, all points, faces, and edges have been placed into the SAT data structure and all geometric redundancies have been removed
	}

	std::vector<Hitbox*> Wrap_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh)
	{
		std::vector<Hitbox*> Hitboxes(1);

		Hitboxes[0] = new Mesh_Hitbox();

		Create_Mesh_Hitbox(Model_Mesh, (Mesh_Hitbox*)Hitboxes[0]);

		return Hitboxes;
	}

	//

	Collision_Info Mesh_Hitbox::Test_Collision(Hitbox* Other_Hitbox)
	{
		return Other_Hitbox->Mesh_Collision(this);
	}

	Collision_Info Mesh_Hitbox::Mesh_Collision(Mesh_Hitbox* Other_Hitbox)
	{
		return SAT_Collision(this, Other_Hitbox);
	}

	Collision_Info Mesh_AABB_Collision(Mesh_Hitbox* Hitbox_A, AABB_Hitbox* Hitbox_B)
	{
		Mesh_Hitbox Mesh_B = Mesh_Hitbox_From_AABB(Hitbox_B);

		Collision_Info Info = Hitbox_A->Mesh_Collision(&Mesh_B);

		Info.A = Hitbox_B; // replace placeholder Mesh_B pointer with actual hitbox pointer

							// (this is done because Mesh_B goes out of scope)
							// (and otherwise managing that memory would be a veritable nightmare and a waste of time)

		return Info;
	}

	Collision_Info AABB_Hitbox::Mesh_Collision(Mesh_Hitbox* Other_Hitbox)
	{
		return Flip_Collision(Other_Hitbox->AABB_Collision(this));
	}

	Collision_Info Mesh_Hitbox::AABB_Collision(AABB_Hitbox* Other_Hitbox)
	{
		return Mesh_AABB_Collision(this, Other_Hitbox);
	}

}