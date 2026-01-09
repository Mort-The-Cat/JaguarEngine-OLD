#ifndef JAGUAR_PHYSICS_ENGINE
#define JAGUAR_PHYSICS_ENGINE

#include "Hitboxes.h"
#include "../OpenGL_Handling/Scene.h"

namespace Jaguar
{
	// In this, I'll make some controllers that do basic collision detection for some simple physics

	struct Jaguar_Engine;

#define PF_TO_BE_DELETED 0u
#define PF_LOCK_ROTATION 1u

	void Axis_Angle_Rotate_Orientation(glm::vec3 Rotation_Vector, glm::vec3* Orientation, glm::vec3* Orientation_Up);

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

		float Elasticity = 0.5f;	// Fraction of kinetic energy is preserved
		float Friction = 0.75f;		// Friction coefficient

		glm::vec3 Velocity, Rotational_Velocity;	// Velocity and angular velocity (as an axis with magnitude of rotation)
		glm::vec3 Orientation_Up, Orientation;	// Up and forward vectors of the physics object (used for rotation etc)
		glm::vec3 Position;

		glm::vec3 Centre;	// Centre of mass of object

		glm::vec3 Force;	// Forces applied to object
		glm::vec3 Torque;	// Moments applied to object (as an axis and magnitude of rotation)

		void Step(float Time)
		{
			float Inv_Mass = 0.5f / Mass;

			Force *= Inv_Mass;					// convert from force -> acceleration/2
			Velocity += Force;						// Apply half of the force before ...
			Position += Time * Velocity;
			Velocity += Force;						// ... and after the movement (more accurate)
			Force = glm::vec3(0.0f);				// reset force
			
			if (Flags[PF_LOCK_ROTATION])
				Inv_Mass = 0.0f;

			Torque *= Inv_Mass;					// convert from torque -> angular_acceleration/2

			Rotational_Velocity += Torque;			// Apply half of the torque before ...
			if(
				Rotational_Velocity.x != 0 ||
				Rotational_Velocity.y != 0 ||
				Rotational_Velocity.z != 0
				)
			Axis_Angle_Rotate_Orientation(Rotational_Velocity * Time, &Orientation, &Orientation_Up);
			Rotational_Velocity += Torque;			// ... and after the rotation

			Orientation = glm::normalize(Orientation);
			Orientation_Up = glm::normalize(Orientation_Up);	
			// We just wanna make EXTRA sure that these are normalised 
			// because otherwise it could potentially fuck up the physics and visuals after a long enough time due to floating point error

			Torque = Force;							// reset torque
		}
	};

	class Physics_Object_Controller : public Controller
	{
	public:
		Physics_Object Physics;

		virtual Physics_Object* Get_Physics_Object() override;

		virtual void Init(Jaguar_Engine* Engine) override;

		virtual void Control_Function(Jaguar_Engine* Engine) override;
	};

	void Record_Collisions(Jaguar_Engine* Engine);
	void Resolve_Collisions(Jaguar_Engine* Engine);

	struct Physics_Data
	{
		std::vector<Hitbox*> Hitboxes; // These are all of the hitboxes in the scene. The Scene_Data structure handles the deallocation of these

		std::vector<Collision_Info> Collision_Info;
		
		// The tricky thing here is that physics objects may be comprised of multiple hitboxes
		// To facilitate this, perhaps we can handle them as multiple physics objects (joined by rigid connectors or otherwise)
		// or, wrap the multiple hitboxes into a new, special hitbox class.

		// To implement something like ragdolls, multiple physics objects could be used, 
		// and a force/displacement could be applied elastically to handle their attachment to other objects

		std::vector<Physics_Object*> Physics_Objects;
	};
}

#endif