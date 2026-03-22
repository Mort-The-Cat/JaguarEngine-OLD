#include "Shadow_Map_Lightmapping.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"
#include "../Controllers/Lightmap_Chart_Rasteriser.h"

#include<vector>
#include<algorithm>
#include<set>

namespace Jaguar
{

	class Sorted_List_Of_Lighting_Nodes
	{
	public:
		float Shortest_Length[4];
		size_t Shortest_Index[4];

		Sorted_List_Of_Lighting_Nodes(float Length)
		{
			// This initialises the distances etc and orders them accordingly

			for (size_t W = 0; W < 4; W++)
			{
				Shortest_Length[W] = Length;
				Shortest_Index[W] = 0;
			}
		}

		void Insert_Element(float New_Length, size_t Index)
		{
			// bubbles up from index 3

			Shortest_Length[3] = New_Length;
			Shortest_Index[3] = Index;

			size_t Bubble_Index = 3;
			while (Bubble_Index)	// (until it underflows to above 2)
			{
				if (Shortest_Length[Bubble_Index - 1] > Shortest_Length[Bubble_Index])
				{
					std::swap(Shortest_Length[Bubble_Index - 1], Shortest_Length[Bubble_Index]);
					std::swap(Shortest_Index[Bubble_Index - 1], Shortest_Index[Bubble_Index]);

					Bubble_Index--;
				}
				else
					return;
			}
		}
	};

	void Get_Nearest_Lighting_Node(const Lighting_Node_Data* Node_Data, glm::vec3 Position, const Lighting_Node** Target_Node)
	{
		// This just gets closest node

		*Target_Node = &Node_Data->Nodes[0];

		glm::vec3 Delta_Vector = Position - Target_Node[0]->Position;

		float Shortest_Distance = glm::dot(Delta_Vector, Delta_Vector);

		for (size_t Index = 1; Index < Node_Data->Nodes.size(); Index++)
		{
			bool Is_Closer = true;

			Delta_Vector = Position - Node_Data->Nodes[Index].Position;

			float Distance = glm::dot(Delta_Vector, Delta_Vector);

			if (Distance < Shortest_Distance)
			{
				Shortest_Distance = Distance;
				*Target_Node = &Node_Data->Nodes[Index];
			}
		}
	}

	void Get_Nearest_Lighting_Nodes(const Lighting_Node_Data* Node_Data, glm::vec3 Position, const Lighting_Node* Target_Nodes[4])
	{
		glm::vec3 Vector = Node_Data->Nodes[0].Position - Position;

		Sorted_List_Of_Lighting_Nodes List(glm::dot(Vector, Vector));

		for (size_t Index = 1; Index < Node_Data->Nodes.size(); Index++)
		{
			Vector = Node_Data->Nodes[Index].Position - Position;
			List.Insert_Element(glm::dot(Vector, Vector), Index);
		}

		for (size_t Index = 0; Index < 4; Index++)
			Target_Nodes[Index] = &Node_Data->Nodes[List.Shortest_Index[Index]];
	}

	bool Line_Intersects_Tri(Lightmap_Chart* Target_Chart, glm::vec3 Position, glm::vec3 To_Light_Vector, size_t Tri, float Epsilon = 0.0f);

	bool Flood_Fill_Lighting_Nodes_Check_Node(Lightmap_Chart* Target_Chart, glm::ivec3 Origin, glm::ivec3 Position, float Size, std::map<int, std::map<int, std::map<int, bool>>>& Grid, std::vector<glm::ivec3>& Node_Positions)
	{
		if (glm::length(glm::vec3(Origin) * Size) > 1000.0f)	// Just a little precaution to stop infinite leaking...
			return 0;

		if (Grid[Position.x][Position.y].find(Position.z) == Grid[Position.x][Position.y].end())
		{
			// CHECK if there's any intersection...

			bool Intersection = false;

			for (size_t Index = 0; Index < Target_Chart->Pushed_Tris.size() && !Intersection; Index++)
				Intersection = Line_Intersects_Tri(Target_Chart, glm::vec3(Origin) * Size, glm::vec3(Position - Origin) * Size, Index, 0.02f);

			if (Intersection)
				return 0;

			Grid[Position.x][Position.y][Position.z] = true;
			Node_Positions.push_back(Position);

			return 1;
		}

		return 0;
	}

