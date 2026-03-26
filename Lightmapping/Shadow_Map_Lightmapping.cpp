#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include "Shadow_Map_Lightmapping.h"

namespace Jaguar
{
	void Generate_Bounced_Light_Lightsources(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, glm::vec3* Lightmap_Texture_Data3[3], std::vector<Lightsource*>& Target_Lightsources);
	void Accumulate_Lighting_Node_Lights(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const std::vector<Lightsource*>& Lightsources, Lighting_Node* Node);

	struct Lightmap_Cubemap
	{
		unsigned int Shadow_Cubemap;
		glm::vec3 Position;
	};

	struct Shadow_Caster
	{
		// This will handle the shadow-casting information

		unsigned int Shadow_Framebuffer;

		const unsigned int Shadow_Map_Width = 2048u, Shadow_Map_Height = 2048u;		// I'll use 1024 because 256 is too low-res

		Shader Shadow_Object_Shader;

		glm::mat4 Shadow_Projection_Matrix[6];

		const float Shadow_Map_Far_Plane = 100.0f;
	};

	struct Lightmap_Buffer_Data
	{
		glm::vec3* Pixel_Data[3];

		glm::vec3* Previous_Pixel_Data[3];
	
		unsigned int Lightmap_Buffer[3];
		Shader Lightmap_Write_Shader;

		unsigned int Lightmap_Framebuffer[3];
	};

	void Init_Lightmap_Buffer_Data(Lightmap_Buffer_Data& Lightmap, const Lightmap_Chart* Target_Chart)
	{
		size_t Pixel_Count = Target_Chart->Sidelength * Target_Chart->Sidelength;

		glGenTextures(3, Lightmap.Lightmap_Buffer);

		glGenFramebuffers(3, Lightmap.Lightmap_Framebuffer);

		for (size_t Map = 0; Map < 3; Map++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Lightmap.Lightmap_Framebuffer[Map]);

			Lightmap.Pixel_Data[Map] = new glm::vec3[Pixel_Count];
			Lightmap.Previous_Pixel_Data[Map] = new glm::vec3[Pixel_Count];

			for (size_t Pixel = 0; Pixel < Pixel_Count; Pixel++)
				Lightmap.Pixel_Data[Map][Pixel] = glm::vec3(0.0f);		// Is full-black by default

			memcpy(Lightmap.Previous_Pixel_Data[Map], Lightmap.Pixel_Data[Map], Pixel_Count * sizeof(glm::vec3));

			glBindTexture(GL_TEXTURE_2D, Lightmap.Lightmap_Buffer[Map]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Target_Chart->Sidelength, Target_Chart->Sidelength, 0, GL_RGB, GL_FLOAT, Lightmap.Pixel_Data[Map]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Lightmap.Lightmap_Buffer[Map], 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cout << " >> Framebuffer is not complete!" << std::endl;
			}
		}

