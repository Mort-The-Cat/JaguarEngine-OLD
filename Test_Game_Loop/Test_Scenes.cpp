#include "../Controllers/Jaguar_Engine_Wrapper.h"

void Place_Animation_Objects(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;																		// sets active flag
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Person.dae").Buffer,			// Model
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Texture.png").Texture,	// Texture
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(																	// Controller
			Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Person.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Person.dae").Skeleton
		),
		glm::vec3(-1, -2, 5)																				// Position
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(
			Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Skeleton
		),
		glm::vec3(5, -2, 5)
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Murderer.dae").Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(
			Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Murderer.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Murderer.dae").Skeleton
		),
		glm::vec3(0, 0, -0.8)
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;																		// sets active flag
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Light_Source.dae").Buffer,			// Model
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,	// Texture
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Light_Source.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Light_Source.dae").Skeleton
		),
		glm::vec3(0.0f, 0.8f, 0.0f)																				// Position
	);

	/*Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;																		// sets active flag
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Buffer,			// Model
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,	// Texture
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Skeleton
		),
		glm::vec3(0.0f, 0.0f, 0.0f)																				// Position
	);
	Object->Orientation = glm::vec3(0.0f, 0.0f, 1.0f);*/

	Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Test_Drill.dae").Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles_Normal.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(
			Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Test_Drill.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Test_Drill.dae").Skeleton
		),
		glm::vec3(-4, 2, 0)
	);

	//

	Object = nullptr;
}

//

void Setup_Pipe_Vent_Scene(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_SkeletaL_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Engine->Scene.Lighting.Environment_Map.A = glm::vec3(-1.207f, -1.00f, -4.832f);	// Will redo these in a moment
	Engine->Scene.Lighting.Environment_Map.B = glm::vec3(1.02f, 1.067f, 1.0017f);

	Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.5f) * (Engine->Scene.Lighting.Environment_Map.A + Engine->Scene.Lighting.Environment_Map.B);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Walls.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		//Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Walls.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Floor.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Floor.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Metal.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal_Normal_Texture.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Pipe_Vent_Scene/Metal.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.5f, 0.501f, 0.587f);
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.75f);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-5.77f, 1.796f, 4.547f);
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.75f);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.0184f, 1.852f, -1.707f);
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.75f);
}

