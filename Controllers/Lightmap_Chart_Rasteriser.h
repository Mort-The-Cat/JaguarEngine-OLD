#ifndef JAGUAR_LIGHTMAP_CHART_RASTERISER
#define JAGUAR_LIGHTMAP_CHART_RASTERISER

#include "../OpenGL_Handling/OpenGL_Handling.h"

namespace Jaguar
{
	// We want a rasterising function that applies optional interpolation of values

	// Should be good!!

	// The triangle we're given is guaranteed to have a flat top such that

	/*

		A----------B
		|        /
		|      /
		|    /
		|  /
		|/
		C

	*/

#define LIGHTMAP_CHART_PADDING 8.0f
#define LIGHTMAP_CHART_INTERPOLATION_CLAMP 0.0f


	// perhaps we can delegate this task using the job system? That'd be awesome!!

	template<bool Interpolate_Values, typename Point, bool(*Perpixel_Function)(size_t, size_t, Point, void*)>
	bool Lightmap_Chart_Rasterise_Function(glm::vec2 A, glm::vec2 B, glm::vec2 C, Point A_Value, Point B_Value, Point C_Value, int Canvas_Size, void* Data)
	{
		if (std::max(B.x, C.x) + LIGHTMAP_CHART_PADDING >= Canvas_Size ||
			C.y + LIGHTMAP_CHART_PADDING >= Canvas_Size ||
			C.x < LIGHTMAP_CHART_PADDING)
		{
			return false;				// (Function failed)
		}

		glm::vec2 A_C = C - A;

		float Inverse_A_C = 1.0f / A_C.y;

		glm::vec2 B_C = C - B;

		float A_B = B.x - A.x;

		float Inverse_A_B_X = 1.0f / A_B;

		assert(A.y == B.y);

		float D_Scanline = -LIGHTMAP_CHART_PADDING;
		for (int Scanline = A.y - LIGHTMAP_CHART_PADDING; Scanline < C.y + LIGHTMAP_CHART_PADDING; Scanline++, D_Scanline++)
		{
			float Clamped_Scanline = 
				std::max(
					std::min(A_C.y - 1 + LIGHTMAP_CHART_INTERPOLATION_CLAMP, D_Scanline),
				-LIGHTMAP_CHART_INTERPOLATION_CLAMP);

			float Scanline_Factor = Inverse_A_C * (float)Clamped_Scanline;

			float Left = A.x + A_C.x * Scanline_Factor;

			float Right = B.x + B_C.x * Scanline_Factor;

			float Inverse_Scanline;
			
			if constexpr (Interpolate_Values)
			{
				Inverse_Scanline = Right - Left;
				Inverse_Scanline = 1.0f / Inverse_Scanline;
			}

			float D_Pixel = -LIGHTMAP_CHART_PADDING;
			for (int Pixel = Left - LIGHTMAP_CHART_PADDING; Pixel < Right + LIGHTMAP_CHART_PADDING; Pixel++, D_Pixel++)
			{
				float Clamped_Pixel;
				float Factor;

				if constexpr (Interpolate_Values)
				{
					//Clamped_Pixel =
					//	std::max(
					//		std::min(Right - Left - 1 - LIGHTMAP_CHART_INTERPOLATION_CLAMP, D_Pixel),
					//		LIGHTMAP_CHART_INTERPOLATION_CLAMP
					//	);

					Clamped_Pixel =
						std::min(
							std::max(-LIGHTMAP_CHART_INTERPOLATION_CLAMP, D_Pixel), (Right - Left) + LIGHTMAP_CHART_INTERPOLATION_CLAMP - 1.2f
						);
					Factor = Inverse_Scanline * Clamped_Pixel;
				}

				// C influence = Scanline_Factor

				// B influence = Factor * (1 - C influence)

				// A influence = 1 - B influence

				float A_Influence, B_Influence, C_Influence;

				Point Interpolated_Value = Point(0);

				if constexpr (Interpolate_Values)
				{
					C_Influence = Scanline_Factor;
					B_Influence = Factor * (1 - C_Influence);
					A_Influence = 1 - B_Influence - C_Influence;

					Interpolated_Value =
						A_Value * A_Influence +
						B_Value * B_Influence +
						C_Value * C_Influence;

				}

				if (Perpixel_Function(Pixel, Scanline, Interpolated_Value, Data))	// A HIT will make the function return early
				{
					return false;
				}
			}
		}

		return true;
	}
}

#endif