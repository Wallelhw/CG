#include <algorithm>
#include <cassert>
#include <chrono>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    auto start = std::chrono::system_clock::now();
    if (primitives.empty())
        return;

    switch (splitMethod)
    {
    case SplitMethod::NAIVE:
        root = recursiveBuild_EqualCount(primitives);
        break;
    
    case SplitMethod::SAH:
        root = recursiveBuild_SAH(primitives);
        break;
    }
    

    
    auto stop = std::chrono::system_clock::now();
    int hrs = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();
    int mins = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count();
    int secs = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\nobj_num: %i \n",
        hrs, mins, secs,this->obj_num);
}

BVHBuildNode* BVHAccel::recursiveBuild_EqualCount(std::vector<Object*> objects)
{
                std::cout<<"recursiveBuild_EqualCount"<<std::endl;
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());

    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        this->obj_num++;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild_EqualCount(std::vector{objects[0]});
        node->right = recursiveBuild_EqualCount(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        //split equal count
        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));
        node->left = recursiveBuild_EqualCount(leftshapes);
        node->right = recursiveBuild_EqualCount(rightshapes);
        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

BVHBuildNode* BVHAccel::recursiveBuild_SAH(std::vector<Object*> objects)
{
            std::cout<<"recursiveBuild_SAH"<<std::endl;
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());

    if (objects.size() == 1) {
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        this->obj_num++;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild_SAH(std::vector{objects[0]});
        node->right = recursiveBuild_SAH(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        //split sah
        float S_finv,S_l,S_r;
        float rate = kInfinity;
        S_finv = 1.0 / bounds.SurfaceArea();
        Bounds3 leftbounds;
        auto middling = objects.begin();
        auto beginning = objects.begin();
        auto ending = objects.end();
        for(int i=0;i<objects.size();i++)
        {
            leftbounds = Union(leftbounds,objects[i]->getBounds());
            S_l = leftbounds.SurfaceArea();
            Bounds3 rightbounds;
            for(int j=i+1;j<objects.size();j++){
                rightbounds= Union(rightbounds,objects[j]->getBounds());
            }
            S_r = rightbounds.SurfaceArea();
            float temp = S_l * S_finv + S_r *S_finv;
            if(temp < rate) { 
                rate = temp;
                middling = objects.begin() + i;
            }
        }

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));
        node->left = recursiveBuild_SAH(leftshapes);
        node->right = recursiveBuild_SAH(rightshapes);
        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    Intersection ins;
    if(!node->bounds.IntersectP(ray,ray.direction_inv,ray.DirIsNeg)) return ins;
    if(node->object)
    {
        ins = node->object->getIntersection(ray);
        return ins;
    }
    auto hit1 = BVHAccel::getIntersection(node->left,ray);
    auto hit2 = BVHAccel::getIntersection(node->right,ray);
    return hit1.distance<hit2.distance?hit1:hit2;
}