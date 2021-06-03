#include <algorithm>
#include <cassert>
#include <chrono>
#include <vector>
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
    //std::cout<<"recursiveBuild_EqualCount"<<std::endl;
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
        std::vector<Object*> left = {objects[0]};
        node->left = recursiveBuild_EqualCount(left);
        std::vector<Object*> right = {objects[1]};
        node->right = recursiveBuild_EqualCount(right);

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
    
    //std::cout<<"recursiveBuild_SAH"<<std::endl;

    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
 
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
        int direction = bounds.maxExtent();
    if (objects.size() == 1) {
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        this->obj_num++;
        return node;
    }
    else if (objects.size() == 2) {
        std::vector<Object*> left = {objects[0]};
        node->left = recursiveBuild_SAH(left);
        std::vector<Object*> right = {objects[1]};
        node->right = recursiveBuild_SAH(right);

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        int buckets_counts = 8;
        std::vector<float> buckets_min_cost(buckets_counts);
        std::vector<Bounds3> buckets(buckets_counts);
        std::vector<std::vector<Object*>> buckets_objects(buckets_counts);
        

        //make buckets
        for(auto object : objects){
            int index;
            Vector3f offset = bounds.Offset(object->getBounds().Centroid());
            switch (direction)
            {
            case 0:
                index = offset.x*buckets_counts;
                break;
            case 1:
                index = offset.y*buckets_counts;
                break;
            case 2:
                index = offset.z*buckets_counts;
                break;
            }
            
            if (index == buckets_counts) index = buckets_counts -1;
            buckets[index] = Union(buckets[index],object->getBounds());
            buckets_objects[index].push_back(object);
            
        }
       
        //get the lowest cost of buckets 
        float cost = std::numeric_limits<float>().max();
        auto beginning = objects.begin();
        auto middling = objects.begin();
        auto ending = objects.end();

        Bounds3 b0,b1;
        float c0 = 0,c1 = 0;
        for(int i = 0;i < buckets_counts;i++){
            b0 = Union(b0,buckets[i]);
            c0 += buckets_objects[i].size();
            b1 = Bounds3();
            c1 = 0;
            for(int j = i + 1;j < buckets_counts;j++){
                  b1 = Union(b1,buckets[j]);
                  c1 += buckets_objects[j].size();
            }
            float temp = 1 + c0 * b0.SurfaceArea()/bounds.SurfaceArea() + c1 * b1.SurfaceArea()/bounds.SurfaceArea();
            if (cost > temp){
                cost = temp;
                middling = objects.begin() + c0;
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