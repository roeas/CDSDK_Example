#version 330 core

#define PI 3.1415926536
#define PI2 9.8696044011
#define INV_PI 0.3183098862
#define INV_PI2 0.1013211836

#define D_LIGHT_COUNT 4
#define T_POINT_LIGHT 0
#define T_DIRECTIONAL_LIGHT 2

out vec4 fragColor;

in vec3 v_worldPos;
in vec3 v_normal;
in vec2 v_texcoord0;
in vec3 v_tangent;
in vec3 v_bitangent;

uniform sampler2D s_texBaseColor;
uniform sampler2D s_texNormal;
uniform sampler2D s_texORM;
uniform sampler2D s_texLUT;
uniform samplerCube s_texCube;
uniform samplerCube s_texCubeIrr;
uniform vec3 u_cameraPos;

// -------------------- PBR -------------------- //

vec3 toLinear(vec3 _rgb) {
	return pow(abs(_rgb), vec3(2.2) );
}

vec4 toLinear(vec4 _rgba) {
	return vec4(toLinear(_rgba.xyz), _rgba.w);
}

// Fresnel
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Distribution
float DistributionGGX(float NdotH, float rough) {
	float a  = rough * rough;
	float a2 = a * a;
	float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return a2 / denom;
}

// Geometry
float Visibility(float NdotV, float NdotL, float rough) {
	// Specular BRDF = (F * D * G) / (4 * NdotV * NdotL)
	// = (F * D * (NdotV / (NdotV * (1 - K) + K)) * (NdotL / (NdotL * (1 - K) + K))) / (4 * NdotV * NdotL)
	// = (F * D * (1 / (NdotV * (1 - K) + K)) * (1 / (NdotL * (1 - K) + K))) / 4
	// = F * D * Vis
	// Vis = 1 / (NdotV * (1 - K) + K) / (NdotL * (1 - K) + K) / 4
	
	float f = rough + 1.0;
	float k = f * f * 0.125;
	float ggxV  = 1.0 / (NdotV * (1.0 - k) + k);
	float ggxL  = 1.0 / (NdotL * (1.0 - k) + k);
	return ggxV * ggxL * 0.25;
}

// -------------------- Material -------------------- //

struct Material {
	vec3 albedo;
	vec3 normal;
	float occlusion;
	float roughness;
	float metallic;
	vec3 F0;
	float opacity;
	vec3 emissive;
};

Material CreateMaterial() {
	Material material;
	material.albedo = vec3(0.99, 0.98, 0.95);
	material.normal = vec3(0.0, 1.0, 0.0);
	material.occlusion = 1.0;
	material.roughness = 0.9;
	material.metallic = 0.1;
	material.F0 = vec3(0.04, 0.04, 0.04);
	material.opacity = 1.0;
	material.emissive = vec3(0.0);
	return material;
}

vec3 SampleAlbedoTexture(vec2 uv) {
	return texture(s_texBaseColor, uv).xyz;
}

vec3 SampleNormalTexture(vec2 uv, vec3 tangent, vec3 bitangent, vec3 normal) {
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 normalTexture = normalize(texture(s_texNormal, uv).xyz * 2.0 - 1.0);
	return normalize(TBN * normalTexture);
}

vec3 SampleORMTexture(vec2 uv) {
	vec3 orm = texture(s_texORM, uv).xyz;
	orm.y = clamp(orm.y, 0.04, 1.0); // roughness
	return orm;
}

vec3 CalcuateF0(Material material) {
	return mix(vec3(0.04), material.albedo, material.metallic);
}

// -------------------- Light -------------------- //

struct Light {
	int type;
	vec3 position;
	float intensity;
	vec3 color;
	float range;
	vec3 direction;
};

uniform Light u_lights[D_LIGHT_COUNT];

float SmoothDistanceAtt(float squaredDistance, float invSqrAttRadius) {
	float factor = squaredDistance * invSqrAttRadius;
	float smoothFactor = clamp(1.0 - factor * factor, 0.0, 1.0);
	return smoothFactor * smoothFactor;
}

float GetDistanceAtt(float sqrDist, float invSqrAttRadius) {
	float attenuation = 1.0 / (max(sqrDist , 0.0001));
	attenuation *= SmoothDistanceAtt(sqrDist, invSqrAttRadius);
	return attenuation;
}

