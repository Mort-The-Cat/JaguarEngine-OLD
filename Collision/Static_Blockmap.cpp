#include "Static_Blockmap.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{

	glm::ivec3 Get_Index_From_Blockmap(const Blockmap_Data& Blockmap, glm::vec3 Position)
	{
		Position = glm::max(Blockmap.Origin, Position);
		Position -= Blockmap.Origin;
		Position /= Blockmap.Size;

		return glm::min(Blockmap.Length - glm::ivec3(1), glm::ivec3(Position));
	}

	void Initialise_Blockmap(Jaguar_Engine* Engine, float Size)
	{
		glm::vec3 A = glm::vec3(99999999.0f), B = -A;

		for (size_t H = 0; H < Engine->Physics.Hitboxes.size(); H++)
		{
			// Get bounds for the blockmap
			glm::vec3 Local_A, Local_B;
			Engine->Physics.Hitboxes[H]->Get_Bounds(&Local_A, &Local_B);
			A = glm::min(A, Local_A);
			B = glm::max(B, Local_B);
		}

		// Initialise blockmap

		Engine->Physics.Blockmap.Origin = A;
		Engine->Physics.Blockmap.Size = Size;

		B -= A;

		Engine->Physics.Blockmap.Length = glm::ceil(B / Size);

		Engine->Physics.Blockmap.Blockmap_Hitboxes.clear();

		Engine->Physics.Blockmap.Blockmap_Hitboxes.resize(Engine->Physics.Blockmap.Length[0]);
		for (size_t X = 0; X < Engine->Physics.Blockmap.Length[0]; X++)
		{
			Engine->Physics.Blockmap.Blockmap_Hitboxes[X].resize(Engine->Physics.Blockmap.Length[1]);
			for (size_t Y = 0; Y < Engine->Physics.Blockmap.Length[1]; Y++)
				Engine->Physics.Blockmap.Blockmap_Hitboxes[X][Y].resize(Engine->Physics.Blockmap.Length[2]);
		}

		// Then, we populate the blockmap...

		for (size_t H = 0; H < Engine->Physics.Hitboxes.size(); H++)
		{
			glm::vec3 A, B;
			glm::ivec3 I_A, I_B;

			Engine->Physics.Hitboxes[H]->Get_Bounds(&A, &B);
			I_A = Get_Index_From_Blockmap(Engine->Physics.Blockmap, A);
			I_B = Get_Index_From_Blockmap(Engine->Physics.Blockmap, B);

			for (float X = I_A[0]; X <= I_B[0]; X++)
				for (float Y = I_A[1]; Y <= I_B[1]; Y++)
					for (float Z = I_A[2]; Z <= I_B[2]; Z++)
						Engine->Physics.Blockmap.Blockmap_Hitboxes[X][Y][Z].push_back(Engine->Physics.Hitboxes[H]);
		}
	}


}