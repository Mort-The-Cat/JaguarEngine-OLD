#include "Collada_Loader.h"

namespace Collada
{
	int Load_Mesh(const XML_Document& Document, Collada_Mesh* Target_Mesh, Collada_Skeleton* Skeleton)
	{
		struct Geometry
		{
			const char* Name;

			std::vector<glm::vec3> Positions;
			std::vector<glm::vec3> Normals;
			std::vector<glm::vec2> Texture_Coordinates;

			std::vector<Collada_Vertex> Vertices;
		};

		std::vector<Geometry> Geometries_Data; // This is used to load each geometry etc

		// the objects in the selected file will be taken from 'visual_scenes'
		// and then copied into the target mesh accordingly

		const std::vector<XML_Document> Geometries = Document["COLLADA"].back()["library_geometries"].back()["geometry"];

		Geometries_Data.resize(Geometries.size());

		for (size_t G = 0; G < Geometries.size(); G++)
		{
			Geometries_Data[G].Name = Geometries[G].Id.c_str();

			const XML_Document Mesh = Geometries[G]["mesh"][0];

			Load_Strings_To_Vectors(Mesh["source"][0]["float_array"][0].Data_Array, Geometries_Data[G].Positions);
			Load_Strings_To_Vectors(Mesh["source"][1]["float_array"][0].Data_Array, Geometries_Data[G].Normals);
			Load_Strings_To_Vectors(Mesh["source"][2]["float_array"][0].Data_Array, Geometries_Data[G].Texture_Coordinates);

			std::vector<std::string> Indices = Mesh["triangles"][0]["p"][0].Data_Array;

			size_t W = 0;
			while (W < Indices.size())
			{
				Collada_Vertex Vertex;

				size_t Index = std::stoi(Indices[W++]);
				
				Vertex.Position = Geometries_Data[G].Positions[Index]; 

				// *glm::vec3(-1.0f, 1.0f, 1.0f);

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
				Vertex.Normal = Geometries_Data[G].Normals[Index]; //only rotates

				Index = std::stoi(Indices[W++]);
				Vertex.Texture_Coordinates = Geometries_Data[G].Texture_Coordinates[Index];

				Geometries_Data[G].Vertices.push_back(Vertex);
			}
		}

		// geometries are loaded... check for skins?

		std::vector<XML_Document> Skin_Controllers;

		if (Document["COLLADA"][0].Nodes.count("library_controllers"))
		{
			Skin_Controllers = Document["COLLADA"][0]["library_controllers"][0].Search_Children("skin");

			for (size_t Controller = 0; Controller < Skin_Controllers.size(); Controller++)
			{

				for (size_t Skin = 0; Skin < Skin_Controllers[Controller].Nodes["skin"].size(); Skin++)
				{

					size_t Geometry = 0;
					while (strcmp(Geometries_Data[Geometry].Name, Skin_Controllers[Controller]["skin"][Skin].URL.c_str() + 1))
						Geometry++;

					Geometries_Data[Geometry].Name = Skin_Controllers[Controller].Id.c_str();
				}
			}
		}

		// geometries are loaded... place objects!

		const XML_Document Visual_Scene = Document["COLLADA"][0]["library_visual_scenes"][0]["visual_scene"][0];

		const char* Instance_Type = "instance_geometry";

		std::vector<XML_Document> Nodes = Visual_Scene.Search_Children("instance_geometry");

		if (Nodes.size() == 0)
		{
			Nodes = Visual_Scene.Search_Children("instance_controller");
			Instance_Type = "instance_controller";
		}

		for (size_t Index = 0; Index < Nodes.size(); Index++)
		{
			bool Flipped_Faces = false;

			std::vector<glm::mat4> Node_Matrix;

			Load_Strings_To_Matrices(Nodes[Index]["matrix"][0].Data_Array, Node_Matrix);

			if (glm::determinant(glm::mat3(Node_Matrix[0])) < 0.0f)		// If the matrix flips the model somehow... make sure the vertex order is preserved!
				Flipped_Faces = true;

			const char* Geometry_URL = Nodes[Index][Instance_Type][0].URL.c_str() + 1; // skips '#' character in string

			size_t Geometry = 0;

			while (strcmp(Geometries_Data[Geometry].Name, Geometry_URL)) // as long as there's any difference....
				Geometry++;												// increment to check the next one

			// now? begin adding the positions etc to the target mesh

			for (size_t V = 0; V < Geometries_Data[Geometry].Vertices.size(); V++)
			{
				// will apply transformation matrix and add to target mesh accordingly

				Target_Mesh->Vertices.push_back(Geometries_Data[Geometry].Vertices[V]);
				Target_Mesh->Vertices.back().Position =
					Node_Matrix[0] *
					glm::vec4(
						Target_Mesh->Vertices.back().Position[0],
						Target_Mesh->Vertices.back().Position[1],
						Target_Mesh->Vertices.back().Position[2],
						1.0f
					);

				Target_Mesh->Vertices.back().Normal =
					glm::normalize(
						glm::mat3(Node_Matrix[0]) *
						Target_Mesh->Vertices.back().Normal
					);

				if ((V - 2) % 3 == 0 && Flipped_Faces)	// If we need to flip the faces, swap the 3rd and 2nd vertices of every triangle!
				{
					std::swap(Target_Mesh->Vertices[V], Target_Mesh->Vertices[V - 1]);
				}

				// all good! transformations applied
			}
		}

		return 0;
	}


	int Old_Load_Mesh(const XML_Document& Document, Collada_Mesh* Target_Mesh, Collada_Skeleton* Skeleton) // 
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> Texture_Coordinates;

		// These all will be compiled into a series of vertices
		const std::vector<XML_Document> Geometries = Document["COLLADA"].back()["library_geometries"].back()["geometry"];

		std::vector<glm::mat4> Node_Matrix;
		
		for (size_t G = 0; G < Geometries.size(); G++) // G is the current geometry index
		{
			const XML_Document Mesh = Geometries[G]["mesh"][0]; // There is only ever 1 mesh per <geometry> node, but there can be many geometry nodes

			// source[0] is positions

			Node_Matrix.clear();
			Load_Strings_To_Matrices(Document["COLLADA"][0]["library_visual_scenes"][0]["visual_scene"][0]["node"][G]["matrix"][0].Data_Array, Node_Matrix);

			// This'll get the matrix for this *geometry*
			
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
				//Vertex.Position = Positions[Index]; 
				
				// *glm::vec3(-1.0f, 1.0f, 1.0f);

				Vertex.Position = Node_Matrix[0] * glm::vec4(Positions[Index].x, Positions[Index].y, Positions[Index].z, 1.0f);

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
				Vertex.Normal = glm::mat3(Node_Matrix[0]) * Normals[Index]; //only rotates

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