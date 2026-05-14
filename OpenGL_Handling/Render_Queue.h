#ifndef JAGUAR_RENDER_QUEUE_DECLARATIONS
#define JAGUAR_RENDER_QUEUE_DECLARATIONS

#include "OpenGL_Handling.h"

namespace Jaguar
{
	class World_Object;
	class Scene_Data;
	
	struct Render_Queue;

	struct Jaguar_Engine;

	void Draw_Render_Queue(const Render_Queue* Queue, const Jaguar_Engine* Engine); // This should simply draw the render queue, not do anything to it

	struct Render_Queue
	{
		Shader Queue_Shader;
		void(*Shader_Init_Function)(const Shader* Target_Shader, const Scene_Data* Scene);					// Used to init lighting uniforms etc etc
		void(*Uniform_Assign_Function)(const Shader* Target_Shader, const World_Object* Target_Object, const Scene_Data*);	// Used to init object uniforms (basic matrices etc)
		void(*Draw_Render_Queue_Function)(const Render_Queue* Queue, const Jaguar_Engine* Engine) = Draw_Render_Queue;		// might need additional engine data ?
		std::vector<void*> Objects;
		//
		// std::vector<World_Object*> Objects; // This is the queue of objects that need to be rendered
	};

	struct Render_Pipeline // Holds all of the render queues etc
	{
		std::vector<Render_Queue> Render_Queues;

		std::map<GLuint, size_t> Queue_Table; // Finds the queue based on the shader requested
	};

	struct Scene_Data;

	Render_Queue* Get_Render_Queue(Render_Pipeline* Target_Pipeline, const Shader* Target_Shader);

	void Clear_Render_Pipeline(Render_Pipeline* Target_Pipeline);

	void Default_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene);
	void Default_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data*);
	void Lighting_Node_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene);

	void Push_Render_Pipeline_Queue(Render_Pipeline* Target_Pipeline, Shader Queue_Shader, void(*Shader_Init_Function)(const Shader*, const Scene_Data*) = Default_Shader_Init_Function, void(*Uniform_Assign_Function)(const Shader*, const World_Object*, const Scene_Data*) = Default_Uniform_Assign_Function);

	void Draw_Render_Pipeline(const Jaguar_Engine* Engine); // This should simply draw the render pipeline, not do anything to it
}

#endif