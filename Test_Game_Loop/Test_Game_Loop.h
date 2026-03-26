#ifndef TEST_GAME_LOOP_DEFINED
#define TEST_GAME_LOOP_DEFINED

//#include "../OpenGL_Handling/OpenGL_Handling.h"
//#include "../OpenGL_Handling/Scene.h"
//#include "../OpenGL_Handling/Render_Queue.h"
//#include "../OpenGL_Handling/Input_Handling.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

enum Controls
{
	Forwards = 0,
	Backwards,
	Left,
	Right,
	Up,
	Down,

	Look_Left,
	Look_Right,

	Look_Up, 
	Look_Down,

	Interact,
	Drop,

	Number_Of_Keys
};

void Set_Input_Keycodes(Jaguar::Input_Data* Inputs)
{
	Inputs->Keys.resize(Controls::Number_Of_Keys);
	Inputs->Keys[Controls::Forwards].Keycode		=		GLFW_KEY_W;
	Inputs->Keys[Controls::Backwards].Keycode		=		GLFW_KEY_S;
	Inputs->Keys[Controls::Left].Keycode			=		GLFW_KEY_A;
	Inputs->Keys[Controls::Right].Keycode			=		GLFW_KEY_D;


	Inputs->Keys[Controls::Up].Keycode				=		GLFW_KEY_SPACE;
	Inputs->Keys[Controls::Down].Keycode			=		GLFW_KEY_LEFT_SHIFT;

	Inputs->Keys[Controls::Look_Left].Keycode		=		GLFW_KEY_LEFT;
	Inputs->Keys[Controls::Look_Right].Keycode		=		GLFW_KEY_RIGHT;
	Inputs->Keys[Controls::Look_Up].Keycode			=		GLFW_KEY_UP;
	Inputs->Keys[Controls::Look_Down].Keycode		=		GLFW_KEY_DOWN;

	Inputs->Keys[Controls::Interact].Keycode		=		GLFW_KEY_F;
	Inputs->Keys[Controls::Drop].Keycode			=		GLFW_KEY_G;
}

glm::vec3 Get_Direction_Vector(float X_Direction)
{
	float Z = cos(X_Direction), X = sin(X_Direction);

	return glm::vec3(X, 0, Z);
}

void Place_Lighting_Node_Visuals(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Node_Shader)
{
	Jaguar::World_Object* Object;

	/*Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Node_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Light_Source.dae").Buffer,			// Doesn't matter what mesh hint we give
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		{},
		nullptr,
		Engine->Scene.Lighting.Lighting_Nodes.Nodes.back().Position
	);*/

	for (size_t Node = 0; Node < Engine->Scene.Lighting.Lighting_Nodes.Nodes.size(); Node++)
	{
		Object = new Jaguar::World_Object();
		Jaguar::Create_World_Object(Engine, Object, &Node_Shader,
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Sphere_Simple.dae").Buffer,			// Doesn't matter what mesh hint we give
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
			{},
			nullptr,
			Engine->Scene.Lighting.Lighting_Nodes.Nodes[Node].Position
		);
	}

	/*for (size_t Light = 0; Light < Engine->Scene.Lighting.Lightsources.size(); Light++)
	{
		Object = new Jaguar::World_Object();
		Jaguar::Create_World_Object(Engine, Object, &Node_Shader,
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Sphere_Simple.dae").Buffer,			// Doesn't matter what mesh hint we give
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
			{},
			nullptr,
			Engine->Scene.Lighting.Lightsources[Light]->Position
		);
	}*/
}

void Shoot_Physics_Object(Jaguar::Jaguar_Engine* Engine)
{
	Jaguar::World_Object* Object;

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;																	// sets active flag
	Object->Flags[MF_SOLID] = true;
	Object->Flags[MF_PHYSICS_OBJECT] = true;
	Jaguar::Create_World_Object(Engine, Object, &Engine->Pipeline.Render_Queues.back().Queue_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Slope_Shape.dae").Buffer,			// Model
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,	// Texture
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Test_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_Mesh_Hitbox(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Slope_Shape.dae").Mesh
		),
		new Jaguar::Physics_Object_Controller(),
		glm::vec3(0.0f, 0.8f, 0.0f)	+ glm::vec3(Jaguar::RNG(), Jaguar::RNG(), Jaguar::RNG())	// Position
	);
}