	void Flood_Fill_Lighting_Nodes(Lightmap_Chart* Target_Chart, glm::vec3 Origin, float Size, Lighting_Data* Target_Lighting)
	{
		std::vector<glm::ivec3> Node_Positions = { glm::ivec3(Origin / glm::vec3(Size)) };
		std::map<int, std::map<int, std::map<int, bool>>> Grid;

		size_t Added = 1;

		while (Added)
		{
			size_t End = Node_Positions.size();
			size_t Index = End - Added;
			Added = 0;
			for (; Index < End; Index++)
			{
				// add new thing if there's space!

				const glm::ivec3 Deltas[6] =
				{
					{ 1, 0, 0 },
					{ 0, 1, 0 },
					{ 0, 0, 1 },
					{ -1, 0, 0 },
					{ 0, -1, 0 },
					{ 0, 0, -1 }
				};

				for (size_t Face = 0; Face < 6; Face++)
					Added += Flood_Fill_Lighting_Nodes_Check_Node(Target_Chart, Node_Positions[Index], Node_Positions[Index] + Deltas[Face], Size, Grid, Node_Positions);
			}
		}

		// Add all of the node_positions as actual lighting nodes

		Target_Lighting->Lighting_Nodes.Size = Size;

		Target_Lighting->Lighting_Nodes.Nodes.resize(Node_Positions.size());

		for (size_t Index = 0; Index < Node_Positions.size(); Index++)
		{
			Target_Lighting->Lighting_Nodes.Nodes[Index].Position = glm::vec3(Node_Positions[Index]) * glm::vec3(Size);
		}
	}

	void Delete_Scene_Lightmap(Lighting_Data* Target_Lighting)
	{
		// Lightmap_Texture needs to be deleted

		Target_Lighting->Lighting_Nodes.Nodes.clear();

#if TRIPLE_LIGHTMAPPING
		for (size_t W = 0; W < 3; W++)
		{
			glBindTexture(GL_TEXTURE_2D, Target_Lighting->Lightmap_Textures[W].Texture_Buffer_ID);
			Destroy_Texture_Buffer(&Target_Lighting->Lightmap_Textures[W]);
		}
#else
		glBindTexture(GL_TEXTURE_2D, Target_Lighting->Lightmap_Texture.Texture_Buffer_ID);
		Destroy_Texture_Buffer(&Target_Lighting->Lightmap_Texture);
#endif
	}

