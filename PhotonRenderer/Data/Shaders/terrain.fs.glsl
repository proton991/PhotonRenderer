#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 Tex;
in vec3 WorldPos;
in vec3 Normal;

layout (binding = 0) uniform sampler2D gTextureHeight0;
layout (binding = 1) uniform sampler2D gTextureHeight1;
layout (binding = 2) uniform sampler2D gTextureHeight2;
layout (binding = 3) uniform sampler2D gTextureHeight3;

uniform float gHeight0 = 80.0;
uniform float gHeight1 = 110.0;
uniform float gHeight2 = 150.0;
uniform float gHeight3 = 180.0;

uniform vec3 gReversedLightDir;

vec4 CalcTexColor()
{
    vec4 TexColor;

    float Height = WorldPos.y;

    if (Height < gHeight0) {
       TexColor = texture(gTextureHeight0, Tex);
    } else if (Height < gHeight1) {
       vec4 Color0 = texture(gTextureHeight0, Tex);
       vec4 Color1 = texture(gTextureHeight1, Tex);
       float Delta = gHeight1 - gHeight0;
       float Factor = (Height - gHeight0) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } else if (Height < gHeight2) {
       vec4 Color0 = texture(gTextureHeight1, Tex);
       vec4 Color1 = texture(gTextureHeight2, Tex);
       float Delta = gHeight2 - gHeight1;
       float Factor = (Height - gHeight1) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } else if (Height < gHeight3) {
       vec4 Color0 = texture(gTextureHeight2, Tex);
       vec4 Color1 = texture(gTextureHeight3, Tex);
       float Delta = gHeight3 - gHeight2;
       float Factor = (Height - gHeight2) / Delta;
       TexColor = mix(Color0, Color1, Factor);
    } else {
       TexColor = texture(gTextureHeight3, Tex);
    }

    return TexColor;
}


void main()
{
    vec4 TexColor = CalcTexColor();

    vec3 Normal_ = normalize(Normal);

    float Diffuse = dot(Normal_, gReversedLightDir);

    Diffuse = max(0.3f, Diffuse);

    FragColor = Color * TexColor * Diffuse;
}
