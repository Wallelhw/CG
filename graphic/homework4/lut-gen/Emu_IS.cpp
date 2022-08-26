#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#include <random>
#include "vec.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

const int resolution = 128;

Vec2f Hammersley(uint32_t i, uint32_t N) { // 0-1   在0-1的范围内采样N次，结果均匀，返回采样进度和当前一次的采样结果
    uint32_t bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float rdi = float(bits) * 2.3283064365386963e-10;
    return {float(i) / float(N), rdi};
}

Vec3f ImportanceSampleGGX(Vec2f Xi, Vec3f N, float roughness) { // Xi Vec2f(采样进度，当前0-1均匀采样的结果)；N 宏观法线；roughness 粗糙度
    //TODO: in spherical space - Bonus 1
    float a_2 = roughness * roughness;
    float phi = Xi.x*2*PI;
    float cos_theta = sqrtf((1 - Xi.y) / (1 + Xi.y * (a_2 - 1)));       //D项计算出pdf_m,再计算出theta边缘概率密度，反推theta值，重要性采样体现在这里
    float sin_theta = sqrtf(1 - cos_theta * cos_theta);


    Vec3f H = Vec3f(.0f);
    H.x = sinf(phi) * sin_theta;
    H.y = cosf(phi) * sin_theta;
    H.z = cos_theta;

    return H;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    // TODO: To calculate Schlick G1 here - Bonus 1
    float a = roughness;
    float k = (a + 1) * (a + 1) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
    return 1.0f;
}

float GeometrySmith(float roughness, float NoV, float NoL) {
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

Vec3f IntegrateBRDF(Vec3f V, float roughness) {

    const float sample_count = 1024.0;
    Vec3f N = Vec3f(0.0, 0.0, 1.0);
    float split_sum = .0f;
    for (int i = 0; i < sample_count; i++) {
        Vec2f Xi = Hammersley(i, sample_count);                             //Xi为（0-1，0-1）空间中，正则采样的点
        Vec3f H = ImportanceSampleGGX(Xi, N, roughness);                    //仅把（0-1）^2的两个值转化为一个向量，重要性计算体现在pdf计算与化简的过程中
        Vec3f L = normalize(H * 2.0f * dot(V, H) - V);
        float NoL = std::max(L.z, 0.0f);
        float NoH = std::max(H.z, 0.0f);
        float VoH = std::max(dot(V, H), 0.0f);
        float NoV = std::max(dot(N, V), 0.0f);
        
        // TODO: To calculate (fr * ni) / p_o here - Bonus 1
        float G = GeometrySmith(roughness,NoV,NoL);
        float weight_L = (G * VoH) / (NoV * NoH);                           //此处为化简公式，已经做了通用蒙特卡洛乘1/pdf的过程，pdf（h）转pdf（i）的过程中出现了D，D项被销，F=1故fr中只剩下了G项
        // Split Sum - Bonus 2
        split_sum += weight_L;
    } 
    float integral_res = split_sum / sample_count;                           //蒙特卡洛解积分通用方法
    return Vec3f(integral_res, integral_res, integral_res);
}

int main() {
    uint8_t data[resolution * resolution * 3];
    float step = 1.0 / resolution;
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float roughness = step * (static_cast<float>(i) + 0.5f);
            float NdotV = step * (static_cast<float>(j) + 0.5f);
            Vec3f V = Vec3f(std::sqrt(1.f - NdotV * NdotV), 0.f, NdotV);

            Vec3f irr = IntegrateBRDF(V, roughness);

            data[(i * resolution + j) * 3 + 0] = uint8_t(irr.x * 255.0);
            data[(i * resolution + j) * 3 + 1] = uint8_t(irr.y * 255.0);
            data[(i * resolution + j) * 3 + 2] = uint8_t(irr.z * 255.0);
        }
    }
    stbi_flip_vertically_on_write(true);
    stbi_write_png("GGX_E_LUT.png", resolution, resolution, 3, data, resolution * 3);
    
    std::cout << "Finished precomputed!" << std::endl;
    return 0;
}