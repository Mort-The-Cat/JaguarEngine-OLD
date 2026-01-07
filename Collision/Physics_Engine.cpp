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

			// Not much else to do! Could perhaps play sound effects if the object experienced a large force
		}
	};

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
					Engine->Physics.Collision_Impulse_Info.Collision_Info.push_back(Info);
				}
			}

		// After that, generate the impulses etc

		
	}

}