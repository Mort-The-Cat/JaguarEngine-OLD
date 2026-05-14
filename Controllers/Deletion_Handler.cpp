#include "Jaguar_Engine_Wrapper.h"

namespace Jaguar
{

	void Handle_Render_Pipeline_Deletions(Render_Pipeline* Target_Pipeline)
	{
		for (size_t W = 0; W < Target_Pipeline->Render_Queues.size(); W++)
		{
			for (size_t V = 0; V < Target_Pipeline->Render_Queues[W].Objects.size(); V++)
				if (reinterpret_cast<World_Object*>(Target_Pipeline->Render_Queues[W].Objects[V])->Flags[MF_TO_BE_DELETED])
					Target_Pipeline->Render_Queues[W].Objects[V] = nullptr; // Removes reference by setting pointer to NULL

			Target_Pipeline->Render_Queues[W].Objects.erase(
				std::remove(
					Target_Pipeline->Render_Queues[W].Objects.begin(),
					Target_Pipeline->Render_Queues[W].Objects.end(), nullptr),
				Target_Pipeline->Render_Queues[W].Objects.end());

			// Removes all NULL pointers from render pipeline's queue
		}
	}

	void Handle_Scene_Deletions(Scene_Data* Target_Scene)
	{
		// This will handle the deallocate of scene objects


		for (size_t W = 0; W < Target_Scene->Objects.size(); W++)
			if (Target_Scene->Objects[W]->Flags[MF_TO_BE_DELETED])
			{
				// Deallocate object and set pointer to NULL
				Destroy_World_Object(Target_Scene->Objects[W]);

				Target_Scene->Objects[W] = nullptr;
			}

		Target_Scene->Objects.erase(std::remove(Target_Scene->Objects.begin(), Target_Scene->Objects.end(), nullptr), Target_Scene->Objects.end()); // Deletes all NULL pointers from vector
		//Target_Scene->Objects.erase(Deleted_Objects.begin(), Deleted_Objects.end());
	}

	void Handle_Deletions(Jaguar_Engine* Target_Engine)
	{
		Handle_Render_Pipeline_Deletions(&Target_Engine->Pipeline);

		Handle_Scene_Deletions(&Target_Engine->Scene);
	}

	void Delete_All(Scene_Data* Target_Scene)
	{
		for (size_t W = 0; W < Target_Scene->Objects.size(); W++)
			Target_Scene->Objects[W]->Flags[MF_TO_BE_DELETED] = true;
	}

}