void Setup_Radiophobia_Level(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Engine->Scene.Lighting.Environment_Map.A = glm::vec3(-1.207f, -1.00f, -4.832f);	// Will redo these in a moment
	Engine->Scene.Lighting.Environment_Map.B = glm::vec3(1.02f, 1.067f, 1.0017f);

	Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.5f) * (Engine->Scene.Lighting.Environment_Map.A + Engine->Scene.Lighting.Environment_Map.B);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Red_Walls.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Red_Walls.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		), 
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Floor_Tiles.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Floor_Tiles.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Ceiling.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Ceiling_Texture.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Ceiling_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Ceiling.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	// Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Vents.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal_Normal_Texture.png").Texture,	// Normal map
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Vent_Duct.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Vent_Duct.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Vent_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Vent_Duct.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Pipes.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rust_Texture.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rust_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Rubble.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Wall.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Wall_3.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Toilet_Wall.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Toilet_Texture.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Lift.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Ceiling_Texture.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Ceiling_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Lift_Metal.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Metal_Normal_Texture.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Level_0/Plank.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Table1.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,
		std::vector<Jaguar::Hitbox*>{},
		nullptr
	);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(2.2065f, 1.2379f, -8.2745f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.079379f, 1.2379f, -7.7279f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-3.4216f, 1.2379f, -7.7722f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-3.2443f, 1.2379f, -5.3644f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(2.2065f, 1.2379f, -4.4131f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.46227f, 1.2379f, -2.6585f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(8.6766f, 1.2379f, -2.9271f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.7f, 0.4f, 0.4f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(6.4428f, 1.2987f, 2.2042f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.6f, 0.6f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(8.4532f, 1.4234f, 0.7256f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.6f, 0.6f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(10.083f, 1.4234f, 0.7256f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	//

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(8.6766f, 1.2379f, -2.9271f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(3.9671f, 1.75352f, 1.2634f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(2.3377f, 1.75352f, 1.2397f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.65372f, 1.7352f, 1.2397f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.65372f, 1.7352f, 3.0654f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(2.3377f, 1.7352f, 3.0654f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.4f, 0.3f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(3.9671f, 1.7352f, 3.0654f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	//

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(1.1312f, 1.3523f, 7.5782f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-0.54336f, 1.2184f, 10.531f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-3.2601f, 1.5461f, 15.883f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-0.54536f, 1.5019f, 15.793f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(2.1539f, 1.5741f, 15.808f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(5.1035f, 1.5589f, 15.808f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(5.3649f, 2.0938f, 19.494f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(8.1276f, 1.3849f, 21.05f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(5.3647f, 2.0938f, 21.536f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	//

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.4f, 0.4f, 0.6f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-2.259f, 1.2184f, 12.509f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	// I fucking hate this
	// I'll come up with a better way of adding lightsources to a scene 
	// (that isn't just hard-coding all of the push calls)
}

//

void Setup_AABB_Physics_Room(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Engine->Scene.Lighting.Environment_Map.A = glm::vec3(-1.207f, -1.00f, -4.832f);	// Will redo these in a moment
	Engine->Scene.Lighting.Environment_Map.B = glm::vec3(1.02f, 1.067f, 1.0017f);

	Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.5f) * (Engine->Scene.Lighting.Environment_Map.A + Engine->Scene.Lighting.Environment_Map.B);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Brick.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Brick.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Rubble.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Rubble.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Tile.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/AABB_Physics_Room/Tile.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	//

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.52395f, -0.34528f, -2.0792f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(3.961f, 3.5593f, -1.9381f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.5f, 0.7f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(4.008f, 2.116f, 2.6285f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;
}

void Setup_Cornell_Box(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	//Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.0f, 0.338f, -3.7411f);
	Engine->Scene.Lighting.Environment_Map.A = glm::vec3(-1.207f, -1.00f, -4.832f);
	Engine->Scene.Lighting.Environment_Map.B = glm::vec3(1.02f, 1.067f, 1.0017f);

	Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.5f) * (Engine->Scene.Lighting.Environment_Map.A + Engine->Scene.Lighting.Environment_Map.B);

	// Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.016530f, 0.519270f, -2.537436f);

	// Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(-0.170224, 0.023105, -3.891045);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Nice_Cornell_Box.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Cornell_Box.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Nice_Cornell_Box.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh),
		nullptr);

	/*Object = new Jaguar::World_Object();
	Object->Flags[MF_ACTIVE] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Skeletal_Animation_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Grey.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default.png").Texture,	// Normal map
		{},
		new Jaguar::Animator_Controller(
			Object,
			Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Animation,
			Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Door.dae").Skeleton
		),
		glm::vec3(-1.30557f, 0.85809f, -4.22892f)
	);*/

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.4f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.0f, 0.8f, 0.32f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;
}

void Setup_New_Test_Level(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.0f, 0.8f, 0.0f);
	Engine->Scene.Lighting.Environment_Map.A = glm::vec3(-2.76f, -0.02f, -2.774f);
	Engine->Scene.Lighting.Environment_Map.B = glm::vec3(2.76f, 5.53f, 2.7339f);

	// Engine->Scene.Lighting.Environment_Map.Origin = glm::vec3(0.5f) * (Engine->Scene.Lighting.Environment_Map.A + Engine->Scene.Lighting.Environment_Map.B);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Brick.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Brick.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Red_Walls.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
		{},
		nullptr
	);

	Object = new Jaguar::World_Object();
	Object->Flags[MF_SOLID] = true;
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Tiles.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Floor_Tiles.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
		Jaguar::Wrap_AABB_Hitboxes(
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Tiles.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh
		),
		nullptr
	);

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level/Rubble.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble.jpg").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Rubble_Normal.png").Texture,	// Normal map
		{},
		nullptr
	);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.75f, 0.4f, 0.75f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-1.0f, 1.1f, 4.3f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.4f, 0.4f, 0.75f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.0f, 2.1f, 0.0f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;
}