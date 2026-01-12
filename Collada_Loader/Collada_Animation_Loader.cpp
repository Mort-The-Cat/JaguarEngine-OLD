#include "Collada_Loader.h"

namespace Collada
{
	void Print_Matrix(glm::mat4 Matrix)
	{
		for (size_t Row = 0; Row < Matrix.length(); Row++)
		{
			printf("\t");

			for (size_t Column = 0; Column < Matrix[Row].length(); Column++)
			{
				printf("%f ", Matrix[Row][Column]);
			}

			printf("\n");
		}
	}



	int Load_Animation(const XML_Document& Document, Collada_Animation* Target_Animation)
	{
		// Gets the joint index map

		// Iterates through the animations,
			// Gets its index
			// Loads matrices and times
				// Sets keyframe data accordingly

		std::map<std::string, unsigned int> Joint_Index_Map;

		std::vector<glm::mat4> Joint_Matrix_Buffer;

		{
			const XML_Document Root_Node = Document["COLLADA"][0]["library_visual_scenes"][0]["visual_scene"][0]["node"][0]["node"][0];

			// Bone root node (for index map)

			Load_Child_Joint_Name_Map(Root_Node, Joint_Index_Map, Joint_Matrix_Buffer);
		}

		Target_Animation->Keyframes.resize(Joint_Index_Map.size()); // Creates indices for every joint

		const XML_Document Animations = Document["COLLADA"][0]["library_animations"][0]["animation"][0];

		for (size_t W = 0; W < Animations["animation"].size(); W++)
		{
			std::vector<glm::mat4> Transformations;
			std::vector<glm::vec1> Times;

			Load_Strings_To_Matrices(Animations["animation"][W]["source"][1]["float_array"][0].Data_Array, Transformations);
			Load_Strings_To_Vectors(Animations["animation"][W]["source"][0]["float_array"][0].Data_Array, Times);

			size_t Index = Joint_Index_Map[Animations["animation"][W]["channel"][0].Target];

			//float Cumulative_Time = 0.0f;

			for (size_t V = 0; V < Times.size(); V++)
			{
				Collada::Collada_Keyframe Keyframe;
				//Cumulative_Time += Times[V].x;
				Keyframe.Time = Times[V].x;// Cumulative_Time;
				Keyframe.Transformation_Matrix = Transformations[V];

				if(Index < Target_Animation->Keyframes.size())
					Target_Animation->Keyframes[Index].push_back(Keyframe);
			}
		}

		for (size_t W = 0; W < Target_Animation->Keyframes.size(); W++)
		{
			if (!Target_Animation->Keyframes[W].size())
			{
				// If no keyframes? Use the default joint matrices from the joint_matrix_buffer

				Collada::Collada_Keyframe Keyframe;
				Keyframe.Time = 0.0f;
				Keyframe.Transformation_Matrix = Joint_Matrix_Buffer[W]; //glm::inverse(Joint_Matrix_Buffer[W]);
				Target_Animation->Keyframes[W].push_back(Keyframe);
			}
		}

		return 0;
	}

}