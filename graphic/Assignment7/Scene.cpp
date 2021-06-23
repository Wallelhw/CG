//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing

float P_RR = 1/6;

Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // // TO DO Implement Path Tracing Algorithm here
    // //std::cout<<ray<<"_________________"<<std::endl;
    // Intersection inter = intersect(ray);     //std::cout<<inter.happened<<"_______000__________"<<std::endl;
    // if(!inter.happened){
    //     return {};
    // }

    // if(inter.m->hasEmission())
    // {
    //     return inter.m->getEmission();
    // }


    // Vector3f L_dir,L_indir,L_i,f_r,wi,wo;
    // float costheta,costhetaprim,distance,pdf_dir,pdf_indir,p;

    // //Contribution from the light source.
    // Intersection random_light_inter = Intersection();//sample point at light areas
    // sampleLight(random_light_inter,pdf_dir);
            
    // L_i = random_light_inter.emit;
    // wo = -ray.direction;
    // wi = (random_light_inter.coords - inter.coords).normalized();
    // f_r = inter.m->eval(wo,wi,inter.normal);
    // costheta = dotProduct(wi,inter.normal);
    // costhetaprim = dotProduct(-wi,random_light_inter.normal);
    // distance =  (inter.coords - random_light_inter.coords).norm();

    // Ray o2l = Ray(inter.coords,(random_light_inter.coords - inter.coords).normalized());
    // Intersection o2l_inter = intersect(o2l);
    // if(distance < o2l_inter.distance + EPSILON){
    //     L_dir = L_i * f_r * costheta * costhetaprim / (distance * distance) / pdf_dir;
    // }

    // //std::cout<<L_dir<<"_________________"<<std::endl;
    // //Contribution from other reflectors.
    // p = get_random_float();
    // if(p > RussianRoulette){
    //     return L_dir;
    // }  
    // wo = -ray.direction;
    // wi = inter.m->sample(-ray.direction,inter.normal);
    // Ray r = Ray(inter.coords,wi.normalized());
    // L_i = castRay(r,depth + 1);

    // Intersection nextinter =intersect(r);
    // if(nextinter.happened && !nextinter.m->hasEmission()){
    //     f_r = inter.m->eval(wo,wi,inter.normal);
    //     pdf_indir = inter.m->pdf(wo,wi,inter.normal);
    //     costheta = dotProduct(wi,inter.normal);
    //     L_indir = L_i * f_r * costheta / pdf_indir / RussianRoulette;
    // }
        
    // return L_dir + L_indir;
    Vector3f ldir = {0, 0, 0};
    Vector3f lindir = {0, 0, 0};

    Intersection objectInter = intersect(ray);
    if(!objectInter.happened)
    {
        return {};
    }

    if(objectInter.m->hasEmission())
    {
        return objectInter.m->getEmission();
    }

    Intersection lightInter;
    float lightPdf = 0.0f;
    sampleLight(lightInter, lightPdf);

    Vector3f obj2light = lightInter.coords - objectInter.coords;
    Vector3f obj2lightdir = obj2light.normalized();
    float distancePow2 = obj2light.x * obj2light.x + obj2light.y * obj2light.y + obj2light.z * obj2light.z;

    Ray obj2lightray = {objectInter.coords, obj2lightdir};
    Intersection t = intersect(obj2lightray);
    if(t.distance - obj2light.norm() > -EPSILON)
    {
        ldir = lightInter.emit * objectInter.m->eval(ray.direction, obj2lightdir, objectInter.normal) * dotProduct(obj2lightdir, objectInter.normal) * dotProduct(-obj2lightdir, lightInter.normal) / distancePow2 / lightPdf;
    }

    if(get_random_float() > RussianRoulette)
    {
        return ldir;
    }

    Vector3f obj2nextobjdir = objectInter.m->sample(ray.direction, objectInter.normal).normalized();
    Ray obj2nextobjray = {objectInter.coords, obj2nextobjdir};
    Intersection nextObjInter = intersect(obj2nextobjray);
    if(nextObjInter.happened && !nextObjInter.m->hasEmission())
    {
        float pdf = objectInter.m->pdf(ray.direction, obj2nextobjdir, objectInter.normal);
        lindir = castRay(obj2nextobjray, depth + 1) * objectInter.m->eval(ray.direction, obj2nextobjdir, objectInter.normal) * dotProduct(obj2nextobjdir, objectInter.normal) / pdf / RussianRoulette;
    }

    return ldir + lindir;

}