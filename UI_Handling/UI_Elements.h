#ifndef JAGUAR_UI_ELEMENTS
#define JAGUAR_UI_ELEMENTS

#include "../OpenGL_Handling/OpenGL_Handling.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	template<typename Value_Type>
	struct Optional_Value
	{
		Value_Type Value;
		size_t Flags = 0;
	};

	union Image_Colour
	{
		const char* Texture_Directory;

		uint32_t RGBA;	// 8-bit RGBA
	};

	struct UI_Element
	{
		// This will contain border/background image/colour data (it can use either an image or colour)
		// It will also provide simple UV's for the image/colours

		Optional_Value<Image_Colour> Image;	// This defines the colour/image directory of the UI element (flag specifies if it's a texture or an RGBA term)

		Optional_Value<float> Margin_Top;	// Writing to any of these will overwrite Origin (and width/height if not already defined)
		Optional_Value<float> Margin_Left;
		Optional_Value<float> Margin_Right;
		Optional_Value<float> Margin_Bottom;

		Optional_Value<float> Width;
		Optional_Value<float> Height;

#define MARGIN_OUTSIDE_FLAG 1u
#define MARGIN_WITHIN_FLAG 2u

		float Angle = 0;
										// This is the angle (in radians) clockwise from the base direction 
										// (note, this is a rotation around the origin of the UI element) (applied AFTER fitting width/height etc)
										// (this isn't necessarily the centre of the element)

		std::vector<UI_Element> Child_Nodes;

#define Origin(Position)\
		.Margin_Top = { .Value = Position.y, .Flags = MARGIN_WITHIN_FLAG },\
		.Margin_Left = { .Value = Position.x, .Flags = MARGIN_WITHIN_FLAG }
#define Width(W) .Width = { .Value = W, .Flags = 1 }
#define Height(H) .Height = { .Value = H, .Flags = 1 }

#define X_Aspect(Value) ((Engine->Window_Height / Engine->Window_Width) * Value)
#define Y_Aspect(Value) ((Engine->Window_Width / Engine->Window_Height) * Value)

#define Margin_Out(Scalar)\
		.Margin_Top = { .Value = Scalar, .Flags = 1 },\
		.Margin_Left = { .Value = Scalar, .Flags = 1 },\
		.Margin_Right = { .Value = Scalar, .Flags = 1 },\
		.Margin_Bottom = { .Value = Scalar, .Flags = 1 }

#define Margin_In(Scalar)\
		.Margin_Top = { .Value = Scalar, .Flags = 2 },\
		.Margin_Left = { .Value = Scalar, .Flags = 2 },\
		.Margin_Right = { .Value = Scalar, .Flags = 2 },\
		.Margin_Bottom = { .Value = Scalar, .Flags = 2 }

#define Margin_Top_In(Scalar) .Margin_Top = { .Value = Scalar, .Flags = 2 }
#define Margin_Top_Out(Scalar) .Margin_Top = { .Value = Scalar, .Flags = 1 }

#define Margin_Left_In(Scalar) .Margin_Left = { .Value = Scalar, .Flags = 2 }
#define Margin_Left_Out(Scalar) .Margin_Left = { .Value = Scalar, .Flags = 1 }

#define Margin_Right_In(Scalar) .Margin_Right = { .Value = Scalar, .Flags = 2 }
#define Margin_Right_Out(Scalar) .Margin_Right = { .Value = Scalar, .Flags = 1 }

#define Margin_Bottom_In(Scalar) .Margin_Bottom = { .Value = Scalar, .Flags = 2 }
#define Margin_Bottom_Out(Scalar) .Margin_Bottom = { .Value = Scalar, .Flags = 1 }


#define Children(Elements) .Child_Nodes = Elements
	};

	struct UI_Dimensions
	{
		glm::vec2 Origin, Scale;

		glm::vec2 Down, Right;		// The 'down' and 'right' vectors...
	};

	/*
	
	3 possibilities (for each axis)

	0 - margin_left_in & margin_right_in
	1 - margin_left_in & scale
	2 - scale & margin_right_in

	3 - margin_left_out & scale
	4 - scale & margin_right_out

	
	*/

	void Evaluate_Axis(Optional_Value<float> Margin_Min, Optional_Value<float> Margin_Max, Optional_Value<float> Scale, float Parent_Origin, float Parent_Scale,
					float* Target_Origin, float* Target_Scale)
	{
		if (Scale.Flags)
		{
			// either 1, 2, 3, or 4

			*Target_Scale = Scale.Value;

			if (Margin_Min.Flags)
			{
				// either 1 or 3

				*Target_Origin = (Margin_Min.Flags == MARGIN_WITHIN_FLAG ? 
					Parent_Origin + Margin_Min.Value : 
					(Parent_Origin + Scale.Value + Margin_Min.Value));
			}
			else
			{
				// either 2 or 4

				*Target_Origin = 
					(Margin_Max.Flags == MARGIN_WITHIN_FLAG ?
						Parent_Origin + Parent_Scale - Margin_Max.Value :
						Parent_Origin - Margin_Max.Value
					) - Scale.Value;
			}
		}
		else
		{
			// definitely 0

			*Target_Origin = Parent_Origin + Margin_Min.Value;
			*Target_Scale = Parent_Scale - (Margin_Min.Value + Margin_Max.Value);
		}
	}

	void Render_UI_Element(UI_Element* Element, UI_Element* Parent_Node, UI_Dimensions Dimensions) // This is run recursively
	{
		UI_Dimensions Local_Dimensions;

		Evaluate_Axis(Element->Margin_Top, Element->Margin_Bottom, Element->Height, Dimensions.Origin.y, Dimensions.Scale.y,
			&Local_Dimensions.Origin.y, &Local_Dimensions.Scale.y);

		Evaluate_Axis(Element->Margin_Left, Element->Margin_Right, Element->Width, Dimensions.Origin.x, Dimensions.Scale.x,
			&Local_Dimensions.Origin.x, &Local_Dimensions.Scale.x);

		Local_Dimensions.Origin += Dimensions.Origin;

		//Local_Dimensions.Down = Dimensions.Down;
		//Local_Dimensions.Right = Dimensions.Right;

		float Sin = sinf(Element->Angle), Cos = cosf(Element->Angle);

		Local_Dimensions.Down = Dimensions.Down * Cos + Dimensions.Right * Sin;
		Local_Dimensions.Right = Dimensions.Right * Cos - Dimensions.Down * Sin;

		// rotates by angle

		glm::vec2 Scaled_Right = Local_Dimensions.Right * Local_Dimensions.Scale.x;
		glm::vec2 Scaled_Down = Local_Dimensions.Down * Local_Dimensions.Scale.y;

		//

		// applies rotation of down/right vectors

		// makes draw call with newly made matrix

		// draw object using origin and scaled_right / scaled_down



		//

		for (size_t Child = 0; Child < Element->Child_Nodes.size(); Child++)
			Render_UI_Element(&Element->Child_Nodes[Child], Element, Local_Dimensions);
	}

	void Test(Jaguar_Engine* Engine)
	{
		// This is the syntax for creating a UI element with a number of base properties
		UI_Element Element =
			{
				Origin(glm::vec2(X_Aspect(0.45f), 0.45f)), // This prevents stretching along the X axis 
				// (keeping the Y position the same and preserving the intended square shape of the object)

				Width( X_Aspect(.10) ),	// Preserves square shape
				Height(.10),

				Children(
					std::vector<UI_Element>{
						{
							Margin_In(0.025f)			// This will create a child node inside of the ui element that is slightly within the other element
						}
					}
				)
			};

	}
}

#endif