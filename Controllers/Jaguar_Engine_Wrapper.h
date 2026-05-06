#ifndef JAGUAR_ENGINE_WRAPPER
#define JAGUAR_ENGINE_WRAPPER

#include <chrono>

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Input_Handling.h"
#include "../OpenGL_Handling/Scene.h"
#include "../OpenGL_Handling/Render_Queue.h"

#include "Skeletal_Animation_Handling.h"

#include "../Controllers/Asset_Cache.h"

// #include "../Controllers/Lightmapping.h"
#include "../Lightmapping/Shadow_Map_Lightmapping.h"

#include "../Collision/Hitboxes.h"
#include "../Collision/Physics_Engine.h"

#include "Job_System.h"

namespace Jaguar
{
	float RNG();

	struct Jaguar_Engine
	{
		float Time;					// This is the engine's delta-time

		float Window_Width = 800.0f, Window_Height = 800.0f; // aspect ratio is width/height
		GLFWwindow* Window;			// OpenGL window object

		Input_Data User_Inputs;

		Job_System Job_Handler;

		Scene_Data Scene;

		Physics_Data Physics;		// This handles the collisions and physics of objects in the scene

		Render_Pipeline Pipeline;	// This is the pipeline for drawing regular scene objects such as buildings or characters
									// We'll use a different interface for particles etc
		Asset_Cache_Data Asset_Cache;


#ifndef JAGUAR_GAME_TYPENAME						// if user hasn't defined this?
	#define JAGUAR_GAME_TYPENAME void*				// just use some bs
#endif

		JAGUAR_GAME_TYPENAME Game_Info;				// This can be swapped out from game to game.
													// this might include global information about game state like player HP or achievements etc etc
	};

	void Handle_Deletions(Jaguar_Engine* Target_Engine);
}

#endif