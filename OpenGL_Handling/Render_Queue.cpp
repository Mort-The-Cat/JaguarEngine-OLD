#ifndef JAGUAR_RENDER_QUEUE
#define JAGUAR_RENDER_QUEUE

#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include "../Collada_Loader/Collada_Loader.h"
#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../OpenGL_Handling/Scene.h"

#include "../Controllers/Skeletal_Animation_Handling.h"

//#include "../Controllers/Lightmapping.h"

#include "../Lightmapping/Shadow_Map_Lightmapping.h"

template<size_t Axes>
glm::vec<Axes, bool, glm::packed_highp> operator>(const glm::vec<Axes, float, glm::packed_highp>& Left, const glm::vec<Axes, float, glm::packed_highp>& Right)
{
	glm::vec<Axes, bool, glm::packed_highp> Output;
	for (size_t Index = 0; Index < Axes; Index++)
	{
		Output[Index] = Left[Index] > Right[Index];
	}

	return Output;
}

namespace Jaguar
{
	// Note that this likely will NOT include particle rendering as that's so fundamentally different to how we're rendering actors here

	Render_Queue* Get_Render_Queue(Render_Pipeline* Target_Pipeline, const Shader* Target_Shader)
	{
		return &Target_Pipeline->Render_Queues[Target_Pipeline->Queue_Table.at(Target_Shader->Program_ID)];
	}

	void Shader_Parse_Environment_Map(const Shader* Target_Shader, const Scene_Data* Scene)
	{
		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Environment_Cubemap"), 5);
		//glActiveTexture(GL_TEXTURE_CUBEMAP0 + 0);
		// glActiveTexture()
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Scene->Lighting.Environment_Map.Cubemap_Texture);

		// we also need to parse the bounds of the cubemap

		glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Cubemap_Origin"), 
			Scene->Lighting.Environment_Map.Origin.x, 
			Scene->Lighting.Environment_Map.Origin.y, 
			Scene->Lighting.Environment_Map.Origin.z);

		glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, "Cubemap_AABB"), 2, glm::value_ptr(Scene->Lighting.Environment_Map.A));
	}

	void Default_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene)
	{
		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Scene->Camera_Projection_Matrix));
	
		glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Camera_Position"), Scene->Camera_Position.x, Scene->Camera_Position.y, Scene->Camera_Position.z);

		Shader_Parse_Environment_Map(Target_Shader, Scene);
	}

	void Lightmapped_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene)
	{
		Default_Shader_Init_Function(Target_Shader, Scene);

		//glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Scene->Camera_Projection_Matrix));

		glUniform1f(glGetUniformLocation(Target_Shader->Program_ID, "Inverse_Lightmap_Size"), Scene->Lighting.Inverse_Lightmap_Scale);

#if TRIPLE_LIGHTMAPPING
		const char* Names[] = {
			"Lightmap_0_Texture",
			"Lightmap_1_Texture",
			"Lightmap_2_Texture"
		};

		for (size_t W = 0; W < 3; W++)
		{
			glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, Names[W]), 2 + W);
			glActiveTexture(GL_TEXTURE2 + W);
			glBindTexture(GL_TEXTURE_2D, Scene->Lighting.Lightmap_Textures[W].Texture_Buffer_ID);
		}
#else
		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Lightmap_Texture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Scene->Lighting.Lightmap_Texture.Texture_Buffer_ID);
