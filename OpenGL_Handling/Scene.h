#ifndef JAGUAR_SCENE_DECLARATIONS
#define JAGUAR_SCENE_DECLARATIONS

#include "OpenGL_Handling.h"
#include "Render_Queue.h"
#include "Texture_Uniform_Buffer.h"

#include "../Controllers/Lightmapping.h"

#include "../Controllers/Cubemap_Reflection_Generation.h"

namespace Jaguar
{
	struct Jaguar_Engine;
	typedef struct Jaguar_Engine Jaguar_Engine;

	class Physics_Object;

	class Controller // These objects control what a World_Object does
	{
	public:
		World_Object* Object;

		virtual Physics_Object* Get_Physics_Object() 
		{
			return nullptr;	
			
			// by default, Controllers don't have a physics object associated with them
		}

		virtual void Init(Jaguar_Engine* Engine) {}
		virtual void Control_Function(Jaguar_Engine* Engine) {}
	};

	class Hitbox;

	class World_Object	// All world-objects will be stored in larger "object pool" which will be responsible for their allocation/deallocation
	{					// This is such that a deleted object's reference can be removed from render queues, hitbox pools, and THEN will be deleted from the scene
#define MF_TO_BE_DELETED	0u
#define MF_ACTIVE			1u
#define MF_SOLID			2u
#define MF_PHYSICS_OBJECT	3u

	public:
		bool Flags[4] = { false, false, false, false }; // Will be used later

		glm::vec3 Position;
		glm::vec3 Orientation;		// Used to generate direction matrix
		glm::vec3 Orientation_Up;	// Used to generate direction matrix

		Controller* Control; // This handles object-related logic

		std::vector<Hitbox*> Collision;

		void* Uniform_Buffer;	// This is a pointer to any additional uniform buffer information (such as skeletal animations etc)

		Texture Albedo;			// Albedo colour texture
		Texture Normal;			// Normal map texture

		Vertex_Buffer Mesh;			// Bound and rendered (can be shared by multiple world objects, pulled from same asset pool)

		Physics_Object* Get_Physics_Object() 
		{
			if (Control)
				return Control->Get_Physics_Object();

			return nullptr;
		}
	};

	void Destroy_World_Object(World_Object* Target_Object);

	void Create_World_Object(Jaguar_Engine* Engine, World_Object* Object, Shader* Object_Shader,
		Vertex_Buffer Mesh_Buffer,
		Texture Albedo,
		Texture Normal,
		std::vector<Hitbox*> Collision,
		Controller* Control,
		glm::vec3 Position = glm::vec3(0.0f),
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 Orientation_Up = glm::vec3(0.0f, 1.0f, 0.0f)
		);

	glm::mat4 Get_Model_Matrix(const World_Object* Object);

	struct Lightsource;

	struct Lighting_Data
	{
#if TRIPLE_LIGHTMAPPING
		Texture Lightmap_Textures[3];
#else
		Texture Lightmap_Texture;
#endif

		Cubemap Environment_Map;

		Lighting_Node_Data Lighting_Nodes;

		float Inverse_Lightmap_Scale;

		std::vector<Lightsource*> Lightsources;
	};

	void Delete_Scene_Lightmap(Lighting_Data* Target_Lighting);

	struct Scene_Data	// The Scene_Data object is responsible for memory management of world objects as well as
	{					// lighting, hitboxes, and camera information for the scene
		std::vector<World_Object*> Objects;

		Lighting_Data Lighting;

		glm::mat4 Camera_Projection_Matrix;
		glm::vec3 Camera_Position;
	};

	void Handle_Scene_Deletions(Scene_Data* Target_Scene);
	void Delete_All(Scene_Data* Target_Scene);

	void Handle_Scene_Controllers(Jaguar_Engine* Engine); // This is all for now

	void Add_Scene_Object(Jaguar_Engine* Engine, World_Object* New_Object, Render_Pipeline* Target_Pipeline = nullptr, const Shader* Shader_Program = nullptr);
}

#endif