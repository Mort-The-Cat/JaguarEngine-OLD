#ifndef JAGUAR_STATIC_BLOCKMAP
#define JAGUAR_STATIC_BLOCKMAP

#include<vector>

#include "Hitboxes.h"

namespace Jaguar
{

	struct Blockmap_Data
	{
		// The blockmap will just encompass the whole map's hitbox

		std::vector<std::vector<std::vector<		// 3 axes
			std::vector<Hitbox*>					// list of static hitboxes
		>>> Blockmap_Hitboxes;

		glm::vec3 Origin;							// Blockmap origin
		glm::ivec3 Length;							// Number of elements per axes
		float Size;
	};

	glm::ivec3 Get_Index_From_Blockmap(const Blockmap_Data& Blockmap, glm::vec3 Position);

	struct Jaguar_Engine;

	void Initialise_Blockmap(Jaguar_Engine* Engine, float Size);

	const std::vector<Hitbox*>* Read_Blockmap(const Blockmap_Data& Blockmap, glm::vec3 Position);		// This doesn't take into account size... assume that each caller object is a reasonable size
}


#endif