#endif
	}

	/*void Lighting_Node_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		Default_Uniform_Assign_Function(Target_Shader, Object, Scene);

		const Lighting_Node* Node[3];
		Get_Nearest_Lighting_Nodes(&Scene->Lighting.Lighting_Nodes, Object->Position, Node);

		glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node"), 6, glm::value_ptr(Node[0]->Illumination[0]));
	}*/

	void Default_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		// This assumes that the orientation vectors are already normalized
		glm::mat4 Model_Matrix = Get_Model_Matrix(Object);

		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model_Matrix));

		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Albedo_Texture"), 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, Object->Albedo.Texture_Buffer_ID);

		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Normal_Texture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Object->Normal.Texture_Buffer_ID);
	}

	void Lighting_Node_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		Default_Uniform_Assign_Function(Target_Shader, Object, Scene); // Does all the regular stuff,

		const Lighting_Node* Node[8];
		Get_Nearest_Lighting_Node(&Scene->Lighting.Lighting_Nodes, Object->Position, Node);

		glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Uniform_0"), 6, glm::value_ptr(Node[0]->Illumination[0]));

		// Get the other 3 nodes based on 'adjacent indices'

		glm::vec3 Delta = (glm::vec3(2.0f) * glm::vec3(Object->Position > Node[0]->Position) - glm::vec3(1.0f)) / glm::vec3(Scene->Lighting.Lighting_Nodes.Size);

		Node[1] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[0]->Adjacent_Indices[Delta[0] > 0 ? 0 : 3]]; // get right or get left
		Node[2] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[0]->Adjacent_Indices[Delta[2] > 0 ? 2 : 5]]; // get forward or get back
		Node[3] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[1]->Adjacent_Indices[Delta[2] > 0 ? 2 : 5]]; // from left/right node, get forward or back

		Node[4] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[0]->Adjacent_Indices[Delta[1] > 0 ? 1 : 4]]; // get up/down
		Node[5] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[1]->Adjacent_Indices[Delta[1] > 0 ? 1 : 4]]; // get up/down
		Node[6] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[2]->Adjacent_Indices[Delta[1] > 0 ? 1 : 4]]; // get up/down
		Node[7] = &Scene->Lighting.Lighting_Nodes.Nodes[Node[3]->Adjacent_Indices[Delta[1] > 0 ? 1 : 4]]; // get up/down

		// Then just do the same for all other nodes but do up/down instead!

		for (size_t Index = 1; Index < 8; Index++)
			glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, ("Lighting_Node_Uniform_" + std::to_string(Index)).c_str()), 6, glm::value_ptr(Node[Index]->Illumination[0]));
		//glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Uniform_2"), 6, glm::value_ptr(Node[2]->Illumination[0]));
		//glUniform3fv(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Uniform_3"), 6, glm::value_ptr(Node[3]->Illumination[0]));

		glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Position"), Node[0]->Position.x, Node[0]->Position.y, Node[0]->Position.z);

		glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Deltas"), Delta.x, Delta.y, Delta.z);

	}

	void Skeletal_Animation_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		Lighting_Node_Uniform_Assign_Function(Target_Shader, Object, Scene); // Does all the regular stuff,

		// then also assigns skeletal animation data!!

		const Animator_Controller* Animator_Object = (Animator_Controller*)Object->Control;

		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Joint_Matrices"), 
			JOINT_BUFFER_COUNT, GL_FALSE, 
			glm::value_ptr(*Animator_Object->Joint_Buffer));

		//glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Positions[1]"), Node[1]->Position.x, Node[1]->Position.y, Node[1]->Position.z);
		//glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Positions[2]"), Node[2]->Position.x, Node[2]->Position.y, Node[2]->Position.z);
		//glUniform3f(glGetUniformLocation(Target_Shader->Program_ID, "Lighting_Node_Positions[3]"), Node[3]->Position.x, Node[3]->Position.y, Node[3]->Position.z);

		// This parses the joint buffer to the shader
	}

	void Clear_Render_Pipeline(Render_Pipeline* Target_Pipeline)
	{
		Target_Pipeline->Render_Queues.clear();
		Target_Pipeline->Queue_Table.clear();
	}

	void Push_Render_Pipeline_Queue(Render_Pipeline* Target_Pipeline, Shader Queue_Shader, void(*Shader_Init_Function)(const Shader*, const Scene_Data*), void(*Uniform_Assign_Function)(const Shader*, const World_Object*, const Scene_Data*))
	{
		Render_Queue Queue;
		Queue.Queue_Shader = Queue_Shader;
		Queue.Shader_Init_Function = Shader_Init_Function;
		Queue.Uniform_Assign_Function = Uniform_Assign_Function;

		Target_Pipeline->Queue_Table[Queue_Shader.Program_ID] = Target_Pipeline->Render_Queues.size();	// Tells engine which queue this shader corresponds to
		Target_Pipeline->Render_Queues.push_back(Queue);											// Adds queue to pipeline
	}

	void Draw_Render_Queue(const Render_Queue* Queue, const Jaguar_Engine* Engine)
	{
		Use_Shader(Queue->Queue_Shader); // Activates shader for *this* queue

		Queue->Shader_Init_Function(&Queue->Queue_Shader, &Engine->Scene); // This function sets lighting uniforms etc for this shader

		for (size_t W = 0; W < Queue->Objects.size(); W++)
		{
			Bind_Vertex_Buffer(reinterpret_cast<World_Object*>(Queue->Objects[W])->Mesh);

			Queue->Uniform_Assign_Function(&Queue->Queue_Shader, reinterpret_cast<World_Object*>(Queue->Objects[W]), &Engine->Scene); // Assigns object uniforms accordingly

			glDrawArrays(GL_TRIANGLES, 0, reinterpret_cast<World_Object*>(Queue->Objects[W])->Mesh.Vertex_Count);
		}
	}

	//void Draw_Render_Pipeline(const Render_Pipeline* Pipeline, const Scene_Data* Scene)
	void Draw_Render_Pipeline(const Jaguar_Engine* Engine)
	{
		for (size_t W = 0; W < Engine->Pipeline.Render_Queues.size(); W++)
			Engine->Pipeline.Render_Queues[W].Draw_Render_Queue_Function(&Engine->Pipeline.Render_Queues[W], Engine);
			//Draw_Render_Queue(&Engine->Pipeline.Render_Queues[W], &Engine->Scene);
	}
}

#endif