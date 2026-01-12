#include "Collada_Loader.h"
#include <algorithm>

namespace Collada
{
	void Load_Child_Joint_Name_Map(XML_Document Parent_Node, std::map<std::string, unsigned int>& Bone_Map, std::vector<glm::mat4>& Buffer)
	{
		int Index = Bone_Map.size();

		Bone_Map[Parent_Node.Id] = Index;

		Load_Strings_To_Matrices(Parent_Node["matrix"][0].Data_Array, Buffer); // Loads joint matrix into buffer

		if (Parent_Node.Nodes.find("node") != Parent_Node.Nodes.end())
			for (size_t W = 0; W < Parent_Node.Nodes["node"].size(); W++)
				Load_Child_Joint_Name_Map(Parent_Node["node"][W], Bone_Map, Buffer);
	}

	void Load_Child_Joints(size_t Parent_Index, XML_Document Parent_Node, Collada_Skeleton* Target_Skeleton)
	{
		Collada_Joint Joint;
		Joint.Parent_Joint = Parent_Index;

		// std::vector<glm::mat4> Buffer;

		//Joint.Offset_Matrix = Buffer[0];

		size_t Current_Joint_Index = Target_Skeleton->Joints.size();

		Target_Skeleton->Joints.push_back(Joint); // Adds new joint

		if (Parent_Node.Nodes.find("node") != Parent_Node.Nodes.end()) // If there are any "nodes" ? We know there are child joints
		{
			// Handle each child joint

			for (size_t W = 0; W < Parent_Node.Nodes["node"].size(); W++)
			{
				Target_Skeleton->Joints[Current_Joint_Index].Child_Joints.push_back(Target_Skeleton->Joints.size()); // Adds child node index to the node
				
				Load_Child_Joints(Current_Joint_Index, Parent_Node.Nodes["node"][W], Target_Skeleton);
			}
		}

		// Otherwise? Return from the function...
	}

	int Load_Skeleton(const XML_Document& Document, Collada_Skeleton* Target_Skeleton)
	{
		std::vector<glm::mat4> Bind_Poses_Array;
		std::vector<glm::vec1> Weights;

		//

		const std::vector<XML_Document> Skins = Document["COLLADA"].back()["library_controllers"].back()["controller"].back()["skin"];

		for (const auto& Skin : Skins) // Iterates through skins
		{
			std::vector<glm::mat4> Bind_Shape_Matrix;

			Load_Strings_To_Matrices(Skin["bind_shape_matrix"][0].Data_Array, Bind_Shape_Matrix);

			Target_Skeleton->Bind_Shape_Matrix = Bind_Shape_Matrix[0];

			// The second source (index=1) of this skin is the bind poses array
			Load_Strings_To_Matrices(Skin["source"][1]["float_array"][0].Data_Array, Bind_Poses_Array);
			Load_Strings_To_Vectors(Skin["source"][2]["float_array"][0].Data_Array, Weights);

			// This gets the weight values and the bind poses array

			const XML_Document Vertex_Weights_Node = Skin["vertex_weights"].back();

			const std::vector<std::string> V_Count = Vertex_Weights_Node["vcount"].back().Data_Array;
			const std::vector<std::string> Weight_Data = Vertex_Weights_Node["v"].back().Data_Array;

			Target_Skeleton->Vertex_Weights.resize(V_Count.size());

			size_t Index = 0;

			for (size_t W = 0; W < V_Count.size(); W++)
			{
				size_t Number_Of_Weights = std::stoi(V_Count[W]);
				Target_Skeleton->Vertex_Weights[W].resize(Number_Of_Weights);

				for (size_t V = 0; V < Number_Of_Weights; V++)
				{
					Target_Skeleton->Vertex_Weights[W][V].Joint_Index = std::stoi(Weight_Data[Index++]);
					Target_Skeleton->Vertex_Weights[W][V].Weight = Weights[std::stoi(Weight_Data[Index++])].x;
				}

				std::sort(Target_Skeleton->Vertex_Weights[W].begin(), Target_Skeleton->Vertex_Weights[W].end());

				Target_Skeleton->Vertex_Weights[W].resize(MAX_WEIGHTS_IN_SKELETON);

				// Normalize first 2 weights

				// find value L such that L(weight_0 + weight_1) = 65,535

				// then multiply both values by L

				// This ensures that the sum weights add up to 1.0f

				float L = 65535.0f / (Target_Skeleton->Vertex_Weights[W][0].Weight + Target_Skeleton->Vertex_Weights[W][1].Weight);

				Target_Skeleton->Vertex_Weights[W][0].Weight *= L;

				Target_Skeleton->Vertex_Weights[W][1].Weight *= L;
			}

			// Create Joint hierarchy
		}

		const XML_Document Root_Node = Document["COLLADA"][0]["library_visual_scenes"][0]["visual_scene"][0]["node"][0]["node"][0];

		// Start from Root_Node, 

		Load_Child_Joints(0, Root_Node, Target_Skeleton);

		for (size_t W = 0; W < Bind_Poses_Array.size(); W++)
			Target_Skeleton->Joints[W].Offset_Matrix = Bind_Poses_Array[W];

		Bind_Poses_Array.clear();

		Load_Strings_To_Matrices(Document["COLLADA"][0]["library_visual_scenes"][0]["visual_scene"][0]["node"][0]["matrix"][0].Data_Array, Bind_Poses_Array);

		Target_Skeleton->Bind_Shape_Matrix = Target_Skeleton->Bind_Shape_Matrix * Bind_Poses_Array[0];

		//

		return 0;
	}
}