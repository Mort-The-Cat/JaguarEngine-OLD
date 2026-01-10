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
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.52395f, -0.33528f, -2.0792f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.25f, 0.25f, 0.35f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(3.961f, 3.5593f, -1.9381f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.25f, 0.25f, 0.35f);
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
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Nice_Cornell_Box.dae").Mesh),
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