#include "Hitboxes.h"

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Scene.h"

namespace Jaguar
{
	const glm::vec3 AABB_Direction_Vectors[6] =
	{
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),

		glm::vec3(-1, 0, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, 0, -1)
	};

	Collision_Info Flip_Collision(Collision_Info Info)
	{
		Info.Normal *= -1.0f;
		std::swap(Info.A, Info.B);

		return Info;
	}

	float Circle_Function(float Radius, float X)
	{
		float Diff = Radius * Radius - X * X;
		if (Diff > 0.0f)
			return std::sqrtf(Diff);
		else
			return 0.0f;
	}

	Collision_Info AABB_Sphere_Collision(AABB_Hitbox* This, Sphere_Hitbox* Other_Hitbox)
	{
		// This'll do an ordinary AABB delta check

		// Then, it'll check that length(dx, dy, dz) is less than radius
			// If so, use the sphere -> box centre vector for normal

		glm::vec3 A, B;

		A = This->A + This->Object->Position;
		B = This->B + This->Object->Position;

		glm::vec3 Sphere_Axes;
		Sphere_Axes.x = Circle_Function(Other_Hitbox->Radius,
			Clamp(A.z, Other_Hitbox->Object->Position.z, B.z)
			);

		Sphere_Axes.y = Circle_Function(Other_Hitbox->Radius,
			Clamp(A.x, Other_Hitbox->Object->Position.x, B.x)
		);

		Sphere_Axes.z = Circle_Function(Other_Hitbox->Radius,
			Clamp(A.y, Other_Hitbox->Object->Position.y, B.y)
		);

		float Deltas[6] =
		{
			B.x - Other_Hitbox->Object->Position.x,	// Only positive if sphere is within bounding box on x axis
			B.y - Other_Hitbox->Object->Position.y,
			B.z - Other_Hitbox->Object->Position.z,

			Other_Hitbox->Object->Position.x - A.x,
			Other_Hitbox->Object->Position.y - A.y,
			Other_Hitbox->Object->Position.z - A.z
		};

		glm::bvec3 Comparison(
			Deltas[0] > Deltas[3],	// True if -x is smaller than +x
			Deltas[1] > Deltas[4],
			Deltas[2] > Deltas[5]
		);

		uint8_t Indices[3] =
		{
			3 * Comparison[0],
			3 * Comparison[1] + 1,
			3 * Comparison[2] + 2
		};

		//glm::vec3 Delta_Vector;
		//Delta_Vector.x = Sphere_Axes.x + Deltas[Indices[0]];
		//Delta_Vector.y = Sphere_Axes.y + Deltas[Indices[1]];
		//Delta_Vector.z = Sphere_Axes.z + Deltas[Indices[2]];

		Deltas[Indices[0]] += Sphere_Axes.x;
		Deltas[Indices[1]] += Sphere_Axes.y;
		Deltas[Indices[2]] += Sphere_Axes.z;
		
		uint8_t Shortest = Deltas[Indices[0]] > Deltas[Indices[1]];
		Shortest = Deltas[Indices[Shortest]] > Deltas[Indices[2]] ? Indices[2] : Indices[Shortest];

		if (Deltas[Shortest] > 0.0f) // as long as there's an intersect of some kind!
		{
			Collision_Info Info;

			Info.B = This;
			Info.A = Other_Hitbox;

			Info.A_Points = std::vector<glm::vec3>{ Sphere_Axes + This->Object->Position };
			Info.B_Points = Info.A_Points;
			Info.Normal = AABB_Direction_Vectors[Shortest];

			Info.Delta = Deltas[Shortest];

			return Info;
		}
		else
			return Collision_Info();
	}

	Collision_Info Sphere_Hitbox::Sphere_Collision(Sphere_Hitbox* Other_Hitbox)
	{
		glm::vec3 Delta =
			Object->Position - Other_Hitbox->Object->Position;

		float Length_Squared = glm::dot(Delta, Delta);

		float Rad = Radius + Other_Hitbox->Radius;

		Rad *= Rad;

		if (Length_Squared <= Rad)
		{
			Collision_Info Info;
			Info.A = Other_Hitbox;
			Info.B = this;

			Info.Delta = sqrtf(Rad) - sqrtf(Length_Squared);

			Info.Normal = Delta * glm::vec3(-glm::inversesqrt(Length_Squared));
			Info.A_Points = 
				std::vector<glm::vec3>{
					glm::vec3(0.5f) * (Object->Position + Other_Hitbox->Object->Position + Info.Normal * glm::vec3(Radius - Other_Hitbox->Radius))
			};

			Info.B_Points = Info.A_Points;

			return Info;
		}
		else
			return Collision_Info();
	}

	Collision_Info Sphere_Hitbox::Test_Collision(Hitbox* Other_Hitbox)
	{
		return Other_Hitbox->Sphere_Collision(this);
	}

	Collision_Info Sphere_Hitbox::AABB_Collision(AABB_Hitbox* Other_Hitbox)
	{
		return Flip_Collision(
			AABB_Sphere_Collision(Other_Hitbox, this)
		);
	}

	Collision_Info AABB_Hitbox::Sphere_Collision(Sphere_Hitbox* Other_Hitbox)
	{
		return AABB_Sphere_Collision(this, Other_Hitbox);
	}

	Collision_Info AABB_Hitbox::Test_Collision(Hitbox* Other_Hitbox)
	{
		return Other_Hitbox->AABB_Collision(this);
	}

	Collision_Info AABB_Hitbox::AABB_Collision(AABB_Hitbox* Other_Hitbox)
	{
		glm::vec3 A_Vectors[2] = { A, Other_Hitbox->A };
		glm::vec3 B_Vectors[2] = { B, Other_Hitbox->B };

		glm::vec3 Position = Object ? Object->Position : glm::vec3(0.0f);

		A_Vectors[0] += Position;
		B_Vectors[0] += Position;

		Position = Other_Hitbox->Object ? Other_Hitbox->Object->Position : glm::vec3(0.0f);

		A_Vectors[1] += Position;
		B_Vectors[1] += Position;

		// Get 'deltas' for all 6 directions, finding the smallest value as you go

		// At the end, if the smallest value is less than zero, there is no collision
		// If it's still a positive value, that's the collision axis!
		
		float Deltas[6] =
		{
			B_Vectors[0][0] - A_Vectors[1][0],	// +x
			B_Vectors[0][1] - A_Vectors[1][1],	// +y
			B_Vectors[0][2] - A_Vectors[1][2],	// +z

			B_Vectors[1][0] - A_Vectors[0][0],	// -x
			B_Vectors[1][1] - A_Vectors[0][1],	// -y
			B_Vectors[1][2] - A_Vectors[0][2]	// -z
		};

		glm::bvec3 Comparison(			// This will also be used to get the average position of any collision that occurs
			Deltas[0] > Deltas[3],		// true if -x is smaller than +x
			Deltas[1] > Deltas[4],
			Deltas[2] > Deltas[5]
		);

		// Then, only two comparisons required to find smallest direction vector

		uint8_t Indices[3] =
		{
			Comparison[0] * 3,
			Comparison[1] * 3 + 1,
			Comparison[2] * 3 + 2
		};

		uint8_t Smallest_Value = Deltas[Indices[0]] > Deltas[Indices[1]];

		Smallest_Value = Deltas[Indices[Smallest_Value]] > Deltas[Indices[2]] ? Indices[2] : Indices[Smallest_Value];

		if (Deltas[Smallest_Value] > 0)
		{
			// Collision!

			Collision_Info Info;

			Info.B = this;
			Info.A = Other_Hitbox;

			Info.Delta = Deltas[Smallest_Value];
			Info.Normal = AABB_Direction_Vectors[Smallest_Value];

			// With AABB hitboxes, we only need 1 'average' position for most cases.

			Info.A_Points.resize(1);

			Info.A_Points[0][0] = B_Vectors[Comparison[0]][0] + A_Vectors[!Comparison[0]][0];
			Info.A_Points[0][1] = B_Vectors[Comparison[1]][1] + A_Vectors[!Comparison[1]][1];
			Info.A_Points[0][2] = B_Vectors[Comparison[2]][2] + A_Vectors[!Comparison[2]][2];

			Info.A_Points[0] *= 0.5f;	// This gets the average of the A and B overlap vectors

			Info.B_Points = Info.A_Points;

			return Info;
		}
		else
		{
			return Collision_Info();	// No collision!
		}
	}

	//

	bool Check_Existing_AABB_Hitbox(const std::vector<AABB_Hitbox*>& Bounding_Boxes, const AABB_Hitbox& New_Hitbox)
	{
		// I'll just check first if they're equal

		for (size_t Index = 0; Index < Bounding_Boxes.size(); Index++)
		{
			bool Found = true;
			for (size_t Axis = 0; Axis < 3; Axis++)
			{
				/*if (
					(Bounding_Boxes[Index]->A[Axis] == New_Hitbox.A[Axis]) &&
					(Bounding_Boxes[Index]->B[Axis] == New_Hitbox.B[Axis])
					)
					return false;	// We can't add another one! Return false*/

				Found &= 
					(Bounding_Boxes[Index]->A[Axis] == New_Hitbox.A[Axis]) &&
					(Bounding_Boxes[Index]->B[Axis] == New_Hitbox.B[Axis]);
			}

			if (Found)
				return false;
		}

		return true;		// We may add another one! Return true
	}

	std::vector<Hitbox*> Wrap_Sphere_Hitbox(const Collada::Collada_Mesh* Model_Mesh, float Thickness)
	{
		Sphere_Hitbox Sphere;
		Sphere.Radius = 0.0f;

		for (size_t Index = 0; Index < Model_Mesh->Vertices.size(); Index++)
			Sphere.Radius = std::fmaxf(Sphere.Radius, glm::length(Model_Mesh->Vertices[Index].Position));

		Sphere.Radius += Thickness;

		return std::vector<Hitbox*>{ new Sphere_Hitbox(Sphere) };
	}

	std::vector<Hitbox*> Wrap_AABB_Hitbox(const Collada::Collada_Mesh* Model_Mesh, float Thickness)
	{
		AABB_Hitbox Box;

		Box.A = glm::vec3(99999999.0f);
		Box.B = glm::vec3(-99999999.0f);

		for(size_t Index = 0; Index < Model_Mesh->Vertices.size(); Index++)
			for (size_t Axis = 0; Axis < 3; Axis++)
			{
				glm::vec3 Vert = glm::vec3(-1.0f, 1.0f, 1.0f) * Model_Mesh->Vertices[Index].Position;

				Box.A[Axis] = std::fminf(Box.A[Axis], Vert[Axis]);
				Box.B[Axis] = std::fmaxf(Box.B[Axis], Vert[Axis]);
			}

		Box.A -= glm::vec3(Thickness);
		Box.B += glm::vec3(Thickness);

		return std::vector<Hitbox*>{ new AABB_Hitbox(Box) };
	}

	std::vector<Hitbox*> Wrap_AABB_Hitboxes(const Collada::Collada_Mesh* Model_Mesh, float Thickness)
	{
		std::vector<Hitbox*> Bounding_Boxes;	// This will just be used to track the bounding boxes of all the triangles
													// these will be compiled down afterwards

		//

		for (size_t Index = 0; Index < Model_Mesh->Vertices.size(); Index += 3)
		{
			const Collada::Collada_Vertex* Vertices = Model_Mesh->Vertices.data() + Index;

			AABB_Hitbox Box;

			Box.A = glm::vec3(-1.0f, 1.0f, 1.0f) * Vertices[0].Position;
			Box.B = glm::vec3(-1.0f, 1.0f, 1.0f) * Vertices[0].Position;

			for (size_t Point = 1; Point < 3; Point++)
				for (size_t Axis = 0; Axis < 3; Axis++)
				{
					glm::vec3 Vert = glm::vec3(-1.0f, 1.0f, 1.0f) * Vertices[Point].Position;

					Box.A[Axis] = std::fminf(Box.A[Axis], Vert[Axis]);
					Box.B[Axis] = std::fmaxf(Box.B[Axis], Vert[Axis]);
				}

			Box.A -= glm::vec3(Thickness);
			Box.B += glm::vec3(Thickness);

			// This gets the min/max of each triangle

			if (Check_Existing_AABB_Hitbox(reinterpret_cast<const std::vector<AABB_Hitbox*>&>(Bounding_Boxes), Box))	// If we don't have this one already,
				Bounding_Boxes.push_back(new AABB_Hitbox(Box));		// add it!


		}

		return Bounding_Boxes;
	}

}