vec3 CalculatePointLight(Light light, Material material, vec3 worldPos, vec3 viewDir, vec3 diffuseBRDF) {
	vec3 lightDir = normalize(light.position - worldPos);
	vec3 harfDir  = normalize(lightDir + viewDir);
	
	float NdotV = max(dot(material.normal, viewDir), 0.0);
	float NdotL = max(dot(material.normal, lightDir), 0.0);
	float NdotH = max(dot(material.normal, harfDir), 0.0);
	float HdotV = max(dot(harfDir, viewDir), 0.0);
	
	float distance = length(light.position - worldPos);
	float attenuation = GetDistanceAtt(distance * distance, 1.0 / (light.range * light.range));
	vec3 radiance = light.color * light.intensity * 0.25 * INV_PI * attenuation;
	
	vec3  Fre = FresnelSchlick(HdotV, material.F0);
	float NDF = DistributionGGX(NdotH, material.roughness);
	float Vis = Visibility(NdotV, NdotL, material.roughness);
	vec3 specularBRDF = Fre * NDF * Vis;
	
	vec3 KD = mix(1.0 - Fre, vec3(0.0), material.metallic);
	return (KD * diffuseBRDF + specularBRDF) * radiance * NdotL;
}

vec3 CalculateDirectionalLight(Light light, Material material, vec3 worldPos, vec3 viewDir, vec3 diffuseBRDF) {
	vec3 lightDir = -light.direction;
	vec3 harfDir  = normalize(lightDir + viewDir);
	
	float NdotV = max(dot(material.normal, viewDir), 0.0);
	float NdotL = max(dot(material.normal, lightDir), 0.0);
	float NdotH = max(dot(material.normal, harfDir), 0.0);
	float HdotV = max(dot(harfDir, viewDir), 0.0);
	
	vec3  Fre = FresnelSchlick(HdotV, material.F0);
	float NDF = DistributionGGX(NdotH, material.roughness);
	float Vis = Visibility(NdotV, NdotL, material.roughness);
	vec3 specularBRDF = Fre * NDF * Vis;
	
	vec3 KD = mix(1.0 - Fre, vec3(0.0), material.metallic);
	vec3 irradiance = light.color * light.intensity;
	return (KD * diffuseBRDF + specularBRDF) * irradiance * NdotL;
}

vec3 CalculateLights(Material material, vec3 worldPos, vec3 viewDir, vec3 diffuseBRDF) {
	vec3 color = vec3(0.0);
	for(int lightIndex = 0; lightIndex < D_LIGHT_COUNT; ++lightIndex) {
		Light light = u_lights[lightIndex];
		switch(light.type) {
			case T_POINT_LIGHT:
				color += CalculatePointLight(light, material, worldPos, viewDir, diffuseBRDF); break;
			case T_DIRECTIONAL_LIGHT:
				color += CalculateDirectionalLight(light, material, worldPos, viewDir, diffuseBRDF); break;
			default:
				color += vec3(0.0); break;
		}
	}
	return color;
}

void main()
{
	Material material = CreateMaterial();
	material.albedo = SampleAlbedoTexture(v_texcoord0);
	material.normal = SampleNormalTexture(v_texcoord0, v_tangent, v_bitangent, v_normal);
	vec3 orm = SampleORMTexture(v_texcoord0);
	material.occlusion = orm.x;
	material.roughness = orm.y;
	material.metallic = orm.z;
	material.F0 = CalcuateF0(material);
	
	vec3 viewDir  = normalize(u_cameraPos - v_worldPos);
	vec3 diffuseBRDF = material.albedo * INV_PI;
	float NdotV = max(dot(material.normal, viewDir), 0.0);
	
	// ------------------------------------ Directional Light ----------------------------------------
	
	vec3 dirColor = CalculateLights(material, v_worldPos, viewDir, diffuseBRDF);
	
	// ----------------------------------- Environment Light ----------------------------------------
	
	// Environment Prefiltered Irradiance
	vec3 envIrradiance = vec3(0.1);
	
	// Environment Specular BRDF
	vec2 lut = texture(s_texLUT, vec2(NdotV, 1.0 - material.roughness)).xy;
	vec3 envSpecularBRDF = (material.F0 * lut.x + lut.y);
	
	// Environment Specular Radiance
	vec3 reflectDir = normalize(reflect(-viewDir, material.normal));
	float mip = clamp(6.0 * material.roughness, 0.1, 6.0);
	vec3 envRadiance = vec3(0.1);
	
	// Occlusion
	float specularOcclusion = mix(pow(material.occlusion, 4.0), 1.0, clamp(-0.3 + NdotV * NdotV, 0.0, 1.0));
	float horizonOcclusion = clamp(1.0 + 1.2 * dot(reflectDir, v_normal), 0.0, 1.0);
	horizonOcclusion *= horizonOcclusion;
	
	vec3 Fre = FresnelSchlick(NdotV, material.F0);
	vec3 KD = mix(1.0 - Fre, vec3(0.0), material.metallic);
	
	vec3 envColor = KD * material.albedo * envIrradiance * material.occlusion +
		envSpecularBRDF * envRadiance * min(specularOcclusion, horizonOcclusion);
	
	// ------------------------------------ Fragment Color -----------------------------------------
	
	fragColor = vec4(dirColor + envColor, 1.0);
}
