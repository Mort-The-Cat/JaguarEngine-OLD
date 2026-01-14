#ifndef JAGUAR_HITBOXES
#define JAGUAR_HITBOXES

#include "../OpenGL_Handling/OpenGL_Handling.h"

#include "../Collada_Loader/Collada_Loader.h"

namespace Jaguar
{
	class World_Object;

	class Hitbox;

	class AABB_Hitbox;

	class Sphere_Hitbox;

	class Mesh_Hitbox;

	struct Collision_Info
	{
		glm::vec3 Normal;		// The normal from B to A
		float Delta = 0;		// 0 if no collision / positive if collision
		Hitbox* A = nullptr;
		Hitbox* B = nullptr;

		std::vector<glm::vec3> Points;

		Collision_Info() { Delta = 0; }

		// I might add multiple collision points to simulate a kind of rigid body

		// I can add some additional info here later
	};

#define HF_TO_BE_DELETED 0u

	class Hitbox			// Some kind of hitbox?
	{
	public:
		bool Flags[1] = { false };

		World_Object* Object = nullptr;

		virtual void Update_Hitbox()
		{
			return;	// This won't do anything for the default hitbox
		}

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox)
		{ return Collision_Info(); }

		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox)
		{ return Collision_Info(); }

		virtual Collision_Info Sphere_Collision(Sphere_Hitbox* Other_Hitbox)
		{ return Collision_Info(); }

		virtual Collision_Info Mesh_Collision(Mesh_Hitbox* Other_Hitbox)
		{ return Collision_Info(); }
	};

	//

	Collision_Info AABB_Sphere_Collision(AABB_Hitbox* This, Sphere_Hitbox* Other_Hitbox);

	class Mesh_Hitbox : public Hitbox
	{
	public:
		struct SAT_Face
		{
			glm::vec3 Transformed_Normal;
			glm::vec3 Normal;
			uint16_t Point_Index;

			SAT_Face() {}

			SAT_Face(uint16_t Point_Indexp, glm::vec3 Transformed_Normalp)
			{
				Point_Index = Point_Indexp;
				Transformed_Normal = Transformed_Normalp;
			}
		};

		std::vector<glm::vec3> Transformed_Points;	// These are the points that have undergone a transformation (rotation/displacement)

		std::vector<glm::vec3> Points;

		std::vector<SAT_Face> Faces;

		struct SAT_Edge	// Oh good heavens
		{
			glm::vec3 Transformed_Edge;
			glm::vec3 Edge;
			uint16_t Point_Index;
		};

		std::vector<SAT_Edge> Edges;

		// triangles with the same normal are merged

		virtual void Update_Hitbox() override;

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox) override;

		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox) override;

		virtual Collision_Info Mesh_Collision(Mesh_Hitbox* Other_Hitbox) override;
	};

	void Create_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh, Mesh_Hitbox* Target_Hitbox);

	class Sphere_Hitbox : public Hitbox
	{
	public:
		float Radius;

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox) override;
		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox) override;
		virtual Collision_Info Sphere_Collision(Sphere_Hitbox* Other_Hitbox) override;
	};

	class AABB_Hitbox : public Hitbox	// The simplest hitbox- very easy
	{
	public:
		glm::vec3 A, B;					// Must remember to offset this by the position of the world-object

		virtual Collision_Info Test_Collision(Hitbox* Other_Hitbox) override;
		virtual Collision_Info AABB_Collision(AABB_Hitbox* Other_Hitbox) override;
		virtual Collision_Info Sphere_Collision(Sphere_Hitbox* Other_Hitbox) override;

		virtual Collision_Info Mesh_Collision(Mesh_Hitbox* Other_Hitbox) override;
	};

	Collision_Info Flip_Collision(Collision_Info Info);

	std::vector<Hitbox*> Wrap_Sphere_Hitbox(const Collada::Collada_Mesh* Model_Mesh, float Thickness);
	std::vector<Hitbox*> Wrap_AABB_Hitboxes(const Collada::Collada_Mesh* Model_Mesh, float Thickness = 0.02f);
	std::vector<Hitbox*> Wrap_AABB_Hitbox(const Collada::Collada_Mesh* Model_Mesh, float Thickness = 0.02f);

	std::vector<Hitbox*> Wrap_Mesh_Hitbox(const Collada::Collada_Mesh* Model_Mesh);
}


#endif