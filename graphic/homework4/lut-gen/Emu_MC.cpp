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
const float SchlicksRo = pow(0.3 / 2.3, 2);

typedef struct samplePoints {
    std::vector<Vec3f> directions;
	std::vector<float> PDFs;
}samplePoints;

samplePoints squareToCosineHemisphere(int sample_count){
    samplePoints samlpeList;
    const int sample_side = static_cast<int>(floor(sqrt(sample_count)));

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> rng(0.0, 1.0);
    for (int t = 0; t < sample_side; t++) {
        for (int p = 0; p < sample_side; p++) {
            double samplex = (t + rng(gen)) / sample_side;
            double sampley = (p + rng(gen)) / sample_side;
            
            double theta = 0.5f * acos(1 - 2*samplex);
            double phi =  2 * M_PI * sampley;
            Vec3f wi = Vec3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float pdf = wi.z / PI;
            
            samlpeList.directions.push_back(wi);
            samlpeList.PDFs.push_back(pdf);
        }
    }
    return samlpeList;
}

float DistributionGGX(Vec3f N, Vec3f H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = std::max(dot(N, H), 0.0f);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / std::max(denom, 0.0001f);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float a = roughness;
    float k = (a+1) * (a+1) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float roughness, float NoV, float NoL) {
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

Vec3f IntegrateBRDF(Vec3f V, float roughness, float sintheta) {
    float A = 0.0;
    float B = 0.0;
    float C = 0.0;
    const int sample_count = 1024;
    Vec3f N = Vec3f(0.0, 0.0, 1.0);
    
    samplePoints sampleList = squareToCosineHemisphere(sample_count);                           //Cos加权采样的入射向量
    for (int i = 0; i < sample_count; i++) {                                                    //蒙特卡洛解双重积分把两个自变量的(定义域乘起来就行 均匀采样；特殊采样要除pdf）
      // TODO: To calculate (fr * ni) / p_o here                                                // ni = sin(theta) 此处ni就是mu  ,/p-o 是除概率密度（蒙特卡洛解积分）   
        Vec3f vi = sampleList.directions[i];                                                    //入射向量
        Vec3f vo = V;                                                                           //出射向量
        Vec3f h = normalize(vi + vo);                                                           //半程向量
        float pdf = sampleList.PDFs[i];                                                         //当前采样概率密度
        float NoV = std::max(dot(N, vo), 0.f);
        float NoL = std::max(dot(N, vi), 0.f);
        float HoV = std::max(dot(h, vo), 0.f);
        float HoL = std::max(dot(h, vi), 0.f);
        float F = 1.;                                                                            //Fresnel项使用Schlicks近似处理(白炉中不考虑F项)
        float D = DistributionGGX(N, h, roughness);                                              //Distribution GGX近似处理
        float G = GeometrySmith(roughness, HoL, HoV);
        float fr = F * G * D / (4. * NoV * NoL);
        A +=  fr * sintheta * (1./ pdf);
    }
    B = A;//RGB3通道没有做特殊处理
    C = A;
    return {A / float(sample_count), B / float(sample_count), C / float(sample_count)};
}

int main() {
    uint8_t data[resolution * resolution * 3];
    float step = 1.0 / resolution;
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            float roughness = step * (static_cast<float>(i) + 0.5f);// rough 0-1
            float sintheta = step * (static_cast<float>(j) + 0.5f);    // sin   0-1                //NoV应该是cos啊大哥，你这命名，我tm直接血压拉满
            Vec3f V = Vec3f(std::sqrt(1.f - sintheta * sintheta), 0.f, sintheta);                        //(cos,o,sin) 出射向量
            Vec3f irr = IntegrateBRDF(V, roughness, sintheta);

            data[(i * resolution + j) * 3 + 0] = uint8_t(irr.x * 255.0);
            data[(i * resolution + j) * 3 + 1] = uint8_t(irr.y * 255.0);
            data[(i * resolution + j) * 3 + 2] = uint8_t(irr.z * 255.0);
        }
    }
    stbi_flip_vertically_on_write(true);
    stbi_write_png("GGX_E_MC_LUT.png", resolution, resolution, 3, data, resolution * 3);
    std::cout << "Finished precomputed!" << std::endl;
    return 0;
}