void Test_Engine_Loop(Jaguar::Jaguar_Engine* Engine)
{
	float Camera_X_Direction = 0;
	float Camera_Y_Direction = 0;
	glm::vec3 Player_Position = glm::vec3(1.0f, 0.8f, 1.0f);

	auto Previous_Time = std::chrono::high_resolution_clock::now();

	auto Current_Time = Previous_Time;

	glViewport(0, 0, 800, 800);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	float Drop_Time = 0.0f;

	while (!glfwWindowShouldClose(Engine->Window))
	{
		// Basic graphics loop

		Current_Time = std::chrono::high_resolution_clock::now();

		Engine->Time = std::chrono::duration<float>(Current_Time - Previous_Time).count();

		if (Engine->Time > 0.2f) // If it's slower than 5FPS, clamp it to zero because the game isn't running like it's supposed to (or it's somehow paused/frozen)
			Engine->Time = 0.0f;

		glClearColor(0.3, 0.3, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Jaguar::Get_User_Inputs(Engine->Window, &Engine->User_Inputs);

		glm::mat4 View_Matrix = glm::mat4(1.0f);

		if (Drop_Time > 0.5f)
		{
			if (Engine->User_Inputs.Keys[Controls::Drop].Pressed)
			{
				Shoot_Physics_Object(Engine);
				Drop_Time = 0.0f;
			}
		}
		else
			Drop_Time += Engine->Time;

		if (Engine->User_Inputs.Keys[Controls::Interact].Pressed) // && Engine->User_Inputs.Keys[Controls::Interact].Changed) // if newly pressed
		{
			//Engine->Scene.Lighting.Lighting_Nodes.Nodes.push_back(Jaguar::Lighting_Node(Player_Position));

			//Place_Lighting_Node_Visuals(Engine, Engine->Pipeline.Render_Queues.back().Queue_Shader);	// This is a bit of a botch, but it's fine for testing!
			
			Engine->Pipeline.Render_Queues[1].Objects.back()->Position = Player_Position - glm::vec3(0, 0.3, 0) + glm::vec3(0.5f) * Get_Direction_Vector(Camera_X_Direction + 0.25f);
			Engine->Pipeline.Render_Queues[1].Objects.back()->Orientation = Get_Direction_Vector(Camera_X_Direction);

			// printf("Position: %f %f %f\n", Player_Position.x, Player_Position.y, Player_Position.z);

			// This code right now is fairly awful but it's just a test obviously
		}

		if (Engine->User_Inputs.Keys[Controls::Forwards].Pressed)
			Player_Position += 2 * Engine->Time * Get_Direction_Vector(Camera_X_Direction);

		if (Engine->User_Inputs.Keys[Controls::Backwards].Pressed)
			Player_Position -= 2 * Engine->Time * Get_Direction_Vector(Camera_X_Direction);

		if (Engine->User_Inputs.Keys[Controls::Up].Pressed)
			Player_Position.y += 2 * Engine->Time;

		if (Engine->User_Inputs.Keys[Controls::Down].Pressed)
			Player_Position.y -= 2 * Engine->Time;

		if (Engine->User_Inputs.Keys[Controls::Left].Pressed)
			Player_Position -= 2 * Engine->Time * Get_Direction_Vector(Camera_X_Direction + 3.14159 / 2);

		if (Engine->User_Inputs.Keys[Controls::Right].Pressed)
			Player_Position += 2 * Engine->Time * Get_Direction_Vector(Camera_X_Direction + 3.14159 / 2);

		if (Engine->User_Inputs.Keys[Controls::Look_Left].Pressed)
			Camera_X_Direction -= Engine->Time * 3;

		if (Engine->User_Inputs.Keys[Controls::Look_Right].Pressed)
			Camera_X_Direction += Engine->Time * 3;

		if (Engine->User_Inputs.Keys[Controls::Look_Up].Pressed)
			Camera_Y_Direction += Engine->Time * 3;

		if (Engine->User_Inputs.Keys[Controls::Look_Down].Pressed)
			Camera_Y_Direction -= Engine->Time * 3;

		{
			// Test handle some player inputs!

			View_Matrix = glm::rotate(Camera_Y_Direction, glm::vec3(-1, 0, 0)) *
				glm::rotate(Camera_X_Direction, glm::vec3(0, 1, 0)) *
				glm::mat4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, -1, 0,
					0, 0, 0, 1
				);				// The VIEW matrix is designed for a left-handed coordinate system (i.e. +z = forwards, +y = up, +x = right)

			View_Matrix = glm::translate(View_Matrix, -Player_Position);
		}

		Engine->Scene.Camera_Projection_Matrix = glm::perspective(glm::radians(85.0f), 1.0f, 0.01f, 100.0f) // Sets nice camera projection matrix
			* View_Matrix;

		Engine->Scene.Camera_Position = Player_Position;

		Jaguar::Record_Collisions(Engine);
		//for(size_t I = 0; I < 1; I++)
			Jaguar::Resolve_Collisions(Engine);
		//Jaguar::Step_Physics(Engine);

		Jaguar::Handle_Scene_Controllers(Engine);

		Jaguar::Draw_Render_Pipeline(&Engine->Pipeline, &Engine->Scene);

		glfwSwapBuffers(Engine->Window);

		Previous_Time = Current_Time;

		glfwPollEvents();
	}
}

