
#include "Hitboxes.h"

namespace Jaguar
{
	// In this, I'll make some controllers that do basic collision detection for some simple physics

#define PF_TO_BE_DELETED 0u
#define PF_LOCK_ROTATION 1u

	class Physics_Object
	{
	public:
		// How to handle this? 

		// I won't include any pointers to world_objects I think
		// I think the parent controller class or whatever should just maintain a pointer 
		// to this object and update its position etc

		bool Flags[2] = {
			false, false
		};

		float Mass = 1.0f;
		glm::vec3 Velocity, Rotational_Velocity;
		glm::vec3 Orientation_Up, Orientation;	// Up and forward vectors of the physics object (used for rotation etc)
		glm::vec3 Position;

		glm::vec3 Centre;	// Centre of mass of object
	};

	struct Physics_Data
	{
		std::vector<Hitbox*> Hitboxes; // These are all of the hitboxes in the scene. The Scene_Data structure handles the deallocation of these

		// The tricky thing here is that physics objects may be comprised of multiple hitboxes
		// To facilitate this, perhaps we can handle them as multiple physics objects (joined by rigid connectors or otherwise)
		// or, wrap the multiple hitboxes into a new, special hitbox class.

		// To implement something like ragdolls, multiple physics objects could be used, 
		// and a force/displacement could be applied elastically to handle their attachment to other objects

		std::vector<Physics_Object*> Physics_Objects;
	};
}