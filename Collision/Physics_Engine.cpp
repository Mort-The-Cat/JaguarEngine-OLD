#include "Physics_Engine.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	void Axis_Angle_Rotate_Orientation(glm::vec3 Rotation_Vector, glm::vec3* Orientation, glm::vec3* Orientation_Up)
	{
		float Length = glm::length(Rotation_Vector);	// gets 'angle' from axis/angle vector

		Rotation_Vector *= 1.0f / Length;	// Normalises rotation vector to create normalised axis

		float Sin_Theta = sinf(Length);
		float Cos_Theta = cosf(Length);

		*Orientation = *Orientation * Cos_Theta + glm::cross(Rotation_Vector, *Orientation) * Sin_Theta;

		*Orientation_Up = *Orientation_Up * Cos_Theta + glm::cross(Rotation_Vector, *Orientation_Up) * Sin_Theta;
	}

	glm::vec3 Velocity_At_Point(const Physics_Object* Physics, glm::vec3 Point)
	{
		return Physics->Get_Velocity() + glm::cross(Physics->Get_Rotational_Velocity(), Point - Physics->Position);
	}

	glm::vec3 Get_Torque(Physics_Object* Physics, glm::vec3 Force, glm::vec3 Point)
	{
		glm::vec3 Axis = glm::normalize(glm::cross(Force, Point - Physics->Position));

		glm::vec3 Tangent = glm::normalize(glm::cross(Axis, Point - Physics->Position));

		float Line_Length = glm::length(Point - Physics->Position);

		return (glm::dot(Force, Tangent) / Line_Length) * Axis;
	}

	Physics_Object* Physics_Object_Controller::Get_Physics_Object()
	{
		return &Physics;
	}

	void Physics_Object_Controller::Init(Jaguar_Engine* Engine)
	{
		Physics.Position = Object->Position;
		Physics.Orientation = Object->Orientation;
		Physics.Orientation_Up = Object->Orientation_Up;

		Physics.Velocity = glm::vec3(0.5f) * (glm::vec3(RNG(), RNG(), RNG()) - glm::vec3(0.5f));

		Engine->Physics.Physics_Objects.push_back(&Physics);
	}

	void Update_Physics_Object_Collision(Controller* Control, Physics_Object* Physics)
	{
		Control->Object->Position = Physics->Position;
		Control->Object->Orientation = Physics->Orientation;
		Control->Object->Orientation_Up = Physics->Orientation_Up;

		for (size_t Index = 0; Index < Control->Object->Collision.size(); Index++)
			Control->Object->Collision[Index]->Update_Hitbox();
	}

	void Physics_Object_Controller::Control_Function(Jaguar_Engine* Engine)
	{
		// This will apply the new transformations to the object

		//Update_Physics_Object_Collision(this, &Physics);

		Physics.Force += glm::vec3(0.0f, Engine->Time * Physics.Mass * -4.0f, 0.0f);

		// Apply Earth's weight-force to object

		// Note that we don't need to use the 'apply force' function here because it's uniform and no torque is produced from this alone

		// Not much else to do! Could perhaps play sound effects if the object experienced a large force
	}

	void Record_Collisions(Jaguar_Engine* Engine)
	{
		Engine->Physics.Collision_Info.clear(); // Ensures that any previous collisions have been cleared

		for (size_t Index = 0; Index < Engine->Physics.Physics_Objects.size(); Index++)						// This needs to be performed before we start recording any collisions
		{
			Engine->Physics.Physics_Objects[Index]->Update_Movement_Vectors();
			Engine->Physics.Physics_Objects[Index]->Step(Engine->Time / Physics_Iterations);						// pass the 'delta time' to the step function

			Update_Physics_Object_Collision(Engine->Physics.Hitboxes[Index]->Object->Control, Engine->Physics.Physics_Objects[Index]);
		}

		// This collects all of the Collision_Info objects between physics-objects and static hitboxes 
		// as well as physics-objects and other physics-objects

		// The 'resolve collisions' function will create all of the impulse infos (as well as resolve the position deltas) and then apply them

		// After that, all of the scene's control functions will be run

		for (size_t Index = 0; Index < Engine->Physics.Physics_Objects.size(); Index++)
		{
			const std::vector<Hitbox*>& Nearby_Hitboxes = *Read_Blockmap(Engine->Physics.Blockmap, Engine->Physics.Physics_Objects[Index]->Position);

			// compare against nearby hitboxes

			for (size_t Other_Index = 0; Other_Index < Nearby_Hitboxes.size(); Other_Index++)
			{
				Collision_Info Info;

				Info = Engine->Physics.Hitboxes[Index]->Test_Collision(Nearby_Hitboxes[Other_Index]);

				if (Info.Delta > 0.0f)	// If NOT zero
				{
					Engine->Physics.Collision_Info.push_back(Info);
				}
			}

			// repeats for other physics objects...

			for (size_t Other_Index = Index + 1; Other_Index < Engine->Physics.Physics_Objects.size(); Other_Index++)
			{
				Collision_Info Info;

				Info = Engine->Physics.Hitboxes[Index]->Test_Collision(Engine->Physics.Hitboxes[Other_Index]);

				if (Info.Delta > 0.0f)	// If NOT zero
				{
					Engine->Physics.Collision_Info.push_back(Info);
				}
			}
		}

		/*for (size_t Index = 0; Index < Engine->Physics.Physics_Objects.size(); Index++)
		{
			for (size_t Other_Index = Index + 1; Other_Index < Engine->Physics.Hitboxes.size(); Other_Index++)
			{
				// Physics object against physics object AND non-physics hitboxes

				Collision_Info Info;

				Info = Engine->Physics.Hitboxes[Index]->Test_Collision(Engine->Physics.Hitboxes[Other_Index]);

				if (Info.Delta > 0.0f)	// If NOT zero
				{
					Engine->Physics.Collision_Info.push_back(Info);
				}
			}
		}*/
	}

	void Get_Force_And_Torque(
		glm::vec3& Sum_Force,
		size_t& Force_Count,
		glm::vec3& Min_A_Torque,
		glm::vec3& Max_A_Torque,

		glm::vec3& Min_B_Torque,
		glm::vec3& Max_B_Torque,
		glm::vec3 Point, const Collision_Info& Collision)
	{


		glm::vec3 A_Velocity = Velocity_At_Point(Collision.A->Object->Control->Get_Physics_Object(), Point);
		glm::vec3 B_Velocity;

		float A_Mass = Collision.A->Object->Control->Get_Physics_Object()->Mass, B_Mass;

		float A_Inv_Mass = 1.0f / A_Mass, B_Inv_Mass;

		float B_Elasticity, B_Friction;

		if (Collision.B->Object->Get_Physics_Object())
		{
			B_Velocity = Velocity_At_Point(Collision.B->Object->Control->Get_Physics_Object(), Point);

			B_Mass = Collision.B->Object->Control->Get_Physics_Object()->Mass;
			B_Inv_Mass = 1.0f / B_Mass;

			B_Elasticity = Collision.B->Object->Control->Get_Physics_Object()->Elasticity;
			B_Friction = Collision.B->Object->Control->Get_Physics_Object()->Friction;
		}
		else
		{
			B_Velocity = glm::vec3(0.0f);

			B_Elasticity = 0.15f;
			B_Friction = 0.4f;

			B_Inv_Mass = 0.0f;
		}

		//

		float Inv_Combined_Mass = 0.5f / (A_Inv_Mass + B_Inv_Mass);

		glm::vec3 Relative_Velocity = A_Velocity - B_Velocity;

		float Normal_Velocity = glm::dot(Relative_Velocity, -Collision.Normal);

		glm::vec3 Tangential_Velocity = Relative_Velocity + Normal_Velocity * Collision.Normal;

		if (Normal_Velocity > 0.0f)
		{
			float Elasticity = Collision.A->Object->Control->Get_Physics_Object()->Elasticity * B_Elasticity;

			float Friction = Collision.A->Object->Control->Get_Physics_Object()->Friction * B_Friction;

			float Normal_Force_Magnitude = -(1.0f + Elasticity) * Normal_Velocity * Inv_Combined_Mass;

			glm::vec3 Force = Collision.Normal * Normal_Force_Magnitude + Friction * Tangential_Velocity * (1.0f - expf(Normal_Force_Magnitude));

			// Apply_Force_To_Physics_Object(Collision.A->Object->Control->Get_Physics_Object(), -Force, Collision.Points[Index]);

			glm::vec3 Torque_A = Get_Torque(Collision.A->Object->Control->Get_Physics_Object(), -Force, Point);

			Sum_Force = glm::dot(Sum_Force, Sum_Force) > glm::dot(Force, Force) ? Sum_Force : Force; // glm::normalize(Force)* sqrtf(1.0f - glm::dot(Torque_A, Torque_A) / (glm::dot(Force, Force) * 10.0f));
			Force_Count++;

			Min_A_Torque = glm::min(Min_A_Torque, Torque_A);
			Max_A_Torque = glm::max(Max_A_Torque, Torque_A);

			if (Collision.B->Object->Get_Physics_Object())
			{
				glm::vec3 Torque_B = Get_Torque(Collision.B->Object->Control->Get_Physics_Object(), Force, Point);

				Min_B_Torque = glm::min(Min_B_Torque, Torque_B);
				Max_B_Torque = glm::max(Max_B_Torque, Torque_B);

				// Apply_Force_To_Physics_Object(Collision.B->Object->Control->Get_Physics_Object(), Force, Collision.Points[Index]);

				//Collision.B->Object->Control->Get_Physics_Object()->Step(Engine->Time);
			}
		}
	}

	void Apply_Impulses(Jaguar_Engine* Engine, const Collision_Info& Collision)
	{
		// This will get the min/max torque for both objects and for both sets of points
		// Note that we only want to check object B if it is indeed a physics object...

		glm::vec3 Min_A_A_Points_Torque = glm::vec3(9999999999999.0f);
		glm::vec3 Max_A_A_Points_Torque = -Min_A_A_Points_Torque;

		glm::vec3 Min_A_B_Points_Torque = Min_A_A_Points_Torque;
		glm::vec3 Max_A_B_Points_Torque = Max_A_A_Points_Torque;

		glm::vec3 Forces[2] = { glm::vec3(0.0f), glm::vec3(0.0f) };

		size_t Forces_Count[2] = { 0, 0 };


		glm::vec3 Min_B_A_Points_Torque = Min_A_A_Points_Torque;
		glm::vec3 Max_B_A_Points_Torque = Max_A_A_Points_Torque;

		glm::vec3 Min_B_B_Points_Torque = Min_B_A_Points_Torque;
		glm::vec3 Max_B_B_Points_Torque = Max_B_A_Points_Torque;

		for (size_t A_Index = 0; A_Index < Collision.A_Points.size(); A_Index++)
			Get_Force_And_Torque(Forces[0], Forces_Count[0], Min_A_A_Points_Torque, Max_A_A_Points_Torque, Min_B_A_Points_Torque, Max_B_A_Points_Torque, Collision.A_Points[A_Index], Collision);

		for (size_t B_Index = 0; B_Index < Collision.B_Points.size(); B_Index++)
			Get_Force_And_Torque(Forces[1], Forces_Count[1], Min_A_B_Points_Torque, Max_A_B_Points_Torque, Min_B_B_Points_Torque, Max_B_B_Points_Torque, Collision.B_Points[B_Index], Collision);

		if (!Forces_Count[0] && !Forces_Count[1])
			return;

		// Applies the correct force and the correct torque...

		// apply force with *fewer* overlaps

		// apply torque that is closest to -Get_Rotational_Velocity() * Mass

		glm::vec3 Min_Torque;// = glm::max(Min_A_A_Points_Torque, Min_A_B_Points_Torque);
		glm::vec3 Max_Torque;// = glm::min(Max_A_A_Points_Torque, Max_A_B_Points_Torque);

		size_t Force_Index;

		if (!Forces_Count[0])
		{
			Min_Torque = Min_A_B_Points_Torque;
			Max_Torque = Max_A_B_Points_Torque;

			Force_Index = 1;
		}
		else if (!Forces_Count[1])
		{
			Min_Torque = Min_A_A_Points_Torque;
			Max_Torque = Max_A_A_Points_Torque;

			Force_Index = 0;
		}
		else
		{
			Min_Torque = glm::max(Min_A_A_Points_Torque, Min_A_B_Points_Torque);
			Max_Torque = glm::min(Max_A_A_Points_Torque, Max_A_B_Points_Torque);

			Forces_Count[0] = 1;
			Forces_Count[1] = 1;

			Force_Index = (Forces_Count[0] * glm::length(Forces[1])) < (glm::length(Forces[0]) * Forces_Count[1]);
		}

		//size_t 

		// Force_Index = glm::length(Forces[1]) < glm::length(Forces[0]) || !Forces_Count[0];

		glm::vec3 Desired_Torque = -Collision.A->Object->Get_Physics_Object()->Get_Rotational_Velocity() * Collision.A->Object->Get_Physics_Object()->Mass;

		glm::vec3 Delta_Torque = glm::max(Min_Torque, glm::min(Max_Torque, Desired_Torque));

		Collision.A->Object->Get_Physics_Object()->Torque += Delta_Torque;

		Forces[Force_Index] *= 1.0f / (float)Forces_Count[Force_Index];

		Collision.A->Object->Get_Physics_Object()->Force -= Forces[Force_Index];

		if (Collision.B->Object->Get_Physics_Object() != nullptr)
		{
			// apply force and torque here as well !!

			if (!Forces_Count[0])
			{
				Min_Torque = Min_B_B_Points_Torque;
				Max_Torque = Max_B_B_Points_Torque;
			}
			else if (!Forces_Count[1])
			{
				Min_Torque = Min_B_A_Points_Torque;
				Max_Torque = Max_B_A_Points_Torque;
			}
			else
			{
				Min_Torque = glm::max(Min_B_A_Points_Torque, Min_B_B_Points_Torque);
				Max_Torque = glm::min(Max_B_A_Points_Torque, Max_B_B_Points_Torque);

				// Force_Index = (Forces_Count[0] * glm::length(Forces[1][1])) > (glm::length(Forces[0][1]) * Forces_Count[1]);
			}

			// Forces[Force_Index][1] *= 1.0f / (float)Forces_Count[Force_Index];

			// Collision.B->Object->Get_Physics_Object()->Force += Forces[Force_Index][1];

			Desired_Torque = -Collision.B->Object->Get_Physics_Object()->Get_Rotational_Velocity() * Collision.B->Object->Get_Physics_Object()->Mass;

			Delta_Torque = glm::max(Min_Torque, glm::min(Max_Torque, Desired_Torque));

			Collision.B->Object->Get_Physics_Object()->Torque += Delta_Torque;

			//

			Collision.B->Object->Get_Physics_Object()->Force += Forces[Force_Index];
		}
		else
			Collision.A->Object->Control->Get_Physics_Object()->Update_Movement_Vectors();
	}

	void Resolve_Collisions(Jaguar_Engine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Physics.Collision_Info.size(); Index++)
		{
			Apply_Impulses(Engine, Engine->Physics.Collision_Info[Index]);

			Physics_Object* B_Physics = Engine->Physics.Collision_Info[Index].B->Object->Get_Physics_Object();

			glm::vec3 Delta = Engine->Physics.Collision_Info[Index].Normal * (Engine->Physics.Collision_Info[Index].Delta);

			if (B_Physics)
			{
				Delta *= 0.5f;

				B_Physics->Position -= Delta;
			}

			Engine->Physics.Collision_Info[Index].A->Object->Get_Physics_Object()->Position += Delta;
		}

		// We'll multithread this stuff later
	}
}