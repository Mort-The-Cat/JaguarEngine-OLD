#include "Collada_Loader.h"

namespace Collada
{
	int Load_Mesh(const XML_Document& Document, Collada_Mesh* Target_Mesh, Collada_Skeleton* Skeleton) // 
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> Texture_Coordinates;

		// These all will be compiled into a series of vertices
		const std::vector<XML_Document> Geometries = Document["COLLADA"].back()["library_geometries"].back()["geometry"];
		
		for (size_t G = 0; G < Geometries.size(); G++) // G is the current geometry index
		{
			const XML_Document Mesh = Geometries[G]["mesh"][0]; // There is only ever 1 mesh per <geometry> node, but there can be many geometry nodes

			// source[0] is positions
			
			Load_Strings_To_Vectors(Mesh["source"][0]["float_array"][0].Data_Array, Positions);
			Load_Strings_To_Vectors(Mesh["source"][1]["float_array"][0].Data_Array, Normals);
			Load_Strings_To_Vectors(Mesh["source"][2]["float_array"][0].Data_Array, Texture_Coordinates);

			// source[1] is normals
			// source[2] is tex coords

			// triangles[0].p[0] are the vertices of each triangle in the buffer in order

			std::vector<std::string> Indices = Mesh["triangles"][0]["p"][0].Data_Array;

			size_t W = 0;
			while(W < Indices.size())
			{
				// Add each vertex

				Collada_Vertex Vertex;

				size_t Index = std::stoi(Indices[W++]);
				Vertex.Position = Positions[Index]; // *glm::vec3(-1.0f, 1.0f, 1.0f);

				if (Skeleton)
				{
					if (Skeleton->Vertex_Weights[Index].size())
					{
						Vertex.Joint_ID = 
							Skeleton->Vertex_Weights[Index][0].Joint_Index +
							(Skeleton->Vertex_Weights[Index][1].Joint_Index << 16);



						Vertex.Joint_Weight =
							(int)Skeleton->Vertex_Weights[Index][0].Weight +
							((int)Skeleton->Vertex_Weights[Index][1].Weight << 16);
					}
					else
					{
						Vertex.Joint_ID = 63 + (63 << 16);
						Vertex.Joint_Weight = 0.0f;
					}
				}

				Index = std::stoi(Indices[W++]);
				Vertex.Normal = Normals[Index];

				Index = std::stoi(Indices[W++]);
				Vertex.Texture_Coordinates = Texture_Coordinates[Index];

				Target_Mesh->Vertices.push_back(Vertex);
			}

			Positions.clear();
			Normals.clear();
			Texture_Coordinates.clear();
		}

		return 0;

		// This gives us the "mesh" data from COLLADA
	}
}