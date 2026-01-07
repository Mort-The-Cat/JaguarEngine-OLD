#ifndef JAGUAR_HITBOXES
#define JAGUAR_HITBOXES

#include "../OpenGL_Handling/OpenGL_Handling.h"

namespace Jaguar
{
	class World_Object;

	class Hitbox;

	class AABB_Hitbox;

	struct Collision_Info
	{
		glm::vec3 Normal;		// The normal from B to A
		float Delta = 0;		// 0 if no collision / positive if collision
		Hitbox* A;
		Hitbox* B;

		std::vector<glm::vec3> Points;

		// I might add multiple collision points to simulate a kind of rigid body

		// I can add some additional info here later
	};

#define HF_TO_BE_DELETED 0u

	class Hitbox			// Some kind of hitbox?
	{
	public:
		bool Flags[1] = { false };

		World_Object* Object = nullptr;

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox)
		{ return Collision_Info(); }

		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox)
		{ return Collision_Info(); }


	};

	//

	class AABB_Hitbox : public Hitbox	// The simplest hitbox- very easy
	{
		glm::vec3 A, B;					// Must remember to offset this by the position of the world-object

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox) override;
		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox) override;
	};

	std::vector<Hitbox*> Wrap_AABB_Hitboxes()
}


#endif