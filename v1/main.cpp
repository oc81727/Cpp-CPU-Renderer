#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
// #include <list>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <stack>
// #include <chrono>




struct vec3 {
    float x;
    float y;
    float z;


    vec3(float a, float b, float c) : x(a), y(b), z(c) {}

    vec3() {}

    vec3 operator+(const float num) const {
        return vec3(x + num, y + num, z + num);
    }

    vec3 operator+(const vec3 v) const {
        return vec3(x + v.x, y + v.y, z + v.z);
    }

    vec3 operator-(const float num) const {
        return vec3(x - num, y - num, z - num);
    }

    vec3 operator-(const vec3 v) const {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    vec3 operator*(const float num) const {
        return vec3(x * num, y * num, z * num);
    }

    vec3 operator*(const vec3 v) const {
        return vec3(x * v.x, y * v.y, z * v.z);
    }

    vec3& operator+=(const float num) {
        x += num;
        y += num;
        z += num;
        return *this;
    }

    vec3& operator+=(const vec3 v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }


    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    vec3& norm() {
        float invL = 1 / length();
        x *= invL;
        y *= invL;
        z *= invL;
        return *this;
    }

    vec3 copy() {
        return vec3(x, y, z);
    }

    float min() {
        if (x < y) {
            if (x < z) {
                return x;
            }
            return z;
        }
        if (z < y) {
            if (z < x) {
                return z;
            }
            return x;
        }
        return y;
    }

    float max() {
        if (x > y) {
            if (x > z) {
                return x;
            }
            return z;
        }
        if (z > y) {
            if (z > x) {
                return z;
            }
            return x;
        }
        return y;
    }



};

std::ostream& operator<<(std::ostream& os, const vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


class AABB {
    public:
        vec3 pos; // corner not center
        vec3 size;
        vec3 maxPos;
        vec3 center;
        AABB(vec3 p, vec3 s) : pos(p), size(s){
            maxPos = p + s;
            center = pos + (size * 0.5);
        }

        AABB(){}

        bool test2(const vec3& start, const vec3& dir) const { // wip
            float invDirX = 1 / dir.x;
            float t1 = (pos.x - start.x) * invDirX;
            float t2 = (maxPos.x - start.x) * invDirX;
            return false;
        }

        bool test(const vec3& start, const vec3& dir, const vec3& invDir, vec3& hitPoint, float& distance) const { // optimise by checking 1 axis at a time and breaking early
            // pos = start + dir * d
            // d = (pos - start) / dir
            // vec3 invDir = vec3(1 / dir.x, 1 / dir.y, 1 / dir.z); // praying never 0, or cry
            vec3 distMin = (pos - start) * invDir;
            vec3 distMax = (maxPos - start) * invDir;
            vec3 enterNear;
            vec3 enterFar;
            if (distMin.x < distMax.x) {
                enterNear.x = distMin.x;
                enterFar.x = distMax.x;
            } else {
                enterNear.x = distMax.x;
                enterFar.x = distMin.x;
            }
            if (distMin.y < distMax.y) {
                enterNear.y = distMin.y;
                enterFar.y = distMax.y;
            } else {
                enterNear.y = distMax.y;
                enterFar.y = distMin.y;
            }
            if (distMin.z < distMax.z) {
                enterNear.z = distMin.z;
                enterFar.z = distMax.z;
            } else {
                enterNear.z = distMax.z;
                enterFar.z = distMin.z;
            }
            float enter = enterNear.max();
            float exit = enterFar.min();
            // return (enter <= exit) && exit >= 0;
            if (enter <= exit && exit >= 0) {
                hitPoint = start + dir * enter;
                distance = enter;
                return true;
                // return std::max(enter, 0.0f);
            }
            // std::cout<<"miss";
            return false;

        }
};

class Sphere {
    public:
        vec3 pos;
        float radius;
        vec3 color;
        AABB aabb;
        int index;

        Sphere(vec3 p, float r, vec3 c, int i) : pos(p), radius(r), color(c), index(i){
            aabb = AABB(pos - radius, vec3(radius * 2, radius * 2, radius * 2));
        }

        float dist(vec3 p) const {
            // return (pos - p).length() - radius;
            float dx = pos.x - p.x;
            float dy = pos.y - p.y;
            float dz = pos.z - p.z;
            return sqrt(dx * dx + dy * dy + dz * dz) - radius;
        }

};


struct sphereReturn {
    const float dist;
    const size_t closest;
};

float marchSphere(const vec3& pos, const vec3& dir, const Sphere& sphere) { // replace with direct calculation later

    vec3 marchPos = pos;
    float dist = 0;
    for (int marchCount = 0; marchCount < 200; marchCount++) {
        float d = sphere.dist(marchPos);
        dist += d;
        if (d < 0.001f) { // hit!
            return dist;
        }
        if (d > 10000) {
            return -1;
        }
        marchPos += dir * d;
    }
    return -1;
}

sphereReturn marchSpheres(const vec3 pos, const std::vector<Sphere>& spheres, const std::vector<size_t>& hits) {
    float dist = 1000000.0f;
    size_t closest = 0;
    for (const size_t& hit : hits)
    {
        float d = spheres[hit].dist(pos);

        if (d < dist)
        {
            dist = d;
            closest = hit;
        }
    }

    return {dist, closest};
}


class boundingVolume : public AABB {
    public:
        std::vector<Sphere> items;
        // std::vector<boundingVolume> children;
        std::unique_ptr<boundingVolume> child1;
        std::unique_ptr<boundingVolume> child2;
        bool isLeaf = false;
        boundingVolume(std::vector<Sphere>& i) : items(i) {
            // std::sort(i.begin(), i.end());
        }
        boundingVolume() {}

        // std::string text() {
        //     if 
        // }
        std::string print() {
            if (isLeaf) {
                return "<" + std::to_string(items.size()) + ">";
            }
            return "{" + child1->print() + ", " + child2->print() + "}";
        }

        void build() {
            // if (items.size() <= 5) {

            //     return;
            // }
            float minX = __FLT_MAX__;
            float minY = __FLT_MAX__;
            float minZ = __FLT_MAX__;

            float maxX = -__FLT_MAX__;
            float maxY = -__FLT_MAX__;
            float maxZ = -__FLT_MAX__;

            for (const Sphere& child : items) {
                if (child.aabb.pos.x < minX) {
                    minX = child.aabb.pos.x;
                }
                if (child.aabb.pos.y < minY) {
                    minY = child.aabb.pos.y;
                }
                if (child.aabb.pos.z < minZ) {
                    minZ = child.aabb.pos.z;
                }

                if (child.aabb.maxPos.x > maxX) {
                    maxX = child.aabb.maxPos.x;
                }
                if (child.aabb.maxPos.y > maxY) {
                    maxY = child.aabb.maxPos.y;
                }
                if (child.aabb.maxPos.z > maxZ) {
                    maxZ = child.aabb.maxPos.z;
                }
            }
            this->pos = vec3(minX, minY, minZ);
            this->maxPos = vec3(maxX, maxY, maxZ);
            if (items.size() <= 8) {
                isLeaf = true;
                return;
            }
            float dx = maxX - minX;
            float dy = maxY - minY;
            float dz = maxZ - minZ;
            float m = std::max(dx, std::max(dy, dz)); // lazy
            if (m == dx) {
                std::sort(items.begin(), items.end(), [](const Sphere& a, const Sphere& b) {
                        return a.pos.x < b.pos.x;
                    }
                );
            } else if (m == dy) {
                std::sort(items.begin(), items.end(), [](const Sphere& a, const Sphere& b) {
                        return a.pos.y < b.pos.y;
                    }
                );
            } else {
                std::sort(items.begin(), items.end(), [](const Sphere& a, const Sphere& b) {
                        return a.pos.z < b.pos.z;
                    }
                );
            }
            
            int mid = items.size() >> 1;
            std::vector<Sphere> a(items.begin(), items.begin() + mid);
            std::vector<Sphere> b(items.begin() + mid, items.end());
            child1 = std::make_unique<boundingVolume>(a);
            child2 = std::make_unique<boundingVolume>(b);
            child1->build();
            child2->build();
            
            
        }


        // size_t getHit(const vec3& start, const vec3& dir) const {
        //     if (!test(start, dir)) {
        //         return 0;
        //     }
        //     if (isLeaf) {
        //         float minD = __FLT_MAX__;
        //         size_t closest = 0;
        //         for (const Sphere& s : items) {
        //             float d = s.aabb.test(start, dir) ;
        //             if (d != 0 && d < minD) {
        //                 minD = d;
        //                 closest = s.index + 1;
        //             }
        //         }
        //         return minD;
        //     } else {
        //         float d1 = (start.x - child1->center.x) * (start.x - child1->center.x) + (start.y - child1->center.y) * (start.y - child1->center.y) + (start.z - child1->center.z) * (start.z - child1->center.z);
        //         float d2 = (start.x - child2->center.x) * (start.x - child2->center.x) + (start.y - child2->center.y) * (start.y - child2->center.y) + (start.z - child2->center.z) * (start.z - child2->center.z);
        //         if (d1 < d2) {
        //             size_t r = child1->getHit(start, dir);
        //             if (r == 0) {
        //                 return child2->getHit(start, dir);
        //             }
        //             return 0;
        //         }
        //         size_t r = child2->getHit(start, dir);
        //         if (r == 0) {
        //             return child1->getHit(start, dir);
        //         }
        //         return 0;
        //         // child1->getHits(start, dir, hits);
        //         // child2->getHits(start, dir, hits);
        //     }
        // }


        size_t march(const vec3& start, const vec3& dir) {
            boundingVolume* toProcess[64];
            int stackSize = 1;
            toProcess[0] = this;
            // std::stack<boundingVolume*> toProcess;
            // toProcess.push(this);
            vec3 hitPoint;
            vec3 currentHitPoint;
            float dist;
            float closestHitDist = __FLT_MAX__;
            size_t closestHitObject = 0;
            vec3 invDir = vec3(1 / dir.x, 1 / dir.y, 1 / dir.z);
            while (stackSize > 0) {
                boundingVolume* current = toProcess[--stackSize];

                if (!current->test(start, dir, invDir, hitPoint, dist)) { // replace hitPoint with currentHitPoint
                    // std::cout<<"missed"<<"\n";
                    
                    continue;
                }
                // std::cout<<toProcess.size()<<"\n";
                // std::cin.get();
                if (current->isLeaf) {
                    for (const Sphere& s : items) {
                        dist = marchSphere(start, dir, s); // replace start with currentHitPoint
                        if (dist != -1 && dist < closestHitDist) {
                            closestHitDist = dist;
                            closestHitObject = s.index;
                        }
                    }
                } else {
                    toProcess[stackSize++] = current->child1.get();
                    toProcess[stackSize++] = current->child2.get();
                    // float d1 = (start.x - child1->center.x) * (start.x - child1->center.x) + (start.y - child1->center.y) * (start.y - child1->center.y) + (start.z - child1->center.z) * (start.z - child1->center.z);
                    // float d2 = (start.x - child2->center.x) * (start.x - child2->center.x) + (start.y - child2->center.y) * (start.y - child2->center.y) + (start.z - child2->center.z) * (start.z - child2->center.z);
                    // if (d1 < closestHitDist) {
                    //     if (d2 < d1) {
                    //         toProcess.push(child2.get());
                    //         toProcess.push(child1.get());
                    //     } else if (d2 > closestHitDist) {
                    //         toProcess.push(child1.get());
                    //     }
                    // } else if (d2 < closestHitDist) {
                    //     toProcess.push(child2.get());
                    // }
                }
                // toProcess.pop();
            }
            if (closestHitDist < __FLT_MAX__) {
                return closestHitObject + 1;
            }
            return 0;


        }


        // void getHits(const vec3& start, const vec3& dir, std::vector<size_t>& hits) const {
        //     vec3 hitPoint;
        //     if (!test(start, dir, hitPoint)) {
        //         return;
        //     }
        //     if (isLeaf) {
        //         for (const Sphere& s : items) {
        //             if (s.aabb.test(start, dir, hitPoint)) {
        //                 hits.emplace_back(s.index);
        //             }
        //         }
                
        //     } else {
        //         child1->getHits(start, dir, hits);
        //         child2->getHits(start, dir, hits);
        //     }
        // } 


};






void renderRows(std::vector<double>& traversalTimes, std::vector<double>& marchTimes, int threadNum, int startRow, int endRow, vec3& playerPos, vec3& topLeftDir, float scaleX, float scaleY, const std::vector<Sphere>& spheres, int camWidth, std::vector<std::uint8_t>& pixels, boundingVolume& bvh) {

    // int endRow = startRow + rowNum;
    double traversalTime = 0;
    double marchTime = 0;
    for (int y = startRow; y < endRow; y++) {
        // std::cout<<y<<"\n";
        for (int x = 0; x < camWidth; x++) {
            // vec3 marchPos = playerPos;
            vec3 rayDir = topLeftDir;
            rayDir.x += x * scaleX;
            rayDir.y -= y * scaleY;
            rayDir.norm();

            size_t pixPointer = (camWidth * y + x) << 2;

            auto start = std::chrono::high_resolution_clock::now();
            // std::cout<<"starting traversal";
            // std::vector<size_t> hits;
            // bvh.getHits(playerPos, rayDir, hits);
            size_t hitObject = bvh.march(playerPos, rayDir);
            if (hitObject != 0) {
                pixels[pixPointer] = spheres[hitObject - 1].color.x;
                pixels[pixPointer + 1] = spheres[hitObject - 1].color.y;
                pixels[pixPointer + 2] = spheres[hitObject - 1].color.z;
                pixels[pixPointer + 3] = 255;
            } else {
                pixels[pixPointer] = 0;
                pixels[pixPointer + 1] = 0;
                pixels[pixPointer + 2] = 0;
                pixels[pixPointer + 3] = 255;
            }
            
            // std::cout<<"finished traversal";
            auto end = std::chrono::high_resolution_clock::now();

            traversalTime += std::chrono::duration<double, std::milli>(end - start).count();;


            // if (hits.size() > 0) {
            //     // std::cout<<"some hits\n";
            // }
            
            // std::vector<Sphere> hits = bvh.getHits(playerPos, rayDir);
            
            // std::vector<size_t> aabbPass;
            // for (size_t i = 0; i < spheres.size(); i++) {
            //     if (spheres[i].aabb.test(marchPos, rayDir)) {
            //         aabbPass.push_back(i);
            //     }
            // } 
            // if (hits.size() == 0) {
            //     pixels[pixPointer] = 0;
            //     pixels[pixPointer + 1] = 0;
            //     pixels[pixPointer + 2] = 0;
            //     pixels[pixPointer + 3] = 255;
            //     continue;
            // } else {
            //     // std::cout << "some hits\n";
            // }

            // int marchCount = 0;
            start = std::chrono::high_resolution_clock::now();

            // bool notBroke = true;
            // for (int marchCount = 0; marchCount < 100; marchCount++) { // avg 8 marches per pixel
            //     // marchCount += 1;
            //     sphereReturn marchRes = marchSpheres(marchPos, spheres, hits);
            //     if (marchRes.dist < 0.0001f) {
            //         // float th = tanh(marchCount * marchCount * marchCount * marchCount * 0.0005);
            //         pixels[pixPointer] = spheres[marchRes.closest].color.x; // * (1-th) + 255 * th;
            //         pixels[pixPointer + 1] = spheres[marchRes.closest].color.y; // * (1-th) + 255 * th;
            //         pixels[pixPointer + 2] = spheres[marchRes.closest].color.z; // * (1-th) + 255 * th;
            //         pixels[pixPointer + 3] = 255;
            //         // std::cout << "hit \n";
            //         notBroke = false;
            //         break;
            //     }
            //     if (marchRes.dist > 1000) {
            //         pixels[pixPointer] = 0;
            //         pixels[pixPointer + 1] = 0;
            //         pixels[pixPointer + 2] = 0;
            //         pixels[pixPointer + 3] = 255;
            //         notBroke = false;
            //         break;
            //     }
            //     marchPos += rayDir * marchRes.dist;
            // }
            // if (notBroke) {
            //     pixels[pixPointer] = 0;
            //     pixels[pixPointer + 1] = 0;
            //     pixels[pixPointer + 2] = 0;
            //     pixels[pixPointer + 3] = 255;
            // }
            end = std::chrono::high_resolution_clock::now();

            marchTime += std::chrono::duration<double, std::milli>(end - start).count();
            // std::cout << "traversal: " << traversalTime << "\n";
            // std::cout << "march: " << marchTime << "\n";
            traversalTimes[threadNum] += traversalTime;
            marchTimes[threadNum] += marchTime;
        }
    }


}



int main()
{
    

    const unsigned width = 800;
    const unsigned height = 500;

    vec3 playerPos = vec3(0, 0, 0);
    int camRes[2] = {800, 500};
    float camSize[2] = {1, 0.625};
    vec3 lookDir = vec3(0, 0, 1);
    float fov = 90;


    // const int threadsNum = std::thread::hardware_concurrency();
    const int threadsNum = 4;
    const int rowsPerThread = camRes[1] / threadsNum;

    float focalLength = camSize[0] * 0.5 / tan(fov * 0.00872665); // to rad then /2
    float scaleX = camSize[0] / camRes[0];
    float scaleY = camSize[1] / camRes[1];



    std::vector<Sphere> spheres;


    srand(3);
    for (int i = 0; i < 100; i++) {
        spheres.emplace_back(vec3(rand() % 500 - 250, rand() % 500 - 250, rand() % 500), rand() % 20, vec3(rand() % 256, rand() % 256, rand() % 256), i);
    }
    boundingVolume bvh(spheres);
    bvh.build();
    std::cout << bvh.print() << "\n";



    // spheres.emplace_back(vec3(50, -40, 200), 5.0f, vec3(255, 0, 0));
    // spheres.emplace_back(vec3(-60, 30, 150), 5.0f, vec3(0, 0, 255));



    sf::RenderWindow window(sf::VideoMode({width, height}), "Pixel Array");

    std::vector<std::uint8_t> pixels(width * height * 4);

    sf::Texture texture;
    bool success = texture.resize({width, height});

    sf::Sprite sprite(texture);
    // bool printed = false;
    int frameNum = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (window.isOpen())
    {
        
        if (frameNum % 50 == 0) {
            start = std::chrono::high_resolution_clock::now();
        }
        

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }


        vec3 lv = lookDir * focalLength;
        vec3 topLeft = playerPos + lv;
        topLeft.x -= camSize[0] * 0.5;
        topLeft.y += camSize[1] * 0.5;

        vec3 topLeftDir = lv.copy();
        topLeftDir.x -= camSize[0] * 0.5;
        topLeftDir.y += camSize[1] * 0.5;


        int pixPointer = 0;
        // float totalMarch = 0;

        


        // for (int y = 0; y < camRes[1]; y++) {
            // for (int x = 0; x < camRes[0]; x++) {
                
        
        
        std::vector<std::thread> threads;
        std::vector<double> traversalTimes;
        traversalTimes.resize(threadsNum);
        std::vector<double> marchTimes;
        marchTimes.resize(threadsNum);
        std::fill(traversalTimes.begin(), traversalTimes.end(), 0);
        std::fill(marchTimes.begin(), marchTimes.end(), 0);
        // renderRows(std::ref(traversalTimes), std::ref(marchTimes), 1, 0, camRes[1], std::ref(playerPos), std::ref(topLeftDir), scaleX, scaleY, std::cref(spheres), camRes[0], std::ref(pixels), std::ref(bvh));

        for (int chunk = 0; chunk < threadsNum; chunk++) {
            int end = (chunk + 1) * rowsPerThread;
            if (chunk == threadsNum - 1) {
                end = camRes[1];
            }
            threads.emplace_back(renderRows, std::ref(traversalTimes), std::ref(marchTimes), chunk, chunk * rowsPerThread, end, std::ref(playerPos), std::ref(topLeftDir), scaleX, scaleY, std::cref(spheres), camRes[0], std::ref(pixels), std::ref(bvh));
        }
        
        for (auto& thread : threads) {
            thread.join();
            
        }
        double tt = 0;
        double mt = 0;
        for (double s : traversalTimes) {
            tt += s;
        }
        for (double s : marchTimes) {
            mt += s;
        }
        std::cout << "traversal: " << tt / threadsNum * 0.001 << ", ";
        std::cout << "march: " << mt / threadsNum * 0.001 << ", ";
        std::cout << "total: " << ( mt + tt) / threadsNum * 0.001 << "\r";



        
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
        if (frameNum % 50 == 49) {
            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration<double, std::milli>(end - start).count();
            std::cout << "\n" << round(50000 / ms) << "fps\n";
        }
        frameNum += 1;
        // auto start = std::chrono::high_resolution_clock::now();
    }

    return 0;
}