	float Lightmap_Simple_Area_Of_Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C)
	{
		glm::vec3 A_B = B - A;
		glm::vec3 A_C = C - A;

		return glm::length(glm::cross(A_B, A_C));
	}

	bool Line_Intersects_Tri(Lightmap_Chart* Target_Chart, glm::vec3 Position, glm::vec3 To_Light_Vector, size_t Tri, float Epsilon)
	{
		// Get tri transformed points
		// Get tri normal

		// Transform Position/To_Light_Vector into TBN space

		// Check if there is an overlap against the Z axis
		// If there is, check if within triangle at that point

		/*glm::vec3 Points[3];
		for (size_t W = 0; W < 3; W++)
		{
			Points[W] = Get_Model_Matrix(Data->Target_Chart->Pushed_Objects[Data->Target_Chart->Pushed_Tris[Tri].Model_Index]) *
				glm::vec4(
					Data->Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Data->Target_Chart->Pushed_Tris[Tri].Index + W].Position, 1);
		}*/

		glm::vec3 A_B = Target_Chart->Pushed_Tris[Tri].Points[1] - Target_Chart->Pushed_Tris[Tri].Points[0];
		glm::vec3 A_C = Target_Chart->Pushed_Tris[Tri].Points[2] - Target_Chart->Pushed_Tris[Tri].Points[0];
		glm::vec3 Normal = Target_Chart->Pushed_Tris[Tri].TBN[2]; // It doesn't actually matter if this is facing the right way

		Position -= Target_Chart->Pushed_Tris[Tri].Points[0];

		glm::vec3 T_Position = Position;
		glm::vec3 T_To_Light_Vector = To_Light_Vector;

		T_Position.z = glm::dot(Position, Normal);
		T_To_Light_Vector.z = glm::dot(To_Light_Vector, Normal);

		if ((T_Position.z < 0) == (T_To_Light_Vector.z + T_Position.z < 0))	// If they don't overlap the Z axis
			return false;													// there isn't an intersect to test

		float Factor = T_Position.z / T_To_Light_Vector.z;

		if (Factor > 0 || Factor < -1)										// if outside of bounds of the ray, it doesn't intersect either
			return false;

		// Use the area-method to check if the point lies within the triangle

		T_Position = Position - To_Light_Vector * Factor;

		float Area[4] = {
			Lightmap_Simple_Area_Of_Triangle(glm::vec3(0.0f), A_B, A_C),

			Lightmap_Simple_Area_Of_Triangle(T_Position, A_B, A_C),
			Lightmap_Simple_Area_Of_Triangle(T_Position, A_C, glm::vec3(0.0f)),
			Lightmap_Simple_Area_Of_Triangle(T_Position, A_B, glm::vec3(0.0f))
		};

		return Area[1] + Area[2] + Area[3] <= Area[0] + Epsilon;// +0.002f;
	}

	//

	const float Luxel_Scale = 35.0f;
	void Init_Lightmap_Chart(Lightmap_Chart* Target_Chart)
	{
		Target_Chart->Sidelength = 1u;
		Target_Chart->Occupied.resize(1);
		Target_Chart->Occupied[0] = { 0 }; // Not occupied
	}

	void Upsize_Chart(Lightmap_Chart* Target_Chart)
	{
		Target_Chart->Sidelength += 8; //  increases sidelength
		Target_Chart->Occupied.resize(Target_Chart->Sidelength);

		for (size_t W = 0; W < Target_Chart->Sidelength; W++)
			Target_Chart->Occupied[W].resize(Target_Chart->Sidelength);
	}

	bool Perpixel_Rasterise_Check(size_t X, size_t Y, int, void* Data)
	{
		const Lightmap_Chart* Target_Chart = (const Lightmap_Chart*)Data;

		if (Target_Chart->Occupied[X][Y])
			return true;					// Hit!!

		return false;						// No hit
	}

	bool Perpixel_Rasterise_Fill(size_t X, size_t Y, int, void* Data)
	{
		Lightmap_Chart* Target_Chart = (Lightmap_Chart*)Data;

		Target_Chart->Occupied[X][Y] = true;

		return false;	// No hit, so continue as normal!
	}

	bool Find_New_Location_Lightmap_Chart(Jaguar_Engine* Engine, int Area, int* X, int* Y, const Lightmap_Chart* Target_Chart, glm::vec2* Projected_Points)
	{
		// Find area closest to top-left to place square

		unsigned int Max = Target_Chart->Sidelength - LIGHTMAP_CHART_PADDING;

		int Begin_X = LIGHTMAP_CHART_PADDING;
		int Begin_Y = LIGHTMAP_CHART_PADDING;

		while (Begin_Y < Target_Chart->Sidelength)
		{
			*X = Begin_X;
			*Y = Begin_Y;

			//while (*X >= LIGHTMAP_CHART_PADDING && *Y < Max)

			while (*X < Max)
			{
				/*if (Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Check>(
					Projected_Points[0] + glm::vec2(*X, *Y),
					Projected_Points[1] + glm::vec2(*X, *Y),
					Projected_Points[2] + glm::vec2(*X, *Y),
					0, 0, 0, Target_Chart->Sidelength, (void*)Target_Chart
				))*/

				if (Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Check>(
					Projected_Points[0] + glm::vec2(*X, *Y),
					Projected_Points[1] + glm::vec2(*X, *Y),
					Projected_Points[2] + glm::vec2(*X, *Y),
					0, 0, 0, Target_Chart->Sidelength, (void*)Target_Chart
				))

					//if (Check_Chart_Square_Area(Area, *X, *Y, Target_Chart))
					return true;

				//(*X)--;
				//(*Y)++;
				(*X) += 2;
			}

			bool Condition = Begin_X < Max;
			// Begin_X += Condition;
			Begin_Y += 2; //!Condition;
		}

		return false;
	}

	bool Lightmap_Tri_Sort_Compare(const Lightmap_Tri& A, const Lightmap_Tri& B)
	{
		return A.Size > B.Size;
	}

	void Lightmap_Chart_Get_Projection(Collada::Collada_Mesh* Mesh, size_t Index, glm::vec2* Projected_Points, float Scale)
	{
		glm::vec3 A_B = Mesh->Vertices[Index + 1].Position - Mesh->Vertices[Index].Position;
		glm::vec3 A_C = Mesh->Vertices[Index + 2].Position - Mesh->Vertices[Index].Position;

		glm::vec3 Tangent = glm::normalize(A_B);
		glm::vec3 Bitangent = glm::normalize(A_C);

		glm::vec3 Normal = glm::normalize(glm::cross(Tangent, Bitangent));

		Bitangent = -glm::normalize(glm::cross(Tangent, Normal));

		Projected_Points[1] = glm::vec2(glm::ceil(Scale * glm::dot(Tangent, A_B)), 0.0f);
		Projected_Points[2] = glm::vec2(
			glm::ceil(Scale * glm::dot(Tangent, A_C)),
			glm::ceil(Scale * glm::dot(Bitangent, A_C))
		);
	}

	void Apply_Baked_Lightmap_Chart(Jaguar_Engine* Engine, const std::vector<Baked_Lightmap_Chart>& Chart)
	{
		for (size_t Index = 0; Index < Chart.size(); Index++)
		{
			Mesh_Cache_Info Mesh = Pull_Mesh(&Engine->Asset_Cache, Chart[Index].Mesh_Name.c_str(), LOAD_MESH_HINT_LIGHTMAP_STATIC);

			for (size_t Vertex = 0; Vertex < Chart[Index].Lightmap_Coords.size(); Vertex++)
			{
				Mesh.Mesh->Vertices[Vertex].Lightmap_UV = Chart[Index].Lightmap_Coords[Vertex];
			}

			// then we just update the vertex data etc

			Bind_Vertex_Buffer(Mesh.Buffer);
			Update_Vertex_Buffer_Data(Mesh.Mesh, &Mesh.Buffer);
		}
	}

	void Chart_Tri_Init_Points(Lightmap_Chart* Target_Chart, size_t Tri)
	{
		glm::vec2 Texture_Coordinates[3];

		for (size_t Point = 0; Point < 3; Point++)
		{
			Target_Chart->Pushed_Tris[Tri].Points[Point] = Get_Model_Matrix(Target_Chart->Pushed_Objects[Target_Chart->Pushed_Tris[Tri].Model_Index]) * glm::vec4(Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Position, 1);

			Texture_Coordinates[Point] = Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Texture_Coordinates;
		}

		Target_Chart->Pushed_Tris[Tri].TBN[0] = Target_Chart->Pushed_Tris[Tri].Points[1] - Target_Chart->Pushed_Tris[Tri].Points[0];
		Target_Chart->Pushed_Tris[Tri].TBN[1] = Target_Chart->Pushed_Tris[Tri].Points[2] - Target_Chart->Pushed_Tris[Tri].Points[0];
		Target_Chart->Pushed_Tris[Tri].TBN[2] = glm::normalize(glm::cross(Target_Chart->Pushed_Tris[Tri].TBN[1], Target_Chart->Pushed_Tris[Tri].TBN[0]));

		Jaguar::Get_UV_Tangent_Bitangent_Vectors(Target_Chart->Pushed_Tris[Tri].Points, Texture_Coordinates, Target_Chart->Pushed_Tris[Tri].TBN[2], &Target_Chart->Pushed_Tris[Tri].TBN[0], &Target_Chart->Pushed_Tris[Tri].TBN[1]);

		Jaguar::Get_Triple_Lightmap_Vectors(Target_Chart->Pushed_Tris[Tri].TBN[2], Target_Chart->Pushed_Tris[Tri].TBN[0], Target_Chart->Pushed_Tris[Tri].TBN[1], Target_Chart->Pushed_Tris[Tri].Triple_Vectors); // gets 3 components necessary for normal mapping
	}

	void Assemble_Lightmap_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* File_Directory = nullptr)
	{
		std::sort(Target_Chart->Pushed_Tris.begin(), Target_Chart->Pushed_Tris.end(), Lightmap_Tri_Sort_Compare); // Sorts them accordingly

		// This is because we want to place the largest tris first and fit the smaller ones between them afterwards wherever possible

		std::vector<Mesh_Cache_Info> Updated_Meshes;

		glm::vec2 Projected_Points[3];
		Projected_Points[0] = glm::vec2(0.0f);

		for (size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
		{
			//int Square_Size = Target_Chart->Pushed_Tris[Tri].Size;
			Mesh_Cache_Info Mesh_Info = Target_Chart->Pushed_Tris[Tri].Mesh;

			if (std::find(Updated_Meshes.begin(), Updated_Meshes.end(), Mesh_Info) == Updated_Meshes.end())	// if not already in vector
				Updated_Meshes.push_back(Mesh_Info);														// add to back of it

			size_t Triangle = Target_Chart->Pushed_Tris[Tri].Index;

			int X, Y;

			Lightmap_Chart_Get_Projection(Mesh_Info.Mesh, Triangle, Projected_Points, Luxel_Scale);

			while (!Find_New_Location_Lightmap_Chart(Engine, 0, &X, &Y, Target_Chart, Projected_Points))
				Upsize_Chart(Target_Chart); // Doubles size until there's room somewhere on chart

			Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Fill>(
				Projected_Points[0] + glm::vec2(X, Y),
				Projected_Points[1] + glm::vec2(X, Y),
				Projected_Points[2] + glm::vec2(X, Y),
				0, 0, 0,
				Target_Chart->Sidelength,
				Target_Chart
			);

			/*Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Fill>(
				glm::vec2(0.125f) * (Projected_Points[0] + glm::vec2(X, Y)) + glm::vec2(0.125f * LIGHTMAP_CHART_PADDING),
				glm::vec2(0.125f) * (Projected_Points[1] + glm::vec2(X, Y)) + glm::vec2(0.125f * LIGHTMAP_CHART_PADDING),
				glm::vec2(0.125f) * (Projected_Points[2] + glm::vec2(X, Y)) + glm::vec2(0.125f * LIGHTMAP_CHART_PADDING),
				0, 0, 0,
				Target_Chart->Sidelength * 0.125,
				Target_Chart
			);*/

			Chart_Tri_Init_Points(Target_Chart, Tri);

			for (size_t Point = 0; Point < 3; Point++)
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV = glm::vec2(X, Y) + Projected_Points[Point];
		}

		if (File_Directory)
			Jaguar::Write_Lightmap_Chart_To_File(File_Directory, Updated_Meshes);	// This writes it to a file

		for (auto& Mesh : Updated_Meshes)	// Updates all of the meshes that need to be updated once
		{
			Bind_Vertex_Buffer(Mesh.Buffer);
			Update_Vertex_Buffer_Data(Mesh.Mesh, &Mesh.Buffer);
		}
	}

	void Push_Queue_Lightmap_Chart(Jaguar_Engine* Engine, const Render_Queue* Queue, Lightmap_Chart* Target_Chart)
	{
		// This will push all of the elements in a specific render queue to a lightmap chart
		// This chart will be used to map model triangles to areas of the lightmap, scaling it dynamically.

		// From there, other functions will be called to create and fill a corresponding texture via a renderbuffer

		// This uses a fancier method than before, fitting accurately projected and scaled triangles onto the chart

		for (size_t Object = 0; Object < Queue->Objects.size(); Object++)
		{
			Target_Chart->Pushed_Objects.push_back(Queue->Objects[Object]);

			Mesh_Cache_Info Mesh_Info = Get_Mesh_From_Buffer_ID(&Engine->Asset_Cache, Queue->Objects[Object]->Mesh.Vertex_Buffer_ID);

			for (size_t Triangle = 0; Triangle < Mesh_Info.Mesh->Vertices.size(); Triangle += 3) // Every 3 verts is a tri
			{
				// Get tri area

				glm::vec3 Points[3];

				for (size_t Vert = 0; Vert < 3; Vert++)
					Points[Vert] = Mesh_Info.Mesh->Vertices[Vert + Triangle].Position;

				Points[1] -= Points[0];
				Points[2] -= Points[0];

				float Square_Size = std::sqrtf(
					Luxel_Scale * glm::length(
						glm::cross(Points[1], Points[2])
					)
				);

				Target_Chart->Pushed_Tris.push_back(Lightmap_Tri(Mesh_Info, Triangle, Square_Size, Target_Chart->Pushed_Objects.size() - 1));

				// Square_Size is just the rough size of the tri here
			}
		}
	}
}