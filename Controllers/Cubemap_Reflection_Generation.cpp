#include "Cubemap_Reflection_Generation.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	glm::mat4 Get_Perspective_Matrix(Jaguar::Cubemap* Target_Cubemap, glm::vec3 Direction, glm::vec3 Up_Vector)
	{
		// Get left/right difference and get up/down difference in cubemap AABB

		// Then, get forward distance to edge of cubemap

		float Near = 0.0001f, Far = 100.0f;

		glm::mat4 Perspective_Test = glm::mat4(1.0f); // glm::perspective(glm::radians(90.0f), 1.0f, 0.0f, 100.0f);

		glm::vec3 Right_Vector = glm::cross(Direction, Up_Vector);

		float X_Length = glm::dot(Target_Cubemap->B - Target_Cubemap->A, Right_Vector);
		float X_Delta = glm::dot((Target_Cubemap->B + Target_Cubemap->A) * glm::vec3(0.5f)
			- Target_Cubemap->Origin
			, Right_Vector);

		float Y_Length = glm::dot(Target_Cubemap->B - Target_Cubemap->A, Up_Vector);
		float Y_Delta = glm::dot((Target_Cubemap->B + Target_Cubemap->A) * glm::vec3(0.5f) 
			- Target_Cubemap->Origin
			, Up_Vector);


		float Z_Delta;

		if (Direction.z == 1.0f)
		{
			X_Length *= -1.0f;
			X_Delta *= -1.0f;

			Y_Length *= -1.0f;
			Y_Delta *= -1.0f;
		}
		else if (Direction.z == -1.0f)
		{
			X_Length *= -1.0f;
			X_Delta *= -1.0f;
		}
		else if (Direction.x == 1.0f)
		{
			Y_Length *= -1.0f;
			Y_Delta *= -1.0f;
		}

		if (glm::dot(glm::vec3(1), Direction) < 0) // Some kind of negative direction?
		{
			// Use A

			Z_Delta = glm::dot(Target_Cubemap->Origin - Target_Cubemap->A, Direction);

			//X_Delta *= -1.0f;
			//Y_Delta *= -1.0f;
		}
		else
		{
			// Use B

			Z_Delta = glm::dot(Target_Cubemap->B - Target_Cubemap->Origin, Direction);
		}

		//Z_Delta = glm::dot(Target_Cubemap->Origin - Target_Cubemap->A, Direction);

		//if(Z_Delta < 0.0f)
		//	Z_Delta = glm::dot(Target_Cubemap->B - Target_Cubemap->Origin, Direction);

		//

		float X_Skew = X_Delta / Z_Delta;
		float Y_Skew = Y_Delta / Z_Delta;

		//float X_Factor = Z_Delta / X_Length;
		//float Y_Factor = Z_Delta / Y_Length;

		//X_Skew *= X_Factor;
		//Y_Skew *= Y_Factor;

		glm::mat4 Test_P = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f);

		glm::mat4 Final_P =
			glm::mat4(
				-2.0f * Z_Delta / X_Length,			0,								0,					0,
				0,									2.0f * Z_Delta / Y_Length,		0,					0,
				-2.0f * X_Delta / X_Length,			-2.0f * Y_Delta / Y_Length,		-(Far + Near) / (Far - Near),		-1.0f,
				0,									0,								-2 * Far * Near / (Far - Near),		0
			);
			
			
			/*=
			glm::mat4(
				2.0f * Z_Delta / X_Length, 0, 2.0f * X_Delta / X_Length, 0,
				0, 2.0f * Z_Delta / Y_Length, 2.0f * Y_Delta / Y_Length, 0,
				0, 0, -(Far + Near) / (Far - Near), -2 * Far * Near / (Far - Near),
				0, 0, -1.0f, 0

			);*/

		return Final_P;

		//return glm::perspective(glm::radians(90.0f), 1.0f, 0.0001f, 100.0f);
	}

	void Generate_Cubemap(Jaguar::Jaguar_Engine* Engine, Cubemap* Target_Cubemap, unsigned int Width, unsigned Height)
	{
		// This will create a special framebuffer which we'll render to using the regular engine render pipeline
		// We'll also set the Camera_Projection_Matrix according to each face of the cubemap

		unsigned int Frame_Buffer;
		glGenFramebuffers(1, &Frame_Buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer);

		unsigned int Render_Buffer; // This is for the depth/stencil buffer
		glGenRenderbuffers(1, &Render_Buffer);

		glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Render_Buffer);

		glGenTextures(1, &Target_Cubemap->Cubemap_Texture);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

		for (size_t Face = 0; Face < 6; Face++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		//

		glViewport(0, 0, Width, Height);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		//

		for (size_t Face = 0; Face < 6; Face++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer);

			glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, Target_Cubemap->Cubemap_Texture, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				printf(" >> ERROR! Incomplete framebuffer...\n");

			//

			glClearColor(0.3, 0.3, 0.2, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			const glm::vec3 Directions[6] =
			{
				glm::vec3(1, 0, 0),
				glm::vec3(-1, 0, 0),

				glm::vec3(0, 1, 0),
				glm::vec3(0, -1, 0),

				glm::vec3(0, 0, 1),
				glm::vec3(0, 0, -1)
			};

			const glm::vec3 Up_Vectors[6] =
			{
				glm::vec3(0, -1, 0),
				glm::vec3(0, 1, 0),

				glm::vec3(0, 0, -1),
				glm::vec3(0, 0, -1),

				glm::vec3(0, -1, 0),
				glm::vec3(0, 1, 0)
			};

			//

			// Get FOV for y and x

			Engine->Scene.Camera_Projection_Matrix = Get_Perspective_Matrix(Target_Cubemap, Directions[Face], Up_Vectors[Face]);

			// 

			Engine->Scene.Camera_Projection_Matrix =
				Engine->Scene.Camera_Projection_Matrix *
				glm::lookAt(Target_Cubemap->Origin, Target_Cubemap->Origin + Directions[Face], (Up_Vectors[Face]));

			Draw_Render_Pipeline(Engine);

			glfwSwapBuffers(Engine->Window);
		}

		//
		
		glDeleteRenderbuffers(1, &Render_Buffer);
		glDeleteFramebuffers(1, &Frame_Buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}


}