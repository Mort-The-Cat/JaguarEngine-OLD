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

		Optional_Value<glm::vec2> Origin;

		Optional_Value<float> Width;
		Optional_Value<float> Height;

		Optional_Value<float> Margin_Top;	// Writing to any of these will overwrite Origin (and width/height if not already defined)
		Optional_Value<float> Margin_Left;
		Optional_Value<float> Margin_Right;
		Optional_Value<float> Margin_Bottom;

#define MARGIN_OUTSIDE_FLAG 1u
#define MARGIN_WITHIN_FLAG 2u

		Optional_Value<float> Angle;	// This is the angle (in radians) clockwise from the base direction

		std::vector<UI_Element> Child_Nodes;

#define Origin(Position) .Origin = { .Value = Position, .Flags = 1 }
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

	void Test(Jaguar_Engine* Engine)
	{
		// This is the syntax for creating a UI element with a number of base properties
		UI_Element Element =
			{
				Origin(glm::vec2( X_Aspect(0.45f), 0.45f )), // This prevents stretching along the X axis 
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