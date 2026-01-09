#include "Jaguar_Engine_Wrapper.h"
#include "../OpenGL_Handling/Scene.h"

#include<random>

namespace Jaguar
{
	float RNG()
	{
		return (float)rand() / (float)RAND_MAX;
	}

	void Handle_Scene_Controllers(Jaguar_Engine* Engine) // This is all
	{
		// For now, this won't be multi-threaded

		for (size_t W = 0; W < Engine->Scene.Objects.size(); W++)
		{
			if (Engine->Scene.Objects[W]->Flags[MF_ACTIVE])
				Engine->Scene.Objects[W]->Control->Control_Function(Engine);
		}
	}

}