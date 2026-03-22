#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include "Shadow_Map_Lightmapping.h"

namespace Jaguar
{
	struct Lightmap_Cubemap
	{
		unsigned int Shadow_Cubemap;
		glm::vec3 Position;
	};

	struct Shadow_Caster
	{
		// This will handle the shadow-casting information

		unsigned int Shadow_Framebuffer;

		const unsigned int Shadow_Map_Width = 1024u, Shadow_Map_Height = 1024u;		// I'll use 1024 because 256 is too low-res

		Shader Shadow_Object_Shader;

		glm::mat4 Shadow_Projection_Matrix[6];

		const float Shadow_Map_Far_Plane = 25.0f;
	};

	struct Lightmap_Buffer_Data
	{
		glm::vec3* Pixel_Data;
		unsigned int Lightmap_Buffer;
		Shader Lightmap_Write_Shader;
	};

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
			{ glm::vec3(1.f, 0.f, 0.f),		glm::vec3(0.f,-1.f, 0.f)	},
			{ glm::vec3(-1.f, 0.f, 0.f),	glm::vec3(0.f, -1.f, 0.f)	},
			{ glm::vec3(0.f, 1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f)	},
			{ glm::vec3(0.f, -1.f, 0.f),	glm::vec3(0.f, 0.f, -1.f)	},
			{ glm::vec3(0.f, 0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f)	},
			{ glm::vec3(0.f, 0.f, -1.f),	glm::vec3(0.f, -1.f, 0.f)	}
		};

		for(size_t Face = 0; Face < 6; Face++)
			Caster.Shadow_Projection_Matrix[Face] = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, Caster.Shadow_Map_Far_Plane) * glm::lookAt(Position, Position + Data[Face][0], Data[Face][1]);
	}

	void Lightmap_Cast_Light(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, Lightsource Light, Shadow_Caster& Caster, Lightmap_Buffer_Data& Lightmap, Lightmap_Cubemap& Cubemap)		// This will initialise a shadowmap and then render that light to the lightmap accordingly
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.Shadow_Cubemap);
		glBindFramebuffer(GL_FRAMEBUFFER, Caster.Shadow_Framebuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, Cubemap.Shadow_Cubemap, 0u);
		glViewport(0, 0, Caster.Shadow_Map_Width, Caster.Shadow_Map_Height);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glClear(GL_DEPTH_BUFFER_BIT);

		Update_Shadow_Matrix(Caster, Light.Position);

		// This will then initialise the shadow cubemap 

		Use_Shader(Caster.Shadow_Object_Shader);
		glUniformMatrix4fv(glGetUniformLocation(Caster.Shadow_Object_Shader.Program_ID, "Projection_Matrix"), 6, GL_FALSE, glm::value_ptr(Caster.Shadow_Projection_Matrix[0]));

		for (size_t Object = 0; Object < Engine->Scene.Objects.size(); Object++)
		{
			// for now, will just treat these all as static lightmap objects
			Bind_Vertex_Buffer(Engine->Scene.Objects[Object]->Mesh);
			Default_Uniform_Assign_Function(&Caster.Shadow_Object_Shader, Engine->Scene.Objects[Object], &Engine->Scene);
			glDrawArrays(GL_TRIANGLES, 0, Engine->Scene.Objects[Object]->Mesh.Vertex_Count);
		}

		// Shadowmap rendered! Now render each tri to the lightmap texture

		glBindTexture(GL_TEXTURE_2D, Lightmap.Lightmap_Buffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_RGB32F, Lightmap.Lightmap_Buffer, 0);
		glViewport(0, 0, Target_Chart->Sidelength, Target_Chart->Sidelength);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		glfwSwapBuffers(Engine->Window);
		glfwPollEvents();
	}

	void Lightmap_Lighting_Pass(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const std::vector<Lightsource*>& Lightsources, Shadow_Caster& Caster, Lightmap_Buffer_Data& Lightmap) // This will take a set of lightsources and apply each of their lighting all of the current tris in the lightmap
	{
		Lightmap_Cubemap Cubemap;

		glGenTextures(1, &Cubemap.Shadow_Cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap.Shadow_Cubemap);

		for (size_t Face = 0; Face < 6; Face++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, 0, GL_DEPTH_STENCIL, Caster.Shadow_Map_Width, Caster.Shadow_Map_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

		for (size_t Light = 0; Light < Lightsources.size(); Light++)
		{
			Lightmap_Cast_Light(Engine, Target_Chart, *Lightsources[Light], Caster, Lightmap, Cubemap);
		}

		glDeleteTextures(1, &Cubemap.Shadow_Cubemap);
	}

	void Create_Lightmap3_From_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* Filename)
	{
		// This will create special frame buffers for the shadow mapping when rendering lights to the screen

		printf(" >> Begun creating lightmap!\n");

		getchar();

		Shadow_Caster Caster;
		Lightmap_Buffer_Data Lightmap;

		Init_Shadow_Caster(Caster);

		Lightmap.Pixel_Data = new glm::vec3[Target_Chart->Sidelength * Target_Chart->Sidelength];
		while(true)
			Lightmap_Lighting_Pass(Engine, Target_Chart, Engine->Scene.Lighting.Lightsources, Caster, Lightmap);

		// This will get the pixel data back from the texture gl object

		// Then, it'll write the compressed data to the file and return

		Destroy_Shadow_Caster(Caster);
		delete Lightmap.Pixel_Data;
	}


}