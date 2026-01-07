#include "Physics_Engine.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	class Physics_Object_Controller : public Controller
	{
	public:
		Physics_Object Physics;

		virtual Physics_Object* Get_Physics_Object() override
		{
			return &Physics;
		}

		virtual void Init(Jaguar_Engine* Engine) override
		{
			Engine->Physics.Physics_Objects.push_back(&Physics);
		}

		virtual void Control_Function(Jaguar_Engine* Engine) override
		{
			// This will apply the new transformations to the object

			Object->Position = Physics.Position;
			Object->Orientation = Physics.Orientation;
			Object->Orientation_Up = Physics.Orientation_Up;

			Physics.Force += glm::vec3(0.0f, Engine->Time * Physics.Mass * -4.905f, 0.0f);	
			// Apply Earth's weight-force to object

			// Note that we don't need to use the 'apply force' function here because it's uniform and no torque is produced from this alone

			// Not much else to do! Could perhaps play sound effects if the object experienced a large force
		}
	};

	glm::vec3 Velocity_At_Point(const Physics_Object* Physics, glm::vec3 Point)
	{
		return Physics->Velocity + glm::cross(Physics->Rotational_Velocity, Point - Physics->Position);
	}

	void Apply_Force_To_Physics_Object(Physics_Object* Physics, glm::vec3 Force, glm::vec3 Point)
	{
		Physics->Force += Force;	// Easy

		// Torque is slightly trickier...

		Physics->Torque += glm::cross(Force, Point - Physics->Position);
	}

	void Apply_Impulses(Jaguar_Engine* Engine, const Collision_Info& Collision)
	{
		for (size_t Index = 0; Index < Collision.Points.size(); Index++)
		{
			glm::vec3 A_Velocity = Velocity_At_Point(Collision.A->Object->Control->Get_Physics_Object(), Collision.Points[Index]);
			glm::vec3 B_Velocity;

			float A_Mass = Collision.A->Object->Control->Get_Physics_Object()->Mass, B_Mass;
			
			float A_Inv_Mass = 1.0f / A_Mass, B_Inv_Mass;

			float B_Elasticity, B_Friction;

			if (Collision.B->Object->Get_Physics_Object())
			{
				B_Velocity = Velocity_At_Point(Collision.B->Object->Control->Get_Physics_Object(), Collision.Points[Index]);

				B_Mass = Collision.B->Object->Control->Get_Physics_Object()->Mass;
				B_Inv_Mass = 1.0f / B_Mass;

				B_Elasticity = Collision.B->Object->Control->Get_Physics_Object()->Elasticity;
				B_Friction = Collision.B->Object->Control->Get_Physics_Object()->Friction;
			}
			else
			{
				B_Velocity = glm::vec3(0.0f);

				B_Elasticity = 0.9f;
				B_Friction = 0.8f;

				B_Inv_Mass = 0.0f;
			}

			//

			float Inv_Combined_Mass = 1.0f / (A_Inv_Mass + B_Inv_Mass);

			glm::vec3 Relative_Velocity = B_Velocity - A_Velocity;

			float Normal_Velocity = glm::dot(Relative_Velocity, Collision.Normal);

			glm::vec3 Tangential_Velocity = Relative_Velocity + Normal_Velocity * Collision.Normal;

			if (Normal_Velocity > 0.0f)
			{
				float Elasticity = Collision.A->Object->Control->Get_Physics_Object()->Elasticity * B_Elasticity;

				float Friction = Collision.A->Object->Control->Get_Physics_Object()->Friction * B_Friction;

				float Normal_Force_Magnitude = -(1.0f + Elasticity) * Normal_Velocity * Inv_Combined_Mass;

				glm::vec3 Force = Collision.Normal * Normal_Force_Magnitude + Friction * Tangential_Velocity * (1 - expf(Normal_Force_Magnitude));

				Apply_Force_To_Physics_Object(Collision.A->Object->Control->Get_Physics_Object(), Force, Collision.Points[Index]);

				if (Collision.B->Object->Get_Physics_Object())
					Apply_Force_To_Physics_Object(Collision.B->Object->Control->Get_Physics_Object(), -Force, Collision.Points[Index]);
			}
		}
	}

	void Record_Collisions(Jaguar_Engine* Engine)
	{
		// This collects all of the Collision_Info objects between physics-objects and static hitboxes 
		// as well as physics-objects and other physics-objects

		// The 'resolve collisions' function will create all of the impulse infos (as well as resolve the position deltas) and then apply them

		// After that, all of the scene's control functions will be run

		for (size_t Index = 0; Index < Engine->Physics.Physics_Objects.size(); Index++)
			for (size_t Other_Index = Index + 1; Other_Index < Engine->Physics.Hitboxes.size(); Other_Index++)
			{
				// Physics object against physics object AND non-physics hitboxes

				Collision_Info Info;

				Info = Engine->Physics.Hitboxes[Index]->Test_Collision(Engine->Physics.Hitboxes[Other_Index]);

				if (Info.Delta)	// If NOT zero
				{
					Engine->Physics.Collision_Info.push_back(Info);

					Apply_Impulses(Engine, Engine->Physics.Collision_Info.back());
				}
			}

		// After that, generate the impulses etc

		
	}

	void Resolve_Collisions(Jaguar_Engine* Engine)
	{
		// Apply position deltas and then 'step' all physics objects

		for (size_t Index = 0; Index < Engine->Physics.Collision_Info.size(); Index++)
		{
			// check if there's an object 'B' apply deltas to either one or both

			Physics_Object* B_Physics = Engine->Physics.Collision_Info[Index].B->Object->Get_Physics_Object();

			glm::vec3 Delta = Engine->Physics.Collision_Info[Index].Normal * Engine->Physics.Collision_Info[Index].Delta;

			if (B_Physics)
			{
				Delta *= 0.5f;

				Engine->Physics.Collision_Info[Index].B->Object->Get_Physics_Object()->Position -= Delta;
			}

			Engine->Physics.Collision_Info[Index].A->Object->Get_Physics_Object()->Position += Delta;
		}

		for (size_t Index = 0; Index < Engine->Physics.Physics_Objects.size(); Index++)
			Engine->Physics.Physics_Objects[Index]->Step(Engine->Time);						// pass the 'delta time' to the step function
	}
}