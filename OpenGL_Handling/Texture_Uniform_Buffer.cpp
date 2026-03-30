#include "Texture_Uniform_Buffer.h"

namespace Jaguar
{
	void Destroy_Texture_Buffer(Texture* Target_Texture) // (Use other function for cubemaps etc)
	{
		glBindTexture(GL_TEXTURE_2D, Target_Texture->Texture_Buffer_ID);
		glDeleteTextures(1, &Target_Texture->Texture_Buffer_ID);
	}

	void Create_Texture_Buffer(
		Texture* Target_Texture, GLuint Target_Colour_Format, GLuint Width,
		GLuint Height, GLuint Source_Colour_Format,	GLuint Data_Type, 
		const void* Pixel_Data, bool Generate_Mipmap)
	{
		glGenTextures(1, &Target_Texture->Texture_Buffer_ID);

		glBindTexture(GL_TEXTURE_2D, Target_Texture->Texture_Buffer_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Generate_Mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, Target_Colour_Format, Width, Height, 0, Source_Colour_Format, Data_Type, Pixel_Data);

		if (Generate_Mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);

		// This generates the texture and generates a mipmap if specified

		//glTexImage2D(GL_TEXTURE_2D)
	}

}