void Place_Animation_Objects(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);
void Setup_New_Test_Level(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);
void Setup_Cornell_Box(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);
void Setup_Radiophobia_Level(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);
void Setup_Pipe_Vent_Scene(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);

void Setup_AABB_Physics_Room(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader);

void Run_Scene(Jaguar::Jaguar_Engine* Engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	Engine->Window = glfwCreateWindow(800, 800, "Test window!", NULL, NULL);

	glfwSwapInterval(0);

	if (Engine->Window == NULL)
		Throw_Error(" >> Failed to create OpenGL window!\n\t%s\n");

	glfwMakeContextCurrent(Engine->Window);

	if (!gladLoadGL())
		Throw_Error(" >> Failed to initialise glad!\n\t%s\n");

	Set_Input_Keycodes(&Engine->User_Inputs);

	Jaguar::Initialise_Job_System(&Engine->Job_Handler, 4); // initialise 7 worker threads

	//

	Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Collada_Loader/Sphere.dae", 0); // The sphere model must be loaded for the lightmapping to use as the 'light'
	
	Jaguar::Shader Test_Shader;
#if TRIPLE_LIGHTMAPPING
	Jaguar::Create_Shader("Shaders/Triple_Lightmapped_Shader.frag", "Shaders/Triple_Lightmapped_Shader.vert", &Test_Shader, "Shaders/Test_TBN_Geometry.geom"); // This shader is used if we're using triple lightmapping
#else
	Jaguar::Create_Shader("Shaders/Lightmapped_Shader.frag", "Shaders/Test_Shader.vert", &Test_Shader);
#endif

	Jaguar::Shader Test_Skeletal_Animation_Shader;
	Jaguar::Create_Shader("Shaders/Dynamic_Shader.frag", "Shaders/Test_Skeletal_Animation.vert", &Test_Skeletal_Animation_Shader, "Shaders/Dynamic_TBN_Geometry.geom");

	Jaguar::Shader Lighting_Node_Shader;
	Jaguar::Create_Shader("Shaders/Test_Shader.frag", "Shaders/Test_Shader.vert", &Lighting_Node_Shader);

	Jaguar::Shader Test_Physics_Object_Shader;
	Jaguar::Create_Shader("Shaders/Dynamic_Shader.frag", "Shaders/Simple_Dynamic_Shader.vert", &Test_Physics_Object_Shader, "Shaders/Dynamic_TBN_Geometry.geom");

	Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Test_Shader,
		Jaguar::Lightmapped_Shader_Init_Function, Jaguar::Default_Uniform_Assign_Function);

	Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Test_Skeletal_Animation_Shader, 
		Jaguar::Default_Shader_Init_Function, Jaguar::Skeletal_Animation_Uniform_Assign_Function);

	Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Test_Physics_Object_Shader,
		Jaguar::Default_Shader_Init_Function, Jaguar::Lighting_Node_Uniform_Assign_Function);

	//Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Lighting_Node_Shader,
	//	Jaguar::Default_Shader_Init_Function, Jaguar::Default_Uniform_Assign_Function);

	std::string Lightmap_Directory = "Test_Game_Loop/Lightmaps/Pipe_Vent";

	//Setup_Cornell_Box(Engine, Test_Shader, Test_Skeletal_Animation_Shader);
	//Setup_New_Test_Level(Engine, Test_Shader, Test_Skeletal_Animation_Shader);
	//Setup_AABB_Physics_Room(Engine, Test_Shader, Test_Skeletal_Animation_Shader);
	//Setup_Radiophobia_Level(Engine, Test_Shader, Test_Skeletal_Animation_Shader);
	Setup_Pipe_Vent_Scene(Engine, Test_Shader, Test_Skeletal_Animation_Shader);

	Place_Animation_Objects(Engine, Test_Shader, Test_Skeletal_Animation_Shader);

	//if constexpr (false)
	{
		Jaguar::Lightmap_Chart Lightmap;			// This will be generated during light-baking
		Jaguar::Init_Lightmap_Chart(&Lightmap);

		Jaguar::Push_Queue_Lightmap_Chart(Engine, Jaguar::Get_Render_Queue(&Engine->Pipeline, &Test_Shader), &Lightmap);

		Jaguar::Assemble_Lightmap_Chart(Engine, &Lightmap, (Lightmap_Directory + ".lmc").c_str());

		// Jaguar::Get_Lighting_Nodes_From_File((Lightmap_Directory + ".ln").c_str(), Engine->Scene.Lighting.Lighting_Nodes);

		Jaguar::Flood_Fill_Lighting_Nodes(&Lightmap, Engine->Scene.Lighting.Lightsources[0]->Position, 0.125f, &Engine->Scene.Lighting);

		//Engine->Scene.Lighting.Lighting_Nodes.Nodes.push_back(Jaguar::Lighting_Node(glm::vec3(0.0f, 0.8f, -0.8f)));

		Jaguar::Create_Lightmap3_From_Chart(Engine, &Lightmap, (Lightmap_Directory + ".lux").c_str());

		Jaguar::Write_Lighting_Nodes_To_File((Lightmap_Directory + ".ln").c_str(), Engine->Scene.Lighting.Lighting_Nodes);
	}
	//else
	{
		std::vector<Jaguar::Baked_Lightmap_Chart> Lightmap_Charts;	// This is used when we want to load the lightmap chart instead of generating it
		Jaguar::Get_Lightmap_Chart_From_File((Lightmap_Directory + ".lmc").c_str(), Lightmap_Charts, &Engine->Asset_Cache);
		Jaguar::Apply_Baked_Lightmap_Chart(Engine, Lightmap_Charts);
		Jaguar::Get_Lightmap3_From_File((Lightmap_Directory + ".lux.opz").c_str(), &Engine->Scene.Lighting, true);

		Jaguar::Get_Lighting_Nodes_From_File((Lightmap_Directory + ".ln").c_str(), Engine->Scene.Lighting.Lighting_Nodes);
		
		//glm::vec3(-0.006598f, 1.049228f, 3.835901f);

		// Jaguar::Flood_Fill_Lighting_Nodes(&Lightmap, glm::vec3(1.0f, 0.0f, 1.0f), 0.5f, &Engine->Scene.Lighting);

		Jaguar::Generate_Cubemap(Engine, &Engine->Scene.Lighting.Environment_Map);

		 // Place_Lighting_Node_Visuals(Engine, Lighting_Node_Shader);

		Test_Engine_Loop(Engine);

		//

		// Jaguar::Write_Lighting_Nodes_To_File((Lightmap_Directory + ".ln").c_str(), Engine->Scene.Lighting.Lighting_Nodes);
	}

	Jaguar::Terminate_Job_System(&Engine->Job_Handler);

	Jaguar::Delete_All(&Engine->Scene);		// Sets all scene objects for deletion
	Jaguar::Handle_Deletions(Engine);		// Handles deletion of scene objects (in actual game, perform in game-loop)

	Jaguar::Destroy_Shader(&Test_Shader);
	Jaguar::Destroy_Shader(&Test_Skeletal_Animation_Shader);

	Jaguar::Clear_Render_Pipeline(&Engine->Pipeline);

	Jaguar::Delete_All_Mesh_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Texture_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Skeleton_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Animation_Cache(&Engine->Asset_Cache);

	Jaguar::Delete_Scene_Lightmap(&Engine->Scene.Lighting);

	glfwTerminate();
}

#endif