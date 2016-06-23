#ifndef TEXTURESANDSAMPLERS_HLSLI
#define TEXTURESANDSAMPLERS_HLSLI
//DIFFUSE = 0, SPECULAR, EMISSIVE, NORMAL, HEIGHT, SkyBox
Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D emissiveMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D heightMap : register(t4);
TextureCube skybox : register(t5);
Texture2D AOMap : register(t6);
//PointWrap = 0, PointClamp, LinearWrap, LinearClamp, AnisotropicWrap, AnisotropicClamp
SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState anisotropicWrap : register(s4);
SamplerState anisotropicClamp : register(s5);
#endif //TEXTURESANDSAMPLERS_HLSLI