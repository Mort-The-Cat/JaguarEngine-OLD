#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	void Update_Mesh_Hitbox_Normals(Mesh_Hitbox* Target)
	{
		for (size_t Index = 0; Index < Target->Faces.size(); Index++)
		{
			glm::vec3 Triangle[3] =
			{
				Target->Transformed_Points[Target->Faces[Index].Point_Index[0]],
				Target->Transformed_Points[Target->Faces[Index].Point_Index[1]],
				Target->Transformed_Points[Target->Faces[Index].Point_Index[2]]
			};

			Triangle[1] -= Triangle[0];
			Triangle[2] -= Triangle[0];

			Target->Faces[Index].Transformed_Normal = glm::normalize(glm::cross(Triangle[1], Triangle[2]));	// I think this is the correct normal direction
		}
	}

	void Mesh_Hitbox::Update_Hitbox()
	{
		glm::mat3 Matrix;

		Matrix = Get_Model_Matrix(Object);

		for (size_t Index = 0; Index < Transformed_Points.size(); Index++)
			Transformed_Points[Index] = Matrix * Points[Index];					// rotates around origin

		Update_Mesh_Hitbox_Normals(this);
	}

	size_t Add_Vertex_To_Mesh(std::vector<glm::vec3>& Points, glm::vec3 Point)
	{
		size_t Index;
		
		for (Index = 0; Index < Points.size(); Index++)
			if (Points[Index] == Point)
				return Index;

		// otherwise?

		Points.push_back(Point);

		return Index;
	}

	void Create_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh, Mesh_Hitbox* Target_Hitbox)
	{
		for (size_t Index = 0; Index < Model_Mesh->Vertices.size(); Index += 3)
		{
			size_t Indices[3] =
			{
				Add_Vertex_To_Mesh(Target_Hitbox->Points, Model_Mesh->Vertices[Index].Position),
				Add_Vertex_To_Mesh(Target_Hitbox->Points, Model_Mesh->Vertices[Index + 1].Position),
				Add_Vertex_To_Mesh(Target_Hitbox->Points, Model_Mesh->Vertices[Index + 2].Position)
			};

			Target_Hitbox->Faces.push_back(
				Mesh_Hitbox::Face(
					{
						Indices[0],
						Indices[1],
						Indices[2]
					},
					glm::vec3(0.0f)
				)
			);
		}

		Target_Hitbox->Transformed_Points.resize(Target_Hitbox->Points.size());
	}

	std::vector<Hitbox*> Wrap_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh)
	{
		std::vector<Hitbox*> Hitboxes(1);

		Hitboxes[0] = new Mesh_Hitbox();

		Create_Mesh_Hitbox(Model_Mesh, (Mesh_Hitbox*)Hitboxes[0]);

		return Hitboxes;
	}

	Mesh_Hitbox Mesh_Hitbox_From_AABB_Hitbox(AABB_Hitbox* AABB)
	{
		Mesh_Hitbox Mesh;

		Mesh.Transformed_Points =
		{
			glm::vec3(AABB->A.x, AABB->A.y, AABB->A.z),
			glm::vec3(AABB->A.x, AABB->A.y, AABB->B.z),
			glm::vec3(AABB->A.x, AABB->B.y, AABB->A.z),
			glm::vec3(AABB->A.x, AABB->B.y, AABB->B.z),
			glm::vec3(AABB->B.x, AABB->A.y, AABB->A.z),
			glm::vec3(AABB->B.x, AABB->A.y, AABB->B.z),
			glm::vec3(AABB->B.x, AABB->B.y, AABB->A.z),
			glm::vec3(AABB->B.x, AABB->B.y, AABB->B.z)
		};

		Mesh.Faces =
		{
			Mesh_Hitbox::Face( { 0, 2, 6 } ),
			Mesh_Hitbox::Face( { 0, 6, 4 } ),	// -z faces
			Mesh_Hitbox::Face( { 5, 7, 3 } ),
			Mesh_Hitbox::Face( { 5, 3, 1 } ),	// +z faces
			Mesh_Hitbox::Face( { 4, 6, 7 } ),
			Mesh_Hitbox::Face( { 4, 7, 5 } ),	// +x faces
			Mesh_Hitbox::Face( { 2, 1, 3 } ),
			Mesh_Hitbox::Face( { 2, 0, 1 } ),	// -x faces
			Mesh_Hitbox::Face( { 2, 3, 6 } ),
			Mesh_Hitbox::Face( { 3, 7, 6 } ),	// +y faces
			Mesh_Hitbox::Face( { 0, 4, 1 } ),
			Mesh_Hitbox::Face( { 4, 5, 1 } )	// -y faces
		};

		Mesh.Object = AABB->Object;

		Update_Mesh_Hitbox_Normals(&Mesh);

		return Mesh;
	}

	Collision_Info Clipping_Mesh_Hitbox(Mesh_Hitbox* Hitbox_A, Mesh_Hitbox* Hitbox_B)	// This handles accurate clipping to get the intersection between two objects
	{
		// This will clip Hitbox_A against the faces of Hitbox_B

		Collision_Info Info;

		Info.Delta = 999999.0f;	// some large number

		Mesh_Hitbox Clipped = *Hitbox_A;

		for (size_t Face = 0; Face < Hitbox_B->Faces.size(); Face++)
		{
			Collision_Info New_Info;
			New_Info.Delta = -999999.0f;
			New_Info.Normal = Hitbox_B->Faces[Face].Transformed_Normal;

			Mesh_Hitbox New_Clipped;

			std::vector<float> Overlaps(Clipped.Transformed_Points.size());

			for (size_t Index = 0; Index < Clipped.Transformed_Points.size(); Index++)
			{
				Overlaps[Index] =
					-glm::dot(
						-(Hitbox_A->Object->Position + Clipped.Transformed_Points[Index]) + (Hitbox_B->Transformed_Points[Hitbox_B->Faces[Face].Point_Index[0]] + Hitbox_B->Object->Position),
						Hitbox_B->Faces[Face].Transformed_Normal
					);
				New_Info.Delta = std::fmaxf(New_Info.Delta, Overlaps[Index]);	// Gets the one furthest inside the shape
			}

			if (New_Info.Delta < Info.Delta) // prioritise side with least overlap
				Info = New_Info;

			// gets all of the overlaps
			// positive overlap means inside shape
			// negative is outside

			for (size_t Index = 0; Index < Clipped.Faces.size(); Index++)
			{
				// This will clip the faces and add them to the 'new_clipped' shape accordingly

				// 4 cases:
					// no points are inside
						// produce 0 points/faces
					// 1 point is inside
						// produce 1 face
					// 2 points are inside
						// produce 2 faces
					// all points are inside
						// produce same face

				size_t Inside_Count = 0, Outside_Count = 0;
				size_t Inside_Indices[3];
				size_t Outside_Indices[3];

				for (size_t Point = 0; Point < 3; Point++)
					if (Overlaps[Clipped.Faces[Index].Point_Index[Point]] >= 0.0f)
						Inside_Indices[Inside_Count++] = Clipped.Faces[Index].Point_Index[Point];
					else
						Outside_Indices[Outside_Count++] = Clipped.Faces[Index].Point_Index[Point];

				switch (Inside_Count)
				{
				case 3:
					
					New_Clipped.Faces.push_back(
						Mesh_Hitbox::Face(
							{ 
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Clipped.Faces[Index].Point_Index[0]]),
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Clipped.Faces[Index].Point_Index[1]]),
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Clipped.Faces[Index].Point_Index[2]])
							},

							Clipped.Faces[Index].Transformed_Normal
						)
					);

					break;

				case 2:
				{
					// here, we need to get 2 interpolated points: (outside->inside1) and (outside->inside2)

					// the 'overlaps' array will be used to get the coefficients 

						// a * overlap1 + (1 - a) * overlap2 = 0.0f

						// a * (overlap1 - overlap2) + overlap2 = 0.0f

						// a = -overlap2 / (overlap1 - overlap2)

					// Then, we'll push 2 faces with the corresponding points and unchanged normals

					float A = -(Overlaps[Outside_Indices[0]]) / (Overlaps[Inside_Indices[0]] - Overlaps[Outside_Indices[0]]);
					float B = -(Overlaps[Outside_Indices[0]]) / (Overlaps[Inside_Indices[1]] - Overlaps[Outside_Indices[0]]);

					glm::vec3 Point_A = A * Clipped.Transformed_Points[Inside_Indices[0]] + (1 - A) * Clipped.Transformed_Points[Outside_Indices[0]];
					glm::vec3 Point_B = B * Clipped.Transformed_Points[Inside_Indices[1]] + (1 - B) * Clipped.Transformed_Points[Outside_Indices[0]];

					size_t Indices[4] =
					{
						Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Point_A),
						Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Point_B),
						Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Inside_Indices[0]]),
						Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Inside_Indices[1]])
					};

					New_Clipped.Faces.push_back(
						Mesh_Hitbox::Face(
							{
								Indices[0],
								Indices[2],
								Indices[3]
							},
							Clipped.Faces[Index].Transformed_Normal
						)
					);

					New_Clipped.Faces.push_back(
						Mesh_Hitbox::Face(
							{
								Indices[0],
								Indices[1],
								Indices[3]
							},
							Clipped.Faces[Index].Transformed_Normal
						)
					);

					break;

				}


				case 1:
				{
					// only 1 triangle is emitted

					float A = -(Overlaps[Inside_Indices[0]]) / (Overlaps[Outside_Indices[0]] - Overlaps[Inside_Indices[0]]);
					float B = -(Overlaps[Inside_Indices[0]]) / (Overlaps[Outside_Indices[1]] - Overlaps[Inside_Indices[0]]);

					glm::vec3 Point_A = A * Clipped.Transformed_Points[Outside_Indices[0]] + (1 - A) * Clipped.Transformed_Points[Inside_Indices[0]];
					glm::vec3 Point_B = B * Clipped.Transformed_Points[Outside_Indices[1]] + (1 - B) * Clipped.Transformed_Points[Inside_Indices[0]];

					New_Clipped.Faces.push_back(
						Mesh_Hitbox::Face(
							{
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Point_A),
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Point_B),
								Add_Vertex_To_Mesh(New_Clipped.Transformed_Points, Clipped.Transformed_Points[Inside_Indices[0]])
							},
							Clipped.Faces[Index].Transformed_Normal
						)
					);

					break;
				}

				default:			// none are inside
					break;
				}
			}

			Clipped = New_Clipped;	// update the 'clipped' object
		}

		Info.Points = Clipped.Transformed_Points;

		for (size_t Point = 0; Point < Info.Points.size(); Point++)
			Info.Points[Point] += Hitbox_A->Object->Position;

		Info.B = Hitbox_A;
		Info.A = Hitbox_B;

		return Info;	
	}

	Collision_Info AABB_Hitbox::Mesh_Collision(Mesh_Hitbox* Other)
	{
		Mesh_Hitbox AABB_Mesh = Mesh_Hitbox_From_AABB_Hitbox(this);

		Collision_Info Info = AABB_Mesh.Mesh_Collision(Other);//Clipping_Mesh_Hitbox(&AABB_Mesh, Other);

		Info.B = this;

		if (Info.Delta > 0.0f)
			return Info;

		return Collision_Info();
	}
	Collision_Info Mesh_Hitbox::AABB_Collision(AABB_Hitbox* Other)
	{
		return Flip_Collision(Other->Mesh_Collision(this));
	}

	Collision_Info Mesh_Hitbox::Mesh_Collision(Mesh_Hitbox* Other)
	{
		Collision_Info Info = Clipping_Mesh_Hitbox(this, Other);

		if (Info.Delta > 0.0) // if there's an overlap?
			return Info;

		return Collision_Info();	// Otherwise? No collision
	}

	Collision_Info Mesh_Hitbox::Test_Collision(Hitbox* Other)
	{
		return Other->Mesh_Collision(this);
	}
}