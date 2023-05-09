#version 460
// The fragment shader renders the 3D scene with PBR shading and fog effects.
// This shader is also responsible for applying multi-texturing to the barrels.

const float PI=3.14159265358979323846;

// Declare input variables.
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

// Define 3 uniform structs.

// Information for the light source.
uniform struct lightInfo {
    vec4 Position;
    vec3 L;
} Light;

// Information for the material of the object being rendered.
uniform struct MaterialInfo {
    float Rough;    // Roughness.
    bool Metal;    // Metal or dielectric.
    vec3 Colour;    // Diffuse colour for dielectrics, f0 for metallic.
    //float Shininess;    // Shininess factor.
} Material;

// Information for the fog effect.
uniform struct FogInfo {
    float MinDist;  // Minimum fog distance.
    float MaxDist;  // Maximum fog distance.
    vec3 Colour;    // Fog colour.
} Fog;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec3 PositionData;
layout (location = 2) out vec3 NormalData;
layout (location = 3) out vec3 ColourData;

layout(binding=0) uniform sampler2D PositionTex;
layout(binding=1) uniform sampler2D NormalTex;
layout(binding=2) uniform sampler2D ColourTex;

// Texture binding for 2 textures in the shader at once.
layout (binding = 3) uniform sampler2D Tex1;
layout (binding = 4) uniform sampler2D Tex2;

uniform int Pass; // Pass number.

// PBR implementation.

float ggxDistribution ( float nDotH) {
    float alpha2 = Material.Rough * Material.Rough * Material.Rough * Material.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith ( float dotProd ) {
    float k = (Material.Rough + 1.0 ) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnesl (float lDotH ) {
    vec3 f0 = vec3(0.04);
    if(Material.Metal) {
        f0 = Material.Colour;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

vec3 microfacetModel(vec3 position, vec3 n, vec3 diffColour) {
    vec3 diffuseBrdf = diffColour;
    vec3 l = vec3(0.0),
    lightI = Light.L;
    if (Light.Position.w == 0.0) {
        l = normalize(Light.Position.xyz);
    }   else {
        l = Light.Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + 1);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * schlickFresnesl(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    vec3 finalColour = (diffuseBrdf + PI * specBrdf) * lightI * nDotL;

    // Apply texture mapping.
    vec4 texColour = texture(Tex1, TexCoord);
    vec4 tex2Colour = texture(Tex2, TexCoord);
    vec3 texFinalColour = mix(texColour.rgb, tex2Colour.rgb, tex2Colour.a);
    
    finalColour *= texFinalColour;
    return finalColour;
}

void pass1()
{
    // Store position, normal, and diffuse color in textures
    PositionData = Position;
    NormalData = normalize(Normal);
    ColourData = Material.Colour;
}

void pass2()
{
    // Retrieve position and normal information from textures
    vec3 pos = vec3( texture( PositionTex, TexCoord ) );
    vec3 norm = vec3( texture( NormalTex, TexCoord ) );
    vec3 diffColour = vec3( texture(ColourTex, TexCoord) );
    //FragColour = vec4( diffuseModel(pos,norm,diffColor), 1.0 );
    
    // For 1 light.
    vec3 sum = vec3(0);
    sum += microfacetModel(pos, norm, diffColour);

    // Calculate gamma.
    sum = pow( sum, vec3(1.0/2.2) );
    float dist = abs( Position.z ); // Distance calculations.

    // FogFactor calculation.
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );   // Clamp values.

    // Assign a colour based on the fogFactor using mix.
    vec3 colour = mix( Fog.Colour, sum, fogFactor );
    FragColour = vec4(colour, 1.0);   // Final colour.
}

void main()
{
    if( Pass == 1)
        pass1();
    else if(Pass==2)
        pass2();
}