#ifndef SAMPLERS_HLSLI
#define SAMPLERS_HLSLI
//DIFFUSE = 0, SPECULAR, EMISSIVE, NORMAL, HEIGHT, SKYBOX, AO, AMBIENT, OPACITY,
Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D emissiveMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D heightMap : register(t4);
TextureCube skybox : register(t5);
Texture2D AOMap : register(t6);
Texture2D ambientMap : register(t7);
Texture2D opcityMap : register(t8);

// Particle
Texture2DArray gTexArray: register( t10 );
Texture1D gRandomTex: register( t11 );

//PointWrap = 0, PointClamp, LinearWrap, LinearClamp, AnisotropicWrap, AnisotropicClamp
SamplerState pointWrap : register(s0);
SamplerState pointClamp : register(s1);
SamplerState linearWrap : register(s2);
SamplerState linearClamp : register(s3);
SamplerState trilinearWrap : register(s4);
SamplerState trilinearClamp : register(s5);
SamplerState anisotropicWrap : register(s6);
SamplerState anisotropicClamp : register(s7);
#endif //SAMPLERS_HLSLI