		Create_Shader("Lightmapping/Write_Lightmap.frag", "Lightmapping/Write_Lightmap.vert", &Lightmap.Lightmap_Write_Shader, nullptr);
	}

	void Destroy_Lightmap_Buffer_Data(Lightmap_Buffer_Data& Lightmap)
	{
		Destroy_Shader(&Lightmap.Lightmap_Write_Shader);
		delete Lightmap.Pixel_Data[0];
		delete Lightmap.Pixel_Data[1];
		delete Lightmap.Pixel_Data[2];

		glDeleteTextures(3, Lightmap.Lightmap_Buffer);
		glDeleteFramebuffers(3, Lightmap.Lightmap_Framebuffer);
	}

	void Init_Shadow_Caster(Shadow_Caster& Caster)
	{
		glGenFramebuffers(1, &Caster.Shadow_Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Caster.Shadow_Framebuffer);

		Create_Shader("Lightmapping/Generate_Shadowmap_Cubemap.frag", "Lightmapping/Generate_Shadowmap_Cubemap.vert", &Caster.Shadow_Object_Shader, "Lightmapping/Generate_Shadowmap_Cubemap.geom");
	}

	void Destroy_Shadow_Caster(Shadow_Caster& Caster)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Caster.Shadow_Framebuffer);
		glDeleteFramebuffers(1, &Caster.Shadow_Framebuffer);

		Destroy_Shader(&Caster.Shadow_Object_Shader);
	}

	void Update_Shadow_Matrix(Shadow_Caster& Caster, glm::vec3 Position)
	{
		const glm::vec3 Data[6][2] =
		{
			{ glm::vec3(1.f, 0.f, 0.f),		glm::vec3(0.f, -1.f, 0.f)	},
			{ glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(0.f, -1.f, 0.f)	},
			{ glm::vec3(0.f, 1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f)	},
			{ glm::vec3(0.f, -1.f, 0.f),	glm::vec3(0.f, 0.f, -1.f)	},
			{ glm::vec3(0.f, 0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)	},
			{ glm::vec3(0.f, 0.f, -1.f),	glm::vec3(0.f, -1.f, 0.f)	}
		};

#define View(Function, Face) glm::Function(Position, Position + Data[Face][0], Data[Face][1])

		const glm::mat4 Views[6] =
		{
			View(lookAt, 0),
			View(lookAt, 1),
			View(lookAt, 2),
			View(lookAt, 3),
			View(lookAt, 4),
			View(lookAt, 5),
		};

#undef View

		for (size_t Face = 0; Face < 6; Face++)
			Caster.Shadow_Projection_Matrix[Face] = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, Caster.Shadow_Map_Far_Plane) * Views[Face]; // glm::lookAt(Position, Position + Data[Face][0], Data[Face][1]);
	}

	void Chart_Tri_Init_Points(Lightmap_Chart* Target_Chart, size_t Tri);

	void Lightmap_Cast_Light(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, Lightsource Light, Shadow_Caster& Caster, Lightmap_Buffer_Data& Lightmap, Lightmap_Cubemap& Cubemap)		// This will initialise a shadowmap and then render that light to the lightmap accordingly
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.Shadow_Cubemap);
		glBindFramebuffer(GL_FRAMEBUFFER, Caster.Shadow_Framebuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, Cubemap.Shadow_Cubemap, 0u);
		glViewport(0, 0, Caster.Shadow_Map_Width, Caster.Shadow_Map_Height);

		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glClear(GL_DEPTH_BUFFER_BIT);

		Update_Shadow_Matrix(Caster, Light.Position);

		// This will then initialise the shadow cubemap 

		Use_Shader(Caster.Shadow_Object_Shader);
		glUniformMatrix4fv(glGetUniformLocation(Caster.Shadow_Object_Shader.Program_ID, "Projection_Matrix"), 6, GL_FALSE, glm::value_ptr(Caster.Shadow_Projection_Matrix[0]));

		/*for (size_t Object = 0; Object < Engine->Scene.Objects.size(); Object++)
		{
			// for now, will just treat these all as static lightmap objects
			Bind_Vertex_Buffer(Engine->Scene.Objects[Object]->Mesh);
			Default_Uniform_Assign_Function(&Caster.Shadow_Object_Shader, Engine->Scene.Objects[Object], &Engine->Scene);
			glDrawArrays(GL_TRIANGLES, 0, Engine->Scene.Objects[Object]->Mesh.Vertex_Count);
		}*/

		for (size_t Object = 0; Object < Target_Chart->Pushed_Objects.size(); Object++)
		{
			Bind_Vertex_Buffer(Target_Chart->Pushed_Objects[Object]->Mesh);
			Default_Uniform_Assign_Function(&Caster.Shadow_Object_Shader, Target_Chart->Pushed_Objects[Object], &Engine->Scene);
			glDrawArrays(GL_TRIANGLES, 0, Target_Chart->Pushed_Objects[Object]->Mesh.Vertex_Count);
		}

		//glfwSwapBuffers(Engine->Window);
		//glfwPollEvents();

		// Shadowmap rendered! Now render each tri to the lightmap texture

		glViewport(0, 0, Target_Chart->Sidelength, Target_Chart->Sidelength);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glBlendFunc(GL_ONE, GL_ONE); // additive blending
		glEnable(GL_BLEND);

		Use_Shader(Lightmap.Lightmap_Write_Shader);

		glUniform3f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Light_Position"), Light.Position.x, Light.Position.y, Light.Position.z);
		glUniform3f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Colour"), Light.Colour.x, Light.Colour.y, Light.Colour.z);

		glUniform3f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Light_Direction"), Light.Direction.x, Light.Direction.y, Light.Direction.z);
		glUniform1f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Light_Angle"), Light.FOV * 3.14159f / 360.0f);
		glUniform1f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Fade_Angle"), 0.001f);

		glUniform1i(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Shadow_Map"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.Shadow_Cubemap);

		glViewport(0, 0, Target_Chart->Sidelength, Target_Chart->Sidelength);

		for (size_t Map = 0; Map < 3; Map++)									// NOTE that there are 3 lightmap textures for each vector
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Lightmap.Lightmap_Framebuffer[Map]);

			glBindTexture(GL_TEXTURE_2D, Lightmap.Lightmap_Buffer[Map]);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Lightmap.Lightmap_Buffer[Map], 0);

			for (size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
			{
				// Target_Chart->Pushed_Tris[Tri].P

				// Chart_Tri_Init_Points(Target_Chart, Tri);

				glm::vec2 Lightmap_UVs[3];

				glm::vec3 Triple_Vector;

				Triple_Vector = Target_Chart->Pushed_Tris[Tri].Triple_Vectors[Map];// +Target_Chart->Pushed_Tris[Tri].Triple_Vectors[1] + Target_Chart->Pushed_Tris[Tri].Triple_Vectors[2];
					//Target_Chart->Pushed_Tris[Tri].TBN[Map];
				//glUniform3fv(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "TBN"), 3, glm::value_ptr(Target_Chart->Pushed_Tris[Tri].TBN[0]));
				//glUniform1ui(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Index"), Map);
					
					//Target_Chart->Pushed_Tris[Tri].Triple_Vectors[Map];

				Lightmap_UVs[0] =
					Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index].Lightmap_UV * glm::vec2(2.0f / Target_Chart->Sidelength) - glm::vec2(1.0f); 
				Lightmap_UVs[1] =
					Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 1].Lightmap_UV * glm::vec2(2.0f / Target_Chart->Sidelength) - glm::vec2(1.0f);
				Lightmap_UVs[2] =
					Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 2].Lightmap_UV * glm::vec2(2.0f / Target_Chart->Sidelength) - glm::vec2(1.0f);

				glUniform3fv(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Positions"), 3, glm::value_ptr(Target_Chart->Pushed_Tris[Tri].Points[0]));
				glUniform2f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Lightmap_UVs_0"), Lightmap_UVs[0].x, Lightmap_UVs[0].y);
				glUniform2f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Lightmap_UVs_1"), Lightmap_UVs[1].x, Lightmap_UVs[1].y);
				glUniform2f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Lightmap_UVs_2"), Lightmap_UVs[2].x, Lightmap_UVs[2].y);
				glUniform3f(glGetUniformLocation(Lightmap.Lightmap_Write_Shader.Program_ID, "Triple_Vector"), Triple_Vector.x, Triple_Vector.y, Triple_Vector.z);

				if(glm::dot(Light.Position - Target_Chart->Pushed_Tris[Tri].Points[0], Target_Chart->Pushed_Tris[Tri].TBN[2]) >= 0.0f)
					glDrawArrays(GL_TRIANGLES, 0, 3); // It doesn't matter what vertex buffer is bound
			}
			//glfwSwapBuffers(Engine->Window);
			//glfwPollEvents();
		}

		glDisable(GL_BLEND);
	}

	void Chart_Tri_Init_Points(Lightmap_Chart* Target_Chart, size_t Tri);

	void Lightmap_Lighting_Pass(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const std::vector<Lightsource*>& Lightsources, Shadow_Caster& Caster, Lightmap_Buffer_Data& Lightmap) // This will take a set of lightsources and apply each of their lighting all of the current tris in the lightmap
	{
		Lightmap_Cubemap Cubemap;

		glGenTextures(1, &Cubemap.Shadow_Cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.Shadow_Cubemap);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		for (size_t Face = 0; Face < 6; Face++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, 0, GL_DEPTH_STENCIL, Caster.Shadow_Map_Width, Caster.Shadow_Map_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		for (size_t Light = 0; Light < Lightsources.size(); Light++)
		{
			Lightmap_Cast_Light(Engine, Target_Chart, *Lightsources[Light], Caster, Lightmap, Cubemap);
		}

		for (size_t Node = 0; Node < Engine->Scene.Lighting.Lighting_Nodes.Nodes.size(); Node++)
			Accumulate_Lighting_Node_Lights(Engine, Target_Chart, Lightsources, &Engine->Scene.Lighting.Lighting_Nodes.Nodes[Node]);

		glDeleteTextures(1, &Cubemap.Shadow_Cubemap);
	}

	void Lightmap_Bounced_Lighting_Pass(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, Shadow_Caster& Caster, Lightmap_Buffer_Data& Lightmap, int Bounces = 7, bool Generate_Bounced_Light = false)
	{
		size_t Pixel_Count = Target_Chart->Sidelength * Target_Chart->Sidelength;

		if (Generate_Bounced_Light)
		{
			std::vector<Lightsource*> Lightsources;
			
			glm::vec3* New_Data[3] = {
				new glm::vec3[Pixel_Count],
				new glm::vec3[Pixel_Count],
				new glm::vec3[Pixel_Count]
			};

			for (size_t Pixel = 0; Pixel < Pixel_Count; Pixel++)
			{
				New_Data[0][Pixel] = Lightmap.Pixel_Data[0][Pixel] - Lightmap.Previous_Pixel_Data[0][Pixel];
				New_Data[1][Pixel] = Lightmap.Pixel_Data[1][Pixel] - Lightmap.Previous_Pixel_Data[1][Pixel];
				New_Data[2][Pixel] = Lightmap.Pixel_Data[2][Pixel] - Lightmap.Previous_Pixel_Data[2][Pixel];
			}

			Generate_Bounced_Light_Lightsources(Engine, Target_Chart, New_Data, Lightsources);

			delete New_Data[0];
			delete New_Data[1];
			delete New_Data[2];

			Lightmap_Lighting_Pass(Engine, Target_Chart, Lightsources, Caster, Lightmap);

			for (auto& Light : Lightsources)		// This just deletes all the unnecessary lights that were just created
				delete Light;
		}
		else
			Lightmap_Lighting_Pass(Engine, Target_Chart, Engine->Scene.Lighting.Lightsources, Caster, Lightmap);

		for (size_t Face = 0; Face < 3; Face++)
		{
			memcpy(Lightmap.Previous_Pixel_Data[Face], Lightmap.Pixel_Data[Face], Pixel_Count * sizeof(glm::vec3));

			glBindTexture(GL_TEXTURE_2D, Lightmap.Lightmap_Buffer[Face]);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, Lightmap.Pixel_Data[Face]);

			// delete Pixel_Data[Face];
		}

		if (--Bounces)
			Lightmap_Bounced_Lighting_Pass(Engine, Target_Chart, Caster, Lightmap, Bounces, true);
	}

	void Create_Lightmap3_From_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* Filename)
	{
		// This will create special frame buffers for the shadow mapping when rendering lights to the screen

		printf(" >> Begun creating lightmap!\n");

		//getchar();

		//for(size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
		//	Chart_Tri_Init_Points(Target_Chart, Tri);

		Shadow_Caster Caster;
		Lightmap_Buffer_Data Lightmap;

		//while (true)
		//{

		Init_Shadow_Caster(Caster);

		Init_Lightmap_Buffer_Data(Lightmap, Target_Chart);

		Lightmap_Bounced_Lighting_Pass(Engine, Target_Chart, Caster, Lightmap);

		//Lightmap_Lighting_Pass(Engine, Target_Chart, Engine->Scene.Lighting.Lightsources, Caster, Lightmap);

		// This will get the pixel data back from the texture gl object

		// Then, it'll write the compressed data to the file and return

		if (Filename)
			Write_Lightmap3_To_File(&Engine->Job_Handler, (std::string(Filename) + ".opz").c_str(), Lightmap.Pixel_Data, Target_Chart->Sidelength, true);

		Destroy_Shadow_Caster(Caster);
		Destroy_Lightmap_Buffer_Data(Lightmap);

		//}

	}
}