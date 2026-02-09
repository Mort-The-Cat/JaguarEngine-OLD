#ifndef JAGUAR_LIGHTMAPPING
#define JAGUAR_LIGHTMAPPING

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../Collada_Loader/Collada_Loader.h"
#include "../Controllers/Asset_Cache.h"

#include<mutex>
#include<set>

namespace Jaguar
{
	class Lighting_Node	// This is used for applying approximate baked lighting to dynamic objects
	{
	public:
		glm::vec3 Position;
		glm::vec3 Illumination[6] = { glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0) };	// six of these for all 6 faces
			// 0,1,2 is xyz positive and +3 is negative

		// for now, we'll default the position to (0,0,0)

		size_t Adjacent_Indices[6];

		Lighting_Node() {}
		Lighting_Node(glm::vec3 Positionp)
		{
			Position = Positionp;
		}
	};

#define LF_STATIC 1u
	struct Lightsource
	{
		glm::vec3 Colour;
		glm::vec3 Position;
		glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);	// This is a direction vector
		float Radius = 1.0f;	// Physical radius of this light

		// bool Bounced = false;

		// I'll implement FOV and other things later

		float FOV = 360.0f;

		bool Flags[1] = { false };

	};

	// Lightmapping structures go here

	class Lightmap_Tri
	{
	public:
		size_t Index;
		float Size;
		Mesh_Cache_Info Mesh;
		unsigned int Model_Index;

		glm::vec3 Points[3];
		glm::vec3 TBN[3];
		glm::vec3 Triple_Vectors[3];

		Lightmap_Tri(Mesh_Cache_Info Meshp, size_t Indexp, int Sizep, unsigned int Model_Indexp)
		{
			Mesh = Meshp;
			Index = Indexp;
			Size = Sizep;
			Model_Index = Model_Indexp;
		}
	};

	struct World_Object;

	class Lightmap_Chart
	{
	public:
		// This stores all of the occupied pixels.
		// The mesh objects are updated to handle all of the lightmap UVs

		std::vector<std::vector<unsigned char>> Occupied; // Sidelength x sidelength long

		std::vector<Lightmap_Tri> Pushed_Tris;
		//
		//std::vector<std::set<size_t>> Tri_Visible_Lightsource_Indices;	// This is done using a C++ set
		//std::mutex Tri_Visible_Lightsource_Mutex;
		//

		glm::vec3 Blockmap_Origin;
		std::vector<std::vector<std::vector<std::set<size_t>>>> Tri_Broadphase_Blockmap; 
							// 3d blockmap, each contains set of indices, 
							// corresponding to the tris present in that grid section
							// This will use a LOT of memory and bandwidth,
							// but it should speed up lightmap generation massively

		//std::map<long, std::map<long, std::map<long, std::set<size_t>>>> Tri_Broadphase_Blockmap;

		std::vector<World_Object*> Pushed_Objects;

		// 0 = not occupied, 1 = occupied

		unsigned int Sidelength;

		unsigned char Bounced_Lighting = false;	// This tells us if we've begun work on bounced-lighting yet
		// Lightmap charts are always squares so sidelength is all that's necessary
	};

	struct Baked_Lightmap_Chart
	{
		std::vector<glm::vec2> Lightmap_Coords;	// saved lightmap coords
		std::string Mesh_Name;					// Name of mesh
	};

	struct Render_Queue;
	struct Jaguar_Engine;

	struct Lighting_Node_Data
	{
		std::vector<Lighting_Node> Nodes;
		float Size;					// This is the distance between nodes
	};

	void Get_Nearest_Lighting_Nodes(const Lighting_Node_Data* Node_Data, glm::vec3 Position, const Lighting_Node* Target_Nodes[4]);

	void Get_Nearest_Lighting_Node(const Lighting_Node_Data* Node_Data, glm::vec3 Position, const Lighting_Node** Target_Node);

	void Init_Lightmap_Chart(Lightmap_Chart* Target_Chart);

	void Assemble_Lightmap_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* File_Directory);
	void Push_Queue_Lightmap_Chart(Jaguar_Engine* Engine, const Render_Queue* Queue, Lightmap_Chart* Target_Chart);
	//void Create_Lightmap_From_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* Filename = nullptr);
	void Create_Lightmap3_From_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* Filename = nullptr);

	struct Lighting_Data;

	//void Get_Lightmap_From_File(const char* Filename, Lighting_Data* Target_Lighting);	// Loads lightmap data and generates texture from it
	//void Write_Lightmap_To_File(const char* Filename, glm::vec3* Data, unsigned int Texture_Dimensions);

	// Regular lightmap has been deprecated in favour of 3-vector lightmap

	void Get_Lightmap3_From_File(const char* Filename, Lighting_Data* Target_Lighting);
	void Write_Lightmap3_To_File(const char* Filename, glm::vec3* Data[3], unsigned int Texture_Dimensions);

	//

	void Flood_Fill_Lighting_Nodes(Lightmap_Chart* Target_Chart, glm::vec3 Origin, float Size, Lighting_Data* Target_Lighting);

	void Write_Lightmap_Chart_To_File(const char* Filename, const std::vector<Mesh_Cache_Info>& Updated_Meshes);
	void Get_Lightmap_Chart_From_File(const char* Filename, std::vector<Baked_Lightmap_Chart>& Lightmap_Charts, Asset_Cache_Data* Asset_Cache);
	void Apply_Baked_Lightmap_Chart(Jaguar_Engine* Engine, const std::vector<Baked_Lightmap_Chart>& Chart);

	void Write_Lighting_Nodes_To_File(const char* Filename, const Lighting_Node_Data& Node_Data);
	void Get_Lighting_Nodes_From_File(const char* Filename, Lighting_Node_Data& Node_Data);

	struct Shader;
	struct Scene_Data;

	void Lightmapped_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene);

	void Upsize_Chart(Lightmap_Chart* Target_Chart);
}

#endif