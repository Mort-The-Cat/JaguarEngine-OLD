#ifndef JAGUAR_PHYSICS_ENGINE
#define JAGUAR_PHYSICS_ENGINE

#include "Hitboxes.h"

namespace Jaguar
{
	// In this, I'll make some controllers that do basic collision detection for some simple physics

	struct Jaguar_Engine;

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
		glm::vec3 Velocity, Rotational_Velocity;	// Velocity and angular velocity (as an axis with magnitude of rotation)
		glm::vec3 Orientation_Up, Orientation;	// Up and forward vectors of the physics object (used for rotation etc)
		glm::vec3 Position;

		glm::vec3 Centre;	// Centre of mass of object

		glm::vec3 Force;	// Forces applied to object
		glm::vec3 Torque;	// Moments applied to object (as an axis and magnitude of rotation)
	};

	struct Impulse_Info
	{
		Physics_Object* Object;			// The object to which the force is applied
		std::vector<glm::vec3> Points;	// The position/s of the forces
		glm::vec3 Force;				// The force applied
	};

	class Physics_Object_Controller;// : public Controller;

	void Record_Collisions(Jaguar_Engine* Engine);

	struct Frame_Collision_Impulse_Data
	{
		std::vector<Collision_Info> Collision_Info;

		std::vector<Impulse_Info> Impulse_Info;
	};

	struct Physics_Data
	{
		std::vector<Hitbox*> Hitboxes; // These are all of the hitboxes in the scene. The Scene_Data structure handles the deallocation of these

		
		Frame_Collision_Impulse_Data Collision_Impulse_Info; // This is used by the physics engine to store all of the collisions and impulses in the scene

		// The tricky thing here is that physics objects may be comprised of multiple hitboxes
		// To facilitate this, perhaps we can handle them as multiple physics objects (joined by rigid connectors or otherwise)
		// or, wrap the multiple hitboxes into a new, special hitbox class.

		// To implement something like ragdolls, multiple physics objects could be used, 
		// and a force/displacement could be applied elastically to handle their attachment to other objects

		std::vector<Physics_Object*> Physics_Objects;
	};
}

#endif