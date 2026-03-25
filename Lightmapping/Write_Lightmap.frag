#version 440

uniform samplerCubeShadow Shadow_Map;

out vec3 Out_Colour;

in vec3 Position;
in vec3 Normal;

uniform vec3 Light_Position;

uniform vec3 Colour;

float Vector_To_Depth(vec3 Vec, float Length)
{
    vec3 Abs_Vec = abs(Vec);
    float Local_Z = Length * max(Abs_Vec.x, max(Abs_Vec.y, Abs_Vec.z));

    const float Far = 100.0f;
    const float Near = 0.01f;

    float Norm_Z = (Far + Near) / (Far - Near) - (2 * Far * Near)/(Far - Near)/Local_Z;
    return (Norm_Z + 1.0) * 0.5;
}

void main()
{
    vec3 Light_To_Pixel = Position - Light_Position;

    float Length = length(Light_To_Pixel);

    Light_To_Pixel /= Length;

	float Exposed = texture(Shadow_Map, vec4(Light_To_Pixel, Vector_To_Depth(Light_To_Pixel, Length - 0.00f) ));

    Exposed = Exposed * max(0, dot(Light_To_Pixel, -Normal)) / (Length);       // This will account for normals

    Out_Colour = vec3(Exposed) * Colour;
}