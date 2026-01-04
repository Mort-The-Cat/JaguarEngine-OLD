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

			Info.A = this;
			Info.B = Other_Hitbox;

			Info.Delta = Deltas[Smallest_Value];
			Info.Normal = AABB_Direction_Vectors[Smallest_Value];

			// With AABB hitboxes, we only need 1 'average' position for most cases.

			Info.Points.resize(1);
			
			Info.Points[0][0] = B_Vectors[Comparison[0]][0] + A_Vectors[!Comparison[0]][0];
			Info.Points[0][1] = B_Vectors[Comparison[1]][1] + A_Vectors[!Comparison[1]][1];
			Info.Points[0][2] = B_Vectors[Comparison[2]][2] + A_Vectors[!Comparison[2]][2];

			Info.Points[0] *= 0.5f;	// This gets the average of the A and B overlap vectors

			return Info;
		}
		else
		{
			return Collision_Info();	// No collision!
		}
	}

}