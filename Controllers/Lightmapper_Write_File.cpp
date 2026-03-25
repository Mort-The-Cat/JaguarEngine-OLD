//#include "Lightmapping.h"

#include "../Lightmapping/Shadow_Map_Lightmapping.h"

#include "../OpenGL_Handling/Scene.h"
#include "../Collada_Loader/Collada_Loader.h"

#include "../Jaguar_Compression/Compression.h"

#include<set>

namespace Jaguar
{


	void Write_Lightmap3_To_File(Job_System* System, const char* Filename, glm::vec3* Data[3], unsigned int Texture_Dimensions, bool Compress)
	{
		size_t Texture_Size = Texture_Dimensions;
		Texture_Size *= Texture_Size * sizeof(glm::vec3);

		Openzip::Compressor_Data Compressor;

		if (Compress)
		{
			Compressor.Raw_Data.resize(Texture_Dimensions * Texture_Dimensions * 3 * 3);

			for (size_t W = 0; W < 3; W++)
				memcpy(Compressor.Raw_Data.data() + (Texture_Dimensions * Texture_Dimensions * W * 3), Data[W], Texture_Size);

			Openzip::Compress_Raw_Data(System, Compressor);
		}

		std::ofstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR opening output lightmap file!\n");

		File.write((const char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		if (Compress)
		{
			File.write((const char*)Compressor.Instructions.data(), Compressor.Instructions.size());
		}
		else
		{

			for (size_t W = 0; W < 3; W++)
				File.write((const char*)Data[W], Texture_Size);

		}

		File.close();
	}

	void Write_Lightmap_To_File(const char* Filename, glm::vec3* Data, unsigned int Texture_Dimensions)
	{
		// Needs to load binary data into file

		std::ofstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR opening output lightmap file!\n");			// Throw_Error(" >> Error opening lightmap output file! %s\n");
		
		size_t Texture_Size = Texture_Dimensions;
		Texture_Size *= Texture_Size * sizeof(glm::vec3);	// Gets texture size in bytes

		File.write((const char*)&Texture_Dimensions, sizeof(Texture_Dimensions));			// writes texture size into file firstly

		File.write((const char*)Data, Texture_Size);										// Then writes rest of file into the binary

		File.close();

		// Done!
	}


	void Get_Lightmap_From_File(const char* Filename, Lighting_Data* Target_Lighting)	// Loads lightmap data and generates texture from it
	{
		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR reading lightmap file!\n");					// Throw_Error(" >> Error reading lightmap file! %s\n");

		unsigned int Texture_Dimensions;

		File.read((char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		size_t Texture_Size = Texture_Dimensions * Texture_Dimensions; // Squares area

		// Reads texture dimensions!

		// Time to allocate data...

		glm::vec3* Data = new glm::vec3[Texture_Size];

		File.read((char*)Data, Texture_Size * sizeof(glm::vec3));

		File.close();
#if !TRIPLE_LIGHTMAPPING
		Create_Texture_Buffer(&Target_Lighting->Lightmap_Texture, GL_RGB32F, Texture_Dimensions, Texture_Dimensions, GL_RGB, GL_FLOAT, Data, true);
#endif

		Target_Lighting->Inverse_Lightmap_Scale = 1.0f / (float)Texture_Dimensions;

		// Creates lightmap! Now we can deallocate the data

		delete[] Data;
	}

	void Get_Lightmap3_From_File(const char* Filename, Lighting_Data* Target_Lighting, bool Compress)
	{
		Openzip::Compressor_Data Compressor;

		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
		{
			glm::vec3 Data[16 * 16];
			for (size_t Index = 0; Index < 16 * 16; Index++)
				Data[Index] = glm::vec3(1.0f);

			for(size_t Axis = 0; Axis < 3; Axis++)
				Create_Texture_Buffer(&Target_Lighting->Lightmap_Textures[Axis], GL_RGB32F, 16, 16, GL_RGB, GL_FLOAT, Data, true);

			printf(" >> ERROR reading lightmap file!\n");

			return;
		}

		File.seekg(0, std::ios::end);
		std::streamsize Instructions_Count = File.tellg();
		Instructions_Count -= sizeof(unsigned int);
		File.seekg(0, std::ios::beg);

		unsigned int Texture_Dimensions;
		File.read((char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		size_t Texture_Size = Texture_Dimensions * Texture_Dimensions;

		Target_Lighting->Inverse_Lightmap_Scale = 1.0f / (float)Texture_Dimensions;

		if (Compress)
		{
			Compressor.Instructions.resize(Instructions_Count);
			File.read((char*)Compressor.Instructions.data(), Instructions_Count);

			File.close();

			Openzip::Decompress_Raw_Data(Compressor);
		}

		glm::vec3* Data = new glm::vec3[Texture_Size];

		for (size_t W = 0; W < 3; W++)
		{
			if (Compress)
			{
				memcpy(Data, Compressor.Raw_Data.data() + Texture_Size * 3 * W, Texture_Size * sizeof(glm::vec3));
			}
			else
				File.read((char*)Data, Texture_Size * sizeof(glm::vec3));

#if TRIPLE_LIGHTMAPPING
			Create_Texture_Buffer(&Target_Lighting->Lightmap_Textures[W], GL_RGB32F, Texture_Dimensions, Texture_Dimensions, GL_RGB, GL_FLOAT, Data, true);
#endif
		}

		if(!Compress)
			File.close();

		delete[] Data;
	}

	//

	void Get_Triple_Lightmap_Vectors(glm::vec3 Normal, glm::vec3 Tangent, glm::vec3 Bitangent, glm::vec3 Components[3])
	{
		// "Components" is the return array

		const float Sqrt_Sixth = sqrtf(1.0f / 6.0f);
		const float Sqrt_Half = sqrtf(0.5f);
		const float Sqrt_Third = sqrtf(1.0f / 3.0f);
		const float Sqrt_Three_Halves = sqrtf(2.0f / 3.0f);

		//Normal = glm::normalize(Normal);
		//Tangent = glm::normalize(Tangent);
		//Bitangent = glm::normalize(Bitangent);

		Normal *= Sqrt_Third;

		Bitangent *= Sqrt_Half;

		for (size_t W = 0; W < 3; W++)
			Components[W] = Normal;

		Components[2] += Sqrt_Three_Halves * Tangent;

		Tangent *= Sqrt_Sixth;

		for (size_t W = 0; W < 2; W++)
			Components[W] -= Tangent;

		Components[0] -= Bitangent;
		Components[1] += Bitangent;

		// Done! little fiddly to compute but it's fine
		// I'll come up with a faster method for the shader code
	}

	void Get_UV_Tangent_Bitangent_Vectors(glm::vec3 Positions[3], glm::vec2 Texture_Coordinates[3], glm::vec3 Normal, glm::vec3* Tangent, glm::vec3* Bitangent)
	{
		glm::vec2 UV_A, UV_B;
		glm::vec3 Edge_A, Edge_B;

		float Inv;

		UV_A = Texture_Coordinates[1] - Texture_Coordinates[0];
		UV_B = Texture_Coordinates[2] - Texture_Coordinates[0];

		Edge_A = Positions[1] - Positions[0];
		Edge_B = Positions[2] - Positions[0];

		Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

		*Tangent = -glm::normalize(Inv * (Edge_A * UV_B.y - Edge_B * UV_A.y));
		*Bitangent = glm::normalize(glm::cross(*Tangent, Normal));
	}

	//

	// Here, I'll handle the lightmap chart results and writing/reading them to/from files

	void Get_Lightmap_Chart_From_File(const char* Filename, std::vector<Baked_Lightmap_Chart>& Lightmap_Charts, Asset_Cache_Data* Asset_Cache)
	{
		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
		{
			printf("ERROR reading lightmap chart file! %s\n", Filename);

			return;
		}

		std::string Chart_Name;

		while (std::getline(File, Chart_Name, '\0'))
		{
			// read mesh name

			Baked_Lightmap_Chart Chart;

			Chart.Mesh_Name = std::move(Chart_Name);
			Chart_Name = std::string();

			//std::getline(File, Chart.Mesh_Name, '\0');	// null-terminated string

			Chart.Lightmap_Coords.resize(Pull_Mesh(Asset_Cache, Chart.Mesh_Name.c_str(), LOAD_MESH_HINT_LIGHTMAP_STATIC).Mesh->Vertices.size()); // We know these will be static
			File.read((char*)Chart.Lightmap_Coords.data(), Chart.Lightmap_Coords.size() * sizeof(glm::vec2));

			// Once data is read, push to lightmap charts

			Lightmap_Charts.push_back(std::move(Chart));
		}

		File.close();
	}

	void Write_Lightmap_Chart_To_File(const char* Filename, const std::vector<Mesh_Cache_Info>& Updated_Meshes)
	{
		std::ofstream File(Filename, std::ios::binary);

		// For each object, we state how many vertices it is in a 32-bit unsigned integer
		// Then, we write the data for all of those vertices

		// this repeats for all meshes until we've reached the end

		if (!File.is_open())
		{
			printf("ERROR writing lightmap chart file! %s\n", Filename);

			return;
		}

		for (const auto& Mesh : Updated_Meshes)
		{
			File.write(Mesh.Name, strlen(Mesh.Name) + 1); // include NULL-terminator character

			// writes the number of vertices in the 32-bit count there

			for (size_t Index = 0; Index < Mesh.Mesh->Vertices.size(); Index++)
			{
				File.write((const char*)&Mesh.Mesh->Vertices[Index].Lightmap_UV, sizeof(Mesh.Mesh->Vertices[Index].Lightmap_UV));
				// This just writes the glm::vec2 data directly
			}
		}

		File.close();	// We're done! Beautiful
	}

	//

	void Write_Lighting_Nodes_To_File(const char* Filename, const Lighting_Node_Data& Node_Data)
	{
		// This will just add all of the lighting node data 1:1 into the file

		std::ofstream File(Filename, std::ios::binary);

		if (!File.is_open())
		{
			printf("ERROR writing to lighting node file %s !\n", Filename);

			return;
		}

		File.write((const char*)&Node_Data.Size, sizeof(Node_Data.Size));

		for (size_t W = 0; W < Node_Data.Nodes.size(); W++)
		{
			File.write((const char*)(Node_Data.Nodes.data() + W), sizeof(Lighting_Node::Position) + sizeof(Lighting_Node::Illumination));

			// These are the only things that are written to the file
			// adjacent indices etc are evaluated during file loading
		}

		// File.write((const char*)Node_Data.Nodes.data(), Node_Data.Nodes.size() * sizeof(Lighting_Node));

		File.close();
	}

	void Get_Lighting_Nodes_From_File(const char* Filename, Lighting_Node_Data& Node_Data)
	{
		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
		{
			printf("ERROR reading from lighting node file! %s\n", Filename);

			Node_Data.Nodes.push_back(Lighting_Node(glm::vec3(0.f, 0.8f, -0.4f)));	// Eh we just want a random node (at least one)

			for (size_t Index = 0; Index < 6; Index++)
				Node_Data.Nodes.back().Illumination[Index] = glm::vec3(1.0f);

			memset(Node_Data.Nodes.back().Adjacent_Indices, 0, sizeof(Lighting_Node::Adjacent_Indices));

			return;
		}

		size_t Index = 0;

		File.read((char*)&Node_Data.Size, sizeof(Node_Data.Size));

		while (File.peek() != EOF)
		{
			Node_Data.Nodes.resize(Index + 1);	// increase size by 1

			// write to buffer

			File.read((char*)(Node_Data.Nodes.data() + Index), sizeof(Lighting_Node::Position) + sizeof(Lighting_Node::Illumination));

			// File.read((char*)(Node_Data.Nodes.data() + Index), sizeof(Lighting_Node));

			Index++;
		}

		File.close();

		// Once we've loaded the file, set up the *adjacent* indices

		// for all 6 directions, find node that fits criteria

		// Otherwise, reference self

		

		// For each node,
			// for each cardinal direction,
			// search for the immediate (direction) node
			// if found?
				// reference that index
			// if not?
				// reference self

		for (size_t Index = 0; Index < Node_Data.Nodes.size(); Index++)
		{
			glm::vec3 Directions[] = 
			{
				glm::vec3(1, 0, 0),
				glm::vec3(0, 1, 0),
				glm::vec3(0, 0, 1),

				glm::vec3(-1, 0, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, 0, -1)
			};

			for (size_t Direction = 0; Direction < 6; Direction++)
			{
				size_t Found_Index = Index;

				for (size_t Search_Index = 0; Search_Index < Node_Data.Nodes.size(); Search_Index++)
					if (Node_Data.Nodes[Search_Index].Position == (Node_Data.Nodes[Index].Position + Directions[Direction] * Node_Data.Size))
					{
						Found_Index = Search_Index;
						break;
					}

				Node_Data.Nodes[Index].Adjacent_Indices[Direction] = Found_Index;
			}
		}

		// Got adjacent indices! Easy
	}
}