#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
// #include <list>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>
#include <numeric>

using namespace std;



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

    float length2() const {
        return x * x + y * y + z * z;
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
        return std::min(x, std::min(y, z));
        // if (x < y) {
        //     if (x < z) {
        //         return x;
        //     }
        //     return z;
        // }
        // if (z < y) {
        //     if (z < x) {
        //         return z;
        //     }
        //     return x;
        // }
        // return y;
    }

    float max() {
        return std::max(x, std::max(y, z));
        // if (x > y) {
        //     if (x > z) {
        //         return x;
        //     }
        //     return z;
        // }
        // if (z > y) {
        //     if (z > x) {
        //         return z;
        //     }
        //     return x;
        // }
        // return y;
    }

    string str() const {
        return "(" + to_string(x) + ", "+ to_string(y) + ", " + to_string(z) +")";
    }

    void set(float a, float b, float c) {
        x = a;
        y = b;
        z = c;
    }



};

ostream& operator<<(ostream& os, const vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

vec3 minVal(vec3& a, vec3& b) {
    return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
vec3 maxVal(vec3& a, vec3& b) {
    return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

void minValKeep(vec3&a, vec3&b) {
    a.x = min(a.x, b.x);
    a.y = min(a.y, b.y);
    a.z = min(a.z, b.z);
}

void maxValKeep(vec3&a, vec3&b) {
    a.x = max(a.x, b.x);
    a.y = max(a.y, b.y);
    a.z = max(a.z, b.z);
}

vec3 avg(vec3& a, vec3& b) {
    return vec3((a.x + b.x) * 0.5, (a.y + b.y) * 0.5, (a.z + b.z) * 0.5);
}

enum ObjectType {
    BoxType,
    SphereType
};

class AABB {
    public:
        vec3 pos;
        vec3 size;
        vec3 maxPos;
        vec3 center;
        uint32_t index;
        ObjectType type;
        // float distCache;
        AABB() {}
        AABB(const vec3 p, const vec3 s) : pos(p), size(s) {
            maxPos = pos + size;
            center = pos + size * 0.5;
        }

        bool intersect(const vec3& start, const vec3& invDir, float& dist) const {
            // p = start + dir * t
            // t = (p - start) / dir
            vec3 t1 = (pos - start) * invDir;
            vec3 t2 = (maxPos - start) * invDir;
            vec3 low = minVal(t1, t2);
            vec3 high = maxVal(t1, t2);
            float tMin = low.max();
            float tMax = high.min();
            dist = max(tMin, 0.0f);
            // distCache = dist;
            return tMax >= dist;
        }
        
};


class Object : public AABB {
    // any scene object, spheres, boxes...
    public:
        int objectType = 0;
        vec3 color;
        size_t sceneIndex;
        Object(const vec3 p, const vec3 s, const vec3 c) : AABB(p, s), color(c) {}
        virtual float distance(const vec3& pos, const vec3& dir, const vec3& invDir) const {
            return -1;
        }
};

class Sphere : public Object {
    public:
        float radius;
        Sphere(const vec3 p, const float r, const vec3 c) : Object(p, vec3(r * 2, r * 2, r * 2), c), radius(r) {
            objectType = 1;
        }

        float distance(const vec3& pos, const vec3& dir, const vec3& invDir) const override {
            vec3 abc = pos - center;
            float a = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);
            float b = (abc.x * dir.x + abc.y * dir.y + abc.z * dir.z) * 2;
            float c = abc.x * abc.x + abc.y * abc.y + abc.z * abc.z - radius * radius;

            float disc = b * b - 4 * a * c;
            if (disc < 0) {
                return -1;
            }
            float qr = sqrt(disc);
            // float inv2a = 1 / (2 * a);
            // float t1 = - b - qr;
            float t1 = (-b - qr) / (2 * a);
            float t2 = (-b + qr) / (2 * a);
            if (t1 >= 0) {
                // return t1 * inv2a;
                return t1;
            }
            // float t2 = -b + qr;
            if (t2 >= 0) {
                // return t2 * inv2a;
                return t2;
            }
            return -1;

        }
};

class Box : public Object {
    public:
        Box(const vec3 p, const vec3 s, const vec3 c) : Object(p, s, c) {
            objectType = 2;
        }

        float distance(const vec3& point, const vec3& dir, const vec3& invDir) const override { 
            vec3 t1 = (pos - point) * invDir;
            vec3 t2 = (maxPos - point) * invDir;
            vec3 low = minVal(t1, t2);
            vec3 high = maxVal(t1, t2);
            float tMin = low.max();
            float tMax = high.min();
            float dist = max(tMin, 0.0f);
            if (tMax >= dist) {
                return dist;
            }
            return -1;
        }
};



// vec3 applyPitchYaw()

struct rotData {
    float pitch = 0;
    float yaw = 0;
    float roll = 0;
    vec3 dir = vec3(0, 0, 1);
    vec3 invDir = vec3(INFINITY, INFINITY, 1);

    float sp = 0;
    float cp = 1;
    float sy = 0;
    float cy = 1;
    float sr = 0;
    float cr = 1;

    float m00 = 1; float m01 = 0; float m02 = 0;
    float m10 = 0; float m11 = 1; float m12 = 0;
    float m20 = 0; float m21 = 0; float m22 = 1;

    const float rad = 0.017453292519943295;

    rotData(vec3 d) {
        pitch = d.x;
        yaw = d.y;
        roll = d.z;
    }
    rotData(float p, float y, float r) : pitch(p), yaw(y), roll(r) {
        update();
    }
    rotData() {}

    void update() {
        float pr = pitch * rad;
        float yr = yaw * rad;
        float rr = roll * rad;
        sp = sin(pr);
        cp = cos(pr);
        sy = sin(yr);
        cy = cos(yr);
        sr = sin(rr);
        cr = cos(rr);

        m00 = cy*cr + sy*sr*sp;
        m01 = -cy*sr + sy*cr*sp;
        m02 = sy*cp;
        m10 = cp*sr;
        m11 = cp*cr;
        m12 = -sp;
        m20 = -sy*cr + cy*sp*sr;
        m21 =  sy*sr + cy*cr*sp;
        m22 = cy*cp;

        dir = apply(vec3(0, 0, 1));
        invDir = vec3(1 / dir.x, 1 / dir.y, 1 / dir.z);
    }

    vec3 apply(const vec3& pos) const {
        // cycr + sysr*sp, -cy*sr + sycr*sp, sy*cp
        // cp*sr, cp*cr, -sp
        // -sycr + cy*sp*sr, sysr + cycr*sp, cy*cp
        return vec3(m00*pos.x + m01*pos.y + m02*pos.z,
                    m10*pos.x + m11*pos.y + m12*pos.z,
                    m20*pos.x + m21*pos.y + m22*pos.z);
    }
};

class BVH;

struct StackEntry {
    const BVH* node;
    float dist; 
    StackEntry() {}
    StackEntry(const BVH* item, float d) : node(item), dist(d) {}
};

class BVH : public AABB {
    public:
        unique_ptr<BVH> child1;
        unique_ptr<BVH> child2;
        bool isLeaf = false;
        bool isRoot = true;
        const int maxItems = 8;
        vector<Object*>& objects;
        uint32_t begin = 0;
        uint32_t end = 0;
        BVH* root;

        // BVH() {}
        BVH(vector<Object*>& objs) : objects(objs) {
            isRoot = true;
            end = objs.size() - 1;
            root = this;
        };
        BVH(uint32_t b, uint32_t e, BVH* r, vector<Object*>& o) : begin(b), end(e), root(r), objects(o) {
            isRoot = false;
        }

        string str() {
            if (isLeaf) {
                return "<" + to_string(end - begin) + ">";
            }
            return "{" + child1->str() + ", " + child2->str() + "}";
        }

        void build() {
            pos = vec3(__FLT_MAX__, __FLT_MAX__, __FLT_MAX__);
            maxPos = vec3(0, 0, 0);
            for (uint32_t i = begin; i < end; i++) {
                Object* o = root->objects[i];
                pos = minVal(pos, o->pos);
                maxPos = maxVal(maxPos, o->maxPos);
            }
            size = maxPos - pos;
            center = (pos + maxPos) * 0.5f;
            if (end - begin <= maxItems) {
                isLeaf = true;
                return;
            }
            if (size.max() == size.x) {
                sort(root->objects.begin() + begin, root->objects.begin() + end, [](const Object* a, const Object* b) {return a->center.x > b->center.x;});
            } else if (size.max() == size.y) {
                sort(root->objects.begin() + begin, root->objects.begin() + end, [](const Object* a, const Object* b) {return a->center.y > b->center.y;});
            } else {
                sort(root->objects.begin() + begin, root->objects.begin() + end, [](const Object* a, const Object* b) {return a->center.z > b->center.z;});
            } 
            uint32_t middle = (begin + end) / 2;
            child1 = make_unique<BVH>(begin, middle, root, root->objects);
            child2 = make_unique<BVH>(middle, end, root, root->objects);
            child1->build();
            child2->build();
        }

        // check root first
        // if root intersects then continue
        // if not leaf then
        // check 
        void cast(const vec3& start, const vec3& dir, const vec3& invDir, float& objectDist, size_t& objectIndex, int& aabbTests, int& primativeTests, int& bvhaabb) const {
            // cout << "starting cast!\n";
            StackEntry stack[64];
            int stackSize = 0;
            float dist;
            float hitDist;
            float closestDist = __FLT_MAX__;
            size_t closestObject = 0;
            bvhaabb++;
            if (intersect(start, invDir, dist)) {
                stack[stackSize++] = StackEntry(this, dist);
            } else {
                return;
            }

            
            while (stackSize > 0) {
                const StackEntry& currentEntry = stack[--stackSize];
                const BVH& currentNode = *(currentEntry.node);
                float nodeDist = currentEntry.dist;
                if (nodeDist > closestDist) {
                    continue;
                }
                if (currentNode.isLeaf) {
                    for (uint32_t i = currentNode.begin; i < currentNode.end; i++) {
                        auto& object = objects[i];
                        
                        if (object->objectType == 2) {
                            aabbTests++;
                            if (!object->intersect(start, invDir, dist)) {
                                continue;
                            }
                            hitDist = dist;
                            if (hitDist != -1 && hitDist < closestDist) {
                                closestDist = hitDist;
                                closestObject = object->sceneIndex;
                            }
                        } else {
                            primativeTests++;
                            hitDist = object->distance(start, dir, invDir);
                            if (hitDist != -1 && hitDist < closestDist) {
                                closestDist = hitDist;
                                closestObject = object->sceneIndex;
                            }
                        }
                        // if (object->intersect(start, invDir, dist)) {
                        //     if (object->objectType == 2) { // type box
                        //         hitDist = dist;
                        //     } else {
                        //         primativeTests++;
                        //         hitDist = object->distance(start, dir, invDir);
                        //     }
                        //     if (hitDist != -1 && hitDist < closestDist) {
                        //         closestDist = hitDist;
                        //         closestObject = object->sceneIndex;
                        //     }
                        // }
                    }
                } else {
                    float d1 = 0;
                    float d2 = 0;
                    bvhaabb+= 2;
                    bool h1 = currentNode.child1->intersect(start, invDir, d1);
                    bool h2 = currentNode.child2->intersect(start, invDir, d2);
                    if (h1 && h2) {
                        if (d1 < d2) {
                            if (d2 < closestDist) {
                                stack[stackSize++] = {currentNode.child2.get(), d2};
                            }
                            if (d1 < closestDist) {
                                stack[stackSize++] = {currentNode.child1.get(), d1};
                            }
                        } else {
                            if (d1 < closestDist) {
                                stack[stackSize++] = {currentNode.child1.get(), d1};
                            }
                            if (d2 < closestDist) {
                                stack[stackSize++] = {currentNode.child2.get(), d2};
                            }
                        }
                    } else if (h1 && d1 < closestDist) {
                        stack[stackSize++] = {currentNode.child1.get(), d1};
                    } else if (h2 && d2 < closestDist) {
                        stack[stackSize++] = {currentNode.child2.get(), d2};
                    }
                    
                }
            }
            objectDist = closestDist;
            objectIndex = closestObject;
        }

};



class Scene {
    public:
        vector<unique_ptr<Object>> objects;
        vector<Object*> bvhObjects;
        BVH bvh;
        Scene() : objects(), bvh(bvhObjects) {};

        void remove(size_t i) {
            objects[i] = move(objects.back());
            objects[i]->sceneIndex = i;
            objects.pop_back();
        }

        void add(unique_ptr<Object> o) {
            o->sceneIndex = objects.size();
            Object* ptr = o.get();
            objects.push_back(std::move(o));
            // cout<<"added\n";
            bvh.objects.push_back(ptr);
            // cout<<"size now: "<<bvh.objects.size()<<"\n";
            bvh.end += 1;
        }
};

struct BVHNode : public AABB {
    uint32_t start;
    uint32_t length;


    bool child1Set = false;
    bool child2Set = false;
    bool isLeaf = false;

    uint32_t child1;
    uint32_t child2;
    int32_t parent; // parent index
    uint32_t index; // PrimativeRef vector index

    BVHNode() {}
    BVHNode(uint32_t st, uint32_t si, int32_t i) : start(st), length(si), index(i) {}


};



class Primative : public AABB {
    public:
    Primative(vec3& p, vec3& s) : AABB(p, s) {}
};


struct Boxv2 : Primative {
    vec3 color;
    ObjectType type = BoxType;
    Boxv2(vec3 p, vec3 s, vec3 c) : Primative(p, s), color(c) {
        // pos = p;
        // size = s;
    } 
};

struct Spherev2 : Primative {
    vec3 color;
    ObjectType type = SphereType;
    float radius;
    Spherev2(vec3 p, vec3 s, vec3 c, float r) : Primative(p, s), color(c), radius(r) {
        // pos = p;
        // size = vec3(2 * r, 2 * r, 2 * r);
    }

};


struct PrimativeRef {
    ObjectType type;
    uint32_t index;
    PrimativeRef(ObjectType t, uint32_t i) : type(t), index(i) {}


};




struct Scenev2 {
    vector<Boxv2> boxes; 
    vector<Spherev2> spheres;
    vector<BVHNode> BVHNodes;
    vector<PrimativeRef> primatives;
    int items;
    const int maxLeafSize = 8;


    vec3 getCenter(const PrimativeRef ref) {
        if (ref.type == BoxType) {
            return boxes[ref.index].center;
        }
        if (ref.type == SphereType) {
            return spheres[ref.index].center;
        } // more in the future maybe
        cout << "getting center of invalid type\n";
        cout<<ref.type<<"\n";
        return vec3(0, 0, 0);
    }

    void addBox(const vec3& pos, const vec3& size, const vec3& color) {
        Boxv2 obj = Boxv2(pos, size, color);
        primatives.push_back(PrimativeRef(BoxType, boxes.size()));
        boxes.push_back(obj);
        items++;
    }

    void addSphere(const vec3& pos, const float radius, const vec3& color) {
        vec3 size = vec3(2 * radius, 2 * radius, 2 * radius);
        Spherev2 obj = Spherev2(pos, size, color, radius);
        primatives.push_back(PrimativeRef(SphereType, boxes.size()));
        spheres.push_back(obj);
        items++;
    }


    string bvhString(uint32_t index = 0) {
        BVHNode& current = BVHNodes[index];
        // P
        if (current.isLeaf) {
            return "<" + to_string(current.length) + ">";
        }
        return "{" + bvhString(current.child1) + ", " + bvhString(current.child2) + "}";
    }

    void buildBVH() {
        // uint32_t begin;
        // uint32_t size;
        float totalRatio = 0;
        int ratioNum = 0;
        uint32_t begin[64];
        uint32_t size[64];
        int32_t parent[64];

        begin[0] = 0;
        size[0] = primatives.size();
        parent[0] = -1;
        int stackTop = 0;

        Primative* p;
        while (stackTop >= 0) {
            uint32_t currentBegin = begin[stackTop];
            uint32_t currentSize = size[stackTop];
            int32_t currentParent = parent[stackTop--];
            // cout<<"currentBegin: "<<currentBegin<<", size: "<<currentSize<<"\n";
            vec3 minPos = vec3(INFINITY, INFINITY, INFINITY);
            vec3 maxPos = vec3(-INFINITY, -INFINITY, -INFINITY);
            // for (PrimativeRef& ref : primatives) {
            for (int i = currentBegin; i < currentBegin + currentSize; i++) {
                PrimativeRef& ref = primatives[i];
                if (ref.type == BoxType) {
                    Boxv2 object = boxes[ref.index];
                    minPos = minVal(minPos, object.pos);
                    maxPos = maxVal(maxPos, object.maxPos);
                    // cout<<"maxPos "<<object.maxPos.str()<<"\n";
                } else { // assume must be sphere, i mean what else could it be right aha
                    Spherev2 object = spheres[ref.index];
                    minPos = minVal(minPos, object.pos);
                    maxPos = maxVal(maxPos, object.maxPos);
                }
                
            }
            // cout<<"maxPosfrfr! "<<maxPos.str()<<"\n";
            vec3 boundSize = maxPos - minPos;
            int32_t thisParent = BVHNodes.size();
            BVHNode bn = BVHNode(currentBegin, currentSize, thisParent);
            cout<<"created node: "<<currentBegin<<", "<<currentSize<<", "<<thisParent<<"\n";
            bn.pos = minPos;
            bn.maxPos = maxPos;
            // cout<<bn.pos.str()<<", "<<bn.maxPos.str()<<"\n";
            bn.size = boundSize;
            bn.center = minPos + boundSize * 0.5;
            bn.parent = currentParent;
            if (currentParent != -1) {
                totalRatio += BVHNodes[currentParent].size.x * BVHNodes[currentParent].size.y * BVHNodes[currentParent].size.x /  (boundSize.x * boundSize.y * boundSize.z);
                ratioNum++;
                if (!BVHNodes[currentParent].child1Set) {
                    BVHNodes[currentParent].child1 = thisParent;
                    BVHNodes[currentParent].child1Set = true;
                } else {
                    BVHNodes[currentParent].child2 = thisParent;
                    BVHNodes[currentParent].child2Set = true;
                }
            }
            if (currentSize <= maxLeafSize) {
                bn.isLeaf = true;
                BVHNodes.push_back(bn);
                continue;
            }
            BVHNodes.push_back(bn);
            if (boundSize.max() == boundSize.x) {
                sort(primatives.begin() + currentBegin, primatives.begin() + currentBegin + currentSize, [&](const PrimativeRef& a, const PrimativeRef& b) {return getCenter(a).x > getCenter(b).x;});
            } else if (boundSize.max() == boundSize.y) {
                sort(primatives.begin() + currentBegin, primatives.begin() + currentBegin + currentSize, [&](const PrimativeRef& a, const PrimativeRef& b) {return getCenter(a).y > getCenter(b).y;});
            } else {
                sort(primatives.begin() + currentBegin, primatives.begin() + currentBegin + currentSize, [&](const PrimativeRef& a, const PrimativeRef& b) {return getCenter(a).z > getCenter(b).z;});
            }
            uint32_t middle = currentSize * 0.5;
            cout<<"creating children: "<<currentBegin<<", "<<currentBegin + middle<<", "<<currentBegin+currentSize<<"\n";
            begin[++stackTop] = currentBegin;
            size[stackTop] = middle;
            parent[stackTop] = thisParent;
            begin[++stackTop] = currentBegin + middle;
            size[stackTop] = currentSize - middle; // middle may have rounded down
            parent[stackTop] = thisParent;

            
        }
        cout<<"average bvh parent/child volume ratio: "<<totalRatio/ratioNum<<"\n";

        cout<<"test!"<<max(-INFINITY, 69.0f)<<"\n";
        
    }

    void cast(const vec3& start, const vec3& dir, const vec3& invDir, vec3& color) const {
        float closestDist = INFINITY;
        uint32_t closestObject;
        uint32_t BVHStack[64];
        float distStack[64];
        int stackTop = 0;
        float distReturn;
        // if (!BVHNodes[0].intersect(start, invDir, distReturn)) {
        //     color = vec3(0, 0, 0);
        //     return;
        // }
        if (BVHNodes[0].intersect(start, invDir, distReturn)) {
            BVHStack[0] = 0; // already 0 by default?
            distStack[0] = distReturn;
        }
        // cout<<"hit root";

        while (stackTop >= 0) {
            uint32_t currentIndex = BVHStack[stackTop];
            float currentDist = distStack[stackTop--];
            if (currentDist >= closestDist) {
                continue;
            }
            const BVHNode& currentNode = BVHNodes[currentIndex];
            if (currentNode.isLeaf) {
                // cout<<"hit leaf";
                float d;
                for (int i = currentNode.start; i < currentNode.start + currentNode.length; i++) {
                    const PrimativeRef& ref = primatives[i];
                    if (ref.type == BoxType) {
                        const Boxv2& object = boxes[ref.index];
                        vec3 t1 = (object.pos - start) * invDir;
                        vec3 t2 = (object.maxPos - start) * invDir;
                        vec3 low = minVal(t1, t2);
                        vec3 high = maxVal(t1, t2);
                        float tMin = low.max();
                        float tMax = high.min();
                        d = max(tMin, 0.0f);
                        if (tMax >= d && d < closestDist) {
                            closestDist = d;
                            closestObject = i;
                            // cout<<"hit box\n";
                            // cout<<object.pos.str()<<", "<<object.maxPos.str()<<"\n";
                        } else {
                            // cout<<"missed box\n";
                        }
                    } else if (ref.type == SphereType) {
                        const Spherev2& object = spheres[ref.index];
                        // cout<<object.pos.str()<<", "<<object.maxPos.str()<<", "<<object.radius<<", "<<object.center.str()<<"\n";
                        vec3 abc = start - object.center; // can precalculate?
                        float a = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);
                        float b = (abc.x * dir.x + abc.y * dir.y + abc.z * dir.z) * 2;
                        float c = abc.x * abc.x + abc.y * abc.y + abc.z * abc.z - object.radius * object.radius; // can precalculate?

                        float disc = b * b - 4 * a * c;
                        if (disc < 0) {continue;}
                        float qr = sqrt(disc);
                        // float inv2a = 1 / (2 * a);
                        // float t1 = - b - qr;
                        float t1 = (-b - qr) / (2 * a);
                        float t2 = (-b + qr) / (2 * a);
                        if (t1 >= 0 && t1 < closestDist) {
                            closestDist = t1;
                            closestObject = i;
                        } else if (t2 >= 0 && t2 < closestDist) {
                            closestDist = t2;
                            closestObject = i;
                        }
                    }
                }
            } else {
                float d1;
                float d2;
                uint32_t c1Index = currentNode.child1;
                uint32_t c2Index = currentNode.child2;
                bool c1Hit = BVHNodes[c1Index].intersect(start, invDir, d1);
                bool c2Hit = BVHNodes[c2Index].intersect(start, invDir, d2);
                // cout<<c1Hit<<c2Hit;
                if (c1Hit && c2Hit && d1 < closestDist && d2 < closestDist) {
                    // cout<<"both children";
                    if (d1 < d2) {
                        BVHStack[++stackTop] = c2Index;
                        distStack[stackTop] = d2;
                        BVHStack[++stackTop] = c1Index;
                        distStack[stackTop] = d1;
                    } else {
                        BVHStack[++stackTop] = c1Index;
                        distStack[stackTop] = d1;
                        BVHStack[++stackTop] = c2Index;
                        distStack[stackTop] = d2;
                    }
                } else if (c1Hit && d1 < closestDist) {
                    // cout<<"child1";
                    BVHStack[++stackTop] = c1Index;
                    distStack[stackTop] = d1;
                } else if (c2Hit && d2 < closestDist) {
                    // cout<<"child2";
                    BVHStack[++stackTop] = c2Index;
                    distStack[stackTop] = d2;
                }
            }
        } 
        if (closestDist != INFINITY) {
            const PrimativeRef& ref = primatives[closestObject];
            if (ref.type == BoxType) {
                const Boxv2& object = boxes[ref.index];
                color = object.color;
            } else if (ref.type == SphereType) {
                const Spherev2& object = spheres[ref.index];
                color = object.color;
            }
        } else {
            color = vec3(0, 0, 0);
        }
    }

    // void add(const ObjectType, AABB& object) {
    //     if (object.type == SphereType) {
    //         Spherev2& obj = static_cast<Spherev2&>(object);
    //         spheres.push_back(obj);
    //     } else if (object.type == BoxType) {
    //         Boxv2& obj = static_cast<Boxv2&>(object);
    //         boxes.push_back(obj);
    //     }
    // }
};



struct Objectv3 {
    vec3 minPos;
    vec3 maxPos;
    // uint32_t dataIndex;
    Objectv3(const vec3& pos, const vec3& maxPos) : minPos(pos), maxPos(maxPos) {}
};

struct ObjectData {
    ObjectType type;
    vec3 color;
    ObjectData(const ObjectType type, const vec3& color) : type(type), color(color) {}
};


bool AABBIntersect(const vec3& start, const vec3& invDir, const vec3& minPos, const vec3& maxPos, float& dist) {
    vec3 t1 = (minPos - start) * invDir;
    vec3 t2 = (maxPos - start) * invDir;
    vec3 low = minVal(t1, t2);
    vec3 high = maxVal(t1, t2);
    float tMin = low.max();
    float tMax = high.min();
    dist = max(tMin, 0.0f);
    // distCache = dist;
    return tMax >= dist;
}


template <typename T>
void printVec(const std::vector<T>& vec)
{
    for (const T& value : vec)
    {
        cout<<value;
    }
    cout<<"\n";
}

struct BVHNodev3 {
    vec3 minPos;
    vec3 maxPos;
    BVHNodev3(vec3& minPos, vec3& maxPos): minPos(minPos), maxPos(maxPos) {}
    float volume() {
        vec3 size = maxPos - minPos;
        return size.x * size.y * size.z;
    }


    inline bool intersect(const vec3& start, const vec3& invDir, float& dist) const {
        vec3 t1 = (minPos - start) * invDir;
        vec3 t2 = (maxPos - start) * invDir;
        vec3 low = minVal(t1, t2);
        vec3 high = maxVal(t1, t2);
        float tMin = low.max();
        float tMax = high.min();
        dist = max(tMin, 0.0f);
        // distCache = dist;
        return tMax >= dist;
    }
};

struct BVHRelation { // compact later maybe
    uint32_t start;
    uint32_t end;
    // uint32_t parent; // unused
    uint32_t child1;
    uint32_t child2;
    // BVHRelation(uint32_t parent) : parent(parent) {}
    BVHRelation(uint32_t start, uint32_t end) : start(start), end(end) {
        child1 = 0;
        child2 = 0;
    }
    string str() {
        return "start: " + to_string(start) + ", end: " + to_string(end) + ", parent: got rid of that hehe" + ", child1: " + to_string(child1) + ", child2: " + to_string(child2);
    }
};




struct Scenev3 {
    vector<Objectv3> objects;
    vector<ObjectData> data;
    
    vector<BVHNodev3> BVHNodes;
    vector<BVHRelation> BVHRelations;

    const int maxLeafSize = 4;

    void addBox(const vec3& pos, const vec3& size, const vec3& color) {
        vec3 maxPos = pos + size;
        objects.emplace_back(pos, maxPos);
        data.emplace_back(BoxType, color);
    }

    void addSphere(const vec3& pos, const float radius, const vec3& color) {
        // pos is center pos
        vec3 minPos = vec3(pos.x - radius, pos.y - radius, pos.z - radius);
        vec3 maxPos = vec3(pos.x + radius, pos.y + radius, pos.z + radius);
        objects.emplace_back(minPos, maxPos);
        data.emplace_back(SphereType, color);
    }

    void sort(uint32_t start, uint32_t end, int dir) {
        // sort by center of objects
        // dir: 0 = x, 1 = y, 2 = z
        vector<uint32_t> indices(end - start);
        iota(indices.begin(), indices.end(), start);
        if (dir == 0) {
            std::sort(indices.begin(), indices.end(), [&](uint32_t a, uint32_t b) {
                return (objects[a].minPos.x + objects[a].maxPos.x) < (objects[b].minPos.x + objects[b].maxPos.x);
                // dont need the * 0.5 for averaging probably
            });
        } else if (dir == 1) {
            std::sort(indices.begin(), indices.end(), [&](uint32_t a, uint32_t b) {
                return (objects[a].minPos.y + objects[a].maxPos.y) < (objects[b].minPos.y + objects[b].maxPos.y);
            });
        } else if (dir == 2) {
            std::sort(indices.begin(), indices.end(), [&](uint32_t a, uint32_t b) {
                return (objects[a].minPos.z + objects[a].maxPos.z) < (objects[b].minPos.z + objects[b].maxPos.z);
            });
        }
        vector<uint32_t> inverse(indices.size());
        for (int i = 0; i < indices.size(); i++) {
            inverse[indices[i] - start] = i;
        }
        for (int n = 0; n < indices.size(); n++) {
            int i = indices[n];
            swap(objects[n + start], objects[i]);
            swap(data[n + start], data[i]);
            int j = inverse[n];
            indices[j] = i;
            inverse[i - start] = j;
        }
    
    }

    string BVHString(uint32_t index = 0) {
        BVHRelation& current = BVHRelations[index];
        if (current.end - current.start <= maxLeafSize) {
            return "<" + to_string(current.end - current.start) + ">";
        }
        return "{"+ BVHString(current.child1) + ", " + BVHString(current.child2) + "}";
    }

    void buildBVH() {
        uint32_t starts[64];
        uint32_t ends[64];
        uint32_t parents[64];
        int stackTop = 0;

        float totalRatio = 0;
        int ratioNum = 0;
        
        starts[0] = 0;
        ends[0] = objects.size();
        parents[0] = 0; // root is its own parent, surely nothing goes wrong aha

        // cout<<"starting bvh build!\n";
        while (stackTop >= 0) {
            uint32_t currentStart = starts[stackTop];
            uint32_t currentEnd = ends[stackTop];
            uint32_t currentParent = parents[stackTop--];
            // cout<<"start: "<<to_string(currentStart)<<", end: "<<to_string(currentEnd)<<", parent: "<<to_string(currentParent)<<"\n";

            vec3 minPos = vec3(INFINITY, INFINITY, INFINITY);
            vec3 maxPos = vec3(-INFINITY, -INFINITY, -INFINITY);

            for (int i = currentStart; i < currentEnd; i++) {
                // minPos = minVal(objects[i].minPos, minPos);
                // maxPos = maxVal(objects[i].maxPos, maxPos);
                minValKeep(minPos, objects[i].minPos);
                maxValKeep(maxPos, objects[i].maxPos);
            }
            BVHNodes.emplace_back(minPos, maxPos);
            // if (currentParent != 0) {
            BVHRelations.emplace_back(currentStart, currentEnd);
            // }

            if (BVHNodes.size() > 1) { // so the current node is not the root
                totalRatio += BVHNodes[currentParent].volume() / BVHNodes[BVHNodes.size() - 1].volume();
                ratioNum++;
                if (BVHRelations[currentParent].child1 == 0) {
                    BVHRelations[currentParent].child1 = BVHNodes.size() - 1;
                } else {
                    BVHRelations[currentParent].child2 = BVHNodes.size() - 1;
                }
            }

            if (currentEnd - currentStart < maxLeafSize) { // is leaf
                continue;
            }
            

            vec3 size = maxPos - minPos;
            if (size.max() == size.x) {
                sort(currentStart, currentEnd, 0);
            } else if (size.max() == size.y) {
                sort(currentStart, currentEnd, 1);
            } else {// if (size.max() == size.z) {
                sort(currentStart, currentEnd, 2);
            }
            uint32_t middle = (currentEnd + currentStart) >> 1;
            starts[++stackTop] = currentStart;
            ends[stackTop] = middle;
            parents[stackTop] = BVHNodes.size() - 1;
            starts[++stackTop] = middle;
            ends[stackTop] = currentEnd;
            parents[stackTop] = BVHNodes.size() - 1;
        }
        cout<<"average bvh parent/child volume ratio: "<<totalRatio/ratioNum<<"\n";
    }


    void cast(const vec3& start, const vec3& dir, const vec3& invDir, vec3& color, int& BVHNodesTraversed, int& BVHAABBTests, int& boxTests, int& boxHits, int& sphereTests, int& sphereHits) const {
        uint32_t BVHStack[64];
        float distanceStack[64];
        int stackTop = 0;

        float closestDist = INFINITY;
        uint32_t closestObject;
        float distReturn;

        // int BVHNodesTraversed = 0;
        // int BVHAABBTests = 0;
        // int boxTests = 0;
        // int boxHits = 0;
        // int sphereTests = 0;
        // int sphereHits = 0;



        BVHAABBTests++;
        if (BVHNodes[0].intersect(start, invDir, distReturn)) {
            BVHStack[0] = 0;
            distanceStack[0] = distReturn;
        } else {
            color.set(0, 0, 0);
            return;
        }
        while (stackTop >= 0) {
            BVHNodesTraversed++;
            float currentDist = distanceStack[stackTop];
            uint32_t currentIndex = BVHStack[stackTop--];
            if (currentDist > closestDist) {
                continue;
            }
            const BVHRelation& currentRelation = BVHRelations[currentIndex];
            const BVHNodev3& currentNode = BVHNodes[currentIndex];
            if (currentRelation.end - currentRelation.start <= maxLeafSize) { // is leaf
                // continue;
                for (int i = currentRelation.start; i < currentRelation.end; i++) {
                    const Objectv3& currentObject = objects[i];
                    const ObjectData& currentData = data[i];
                    if (currentData.type == BoxType) {
                        boxTests++;
                        vec3 t1 = (currentObject.minPos - start) * invDir;
                        vec3 t2 = (currentObject.maxPos - start) * invDir;
                        vec3 low = minVal(t1, t2);
                        vec3 high = maxVal(t1, t2);
                        float tMin = low.max();
                        float tMax = high.min();
                        float dist = max(tMin, 0.0f);
                        // distCache = dist;
                        if (tMax >= dist && dist < closestDist) {
                            boxHits++;
                            closestDist = dist;
                            closestObject = i;
                        }
                    } else if (currentData.type == SphereType) {
                        sphereTests++;
                        vec3 center = (currentObject.minPos + currentObject.maxPos) * 0.5;
                        float radius = (currentObject.maxPos.x - currentObject.minPos.x) * 0.5;
                        vec3 abc = start - center; // can precalculate?
                        float a = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);
                        float b = (abc.x * dir.x + abc.y * dir.y + abc.z * dir.z) * 2;
                        float c = abc.x * abc.x + abc.y * abc.y + abc.z * abc.z - radius * radius; // can precalculate?
                        float disc = b * b - 4 * a * c;
                        if (disc < 0) {continue;}
                        float qr = sqrt(disc);
                        // float inv2a = 1 / (2 * a);
                        // float t1 = - b - qr;
                        float t1 = (-b - qr) / (2 * a);
                        float t2 = (-b + qr) / (2 * a);
                        if (t1 >= 0 && t1 < closestDist) {
                            closestDist = t1;
                            closestObject = i;
                            sphereHits++;
                        } else if (t2 >= 0 && t2 < closestDist) {
                            closestDist = t2;
                            closestObject = i;
                            sphereHits++;
                        }

                    }

                }
            } else {
                float d1;
                float d2;
                uint32_t c1 = currentRelation.child1;
                uint32_t c2 = currentRelation.child2;
                BVHAABBTests += 2;
                bool hit1 = BVHNodes[c1].intersect(start, invDir, d1);
                bool hit2 = BVHNodes[c2].intersect(start, invDir, d2);
                if (hit1 && hit2 && d1 < closestDist && d2 < closestDist) {
                    if (d1 < d2) {
                        BVHStack[++stackTop] = c2;
                        distanceStack[stackTop] = d2;
                        BVHStack[++stackTop] = c1;
                        distanceStack[stackTop] = d1;
                    } else {
                        BVHStack[++stackTop] = c1;
                        distanceStack[stackTop] = d1;
                        BVHStack[++stackTop] = c2;
                        distanceStack[stackTop] = d2;
                    }
                } else if (hit1 && d1 < closestDist) {
                    BVHStack[++stackTop] = c1;
                    distanceStack[stackTop] = d1;
                } else if (hit2 && d2 < closestDist) {
                    BVHStack[++stackTop] = c2;
                    distanceStack[stackTop] = d2;
                }
            }

        }
        if (closestDist == INFINITY) {
            color.set(0, 0, 0);
            return;
        }
        color = data[closestObject].color;


    }


};



vec3 randColor() {
    return vec3(rand()%256, rand()%256, rand()%256);
}



class Camera {
    public:
        vec3 pos;
        rotData camRot;
        // int camRes[2];
        int resX;
        int resY;
        float sizeX;
        float sizeY;
        // float camSize[2];
        float fov;
        float focalLength;
        float pxSizeX;
        float pxSizeY;

        vec3 topLeft; // vector from cam origin to top left pixel
        vec3 right; // vector from pixel to the one 
        vec3 down;

        Camera(vec3 p, rotData cr, float f, int rX, int rY, float sX, float sY) : 
        pos(p), camRot(cr), fov(f), resX(rX), resY(rY), sizeX(sX), sizeY(sY) {
            pxSizeX = sizeX / resX;
            pxSizeY = sizeY / resY;
            updateFov();
            
        }

        void updateFov() {
            focalLength = sizeX * 0.5 / tan(fov * 0.00872665);
            right = camRot.apply(vec3(pxSizeX, 0, 0));
            down = camRot.apply(vec3(0, -pxSizeY, 0));
            topLeft = pos + camRot.dir * focalLength - right * resX * 0.5 - down * resY * 0.5;

        }

        string cast(int x, int y, const Scene& scene) {
            vec3 rayDir = topLeft - pos + down * y + right * x;
            vec3 invDir = vec3(1 / rayDir.x, 1 / rayDir.y, 1 / rayDir.z);
            float closestHitDist = __FLT_MAX__;
            size_t closestHitObject = 0;
            for (auto& object : scene.objects) {
                float dist = 0;
                if (object->intersect(pos, invDir, dist)) {
                    float objectDist = object->distance(pos, rayDir, invDir);
                    if (objectDist != -1 && objectDist < closestHitDist) {
                        closestHitDist = objectDist;
                        closestHitObject = object->sceneIndex;
                    }
                }
            }
            if (closestHitDist == __FLT_MAX__) {
                return "miss";
            }
            return "hit " + to_string(scene.objects[closestHitObject]->objectType) + ", dist: " + to_string(closestHitDist);
        }

        void renderRowsv3(int start, int end, vector<uint8_t>& pixels, const Scenev3& scene) {
            int at = 0;
            int pt = 0;
            int bt = 0;

            int BVHNodesTraversed = 0;
            int BVHAABBTests = 0;
            int boxTests = 0;
            int boxHits = 0;
            int sphereTests = 0;
            int sphereHits = 0;

            vec3 rayDirTop = topLeft - pos + down * start;
            float objectDist = 0;
            for (int y = start; y < end; y++) {
                vec3 rayDir = rayDirTop;
                for (int x = 0; x < resX; x++) {
                    vec3 invDir = vec3(1 / rayDir.x, 1 / rayDir.y, 1 / rayDir.z);
                    size_t pixPointer = (y * resX + x) << 2;
                    vec3 color;
                    scene.cast(pos, rayDir, invDir, color, BVHNodesTraversed, BVHAABBTests, boxTests, boxHits, sphereTests, sphereHits);
                    pixels[pixPointer++] = color.x;
                    pixels[pixPointer++] = color.y;
                    pixels[pixPointer++] = color.z;
                    pixels[pixPointer++] = 255;
                    rayDir += right;
                }
                rayDirTop += down;
            }
            // cout<<"rays: "<<(end - start) * resX<<", BVHNodesTraversed: "<<BVHNodesTraversed<<", BVHAABBTests: "<<BVHAABBTests<<", boxTests: "<<boxTests<<", boxHits: "<<boxHits<<", sphereTests: "<<sphereTests<<", sphereHits: "<<sphereHits<<"\n";
            // cout<<" aabb tests: "<<at<<", primative tests: "<<pt<<", bvh aabb: "<<bt<<"\n";
        }

        void renderRowsv2(int start, int end, vector<uint8_t>& pixels, const Scenev2& scene) {
            int at = 0;
            int pt = 0;
            int bt = 0;
            vec3 rayDirTop = topLeft - pos + down * start;
            float objectDist = 0;
            for (int y = start; y < end; y++) {
                vec3 rayDir = rayDirTop;
                for (int x = 0; x < resX; x++) {
                    vec3 invDir = vec3(1 / rayDir.x, 1 / rayDir.y, 1 / rayDir.z);
                    size_t pixPointer = (y * resX + x) << 2;
                    vec3 color;
                    scene.cast(pos, rayDir, invDir, color);
                    pixels[pixPointer++] = color.x;
                    pixels[pixPointer++] = color.y;
                    pixels[pixPointer++] = color.z;
                    pixels[pixPointer++] = 255;
                    rayDir += right;
                }
                rayDirTop += down;
            }
            // cout<<" aabb tests: "<<at<<", primative tests: "<<pt<<", bvh aabb: "<<bt<<"\n";
        }

        void renderRows(int start, int end, vector<uint8_t>& pixels, const Scene& scene) {
            int at = 0;
            int pt = 0;
            int bt = 0;
            vec3 rayDirTop = topLeft - pos + down * start;
            float objectDist = 0;
            for (int y = start; y < end; y++) {
                vec3 rayDir = rayDirTop;
                for (int x = 0; x < resX; x++) {
                    vec3 invDir = vec3(1 / rayDir.x, 1 / rayDir.y, 1 / rayDir.z);
                    size_t pixPointer = (y * resX + x) << 2;
                    float closestHitDist = __FLT_MAX__;
                    size_t closestHitObject = 0;
                    scene.bvh.cast(pos, rayDir, invDir, closestHitDist, closestHitObject, at, pt, bt);
                    // for (auto& object : scene.objects) 
                    //     float dist = 0; // aabb dist, use for ray marching later
                    //     if (object->intersect(pos, invDir, dist)) { // hit aabb
                    //         if (object->objectType == 2) { // type box
                    //             objectDist = dist;
                    //         } else {
                    //             objectDist = object->distance(pos, rayDir, invDir);
                    //         }
                            
                    //         if (objectDist != -1 && objectDist < closestHitDist) {
                    //             closestHitDist = objectDist;
                    //             closestHitObject = object->sceneIndex;
                    //         }
                    //     }
                    // }
                    if (closestHitDist != __FLT_MAX__) {
                        pixels[pixPointer++] = scene.objects[closestHitObject]->color.x;
                        pixels[pixPointer++] = scene.objects[closestHitObject]->color.y;
                        pixels[pixPointer++] = scene.objects[closestHitObject]->color.z;
                        pixels[pixPointer++] = 255;
                        // break;
                    } else {
                        pixels[pixPointer++] = 0;
                        pixels[pixPointer++] = 0;
                        pixels[pixPointer++] = 0;
                        pixels[pixPointer++] = 255;
                    }
                    rayDir += right;

                }
                rayDirTop += down;
            }
            // cout<<" aabb tests: "<<at<<", primative tests: "<<pt<<", bvh aabb: "<<bt<<"\n";
        }

        void update() {
            camRot.update();
            right = camRot.apply(vec3(pxSizeX, 0, 0));
            down = camRot.apply(vec3(0, -pxSizeY, 0));
            topLeft = pos + camRot.dir * focalLength - right * resX * 0.5 - down * resY * 0.5;
        }
};



vec3 randPos() {
    return vec3(rand()%1000 - 500, rand()%1000 - 500, rand()%1000 +200);
}
vec3 randSize() {
    return vec3(rand()%500, rand()%500, rand()%500);
}




int main()
{
    

    const unsigned width = 800;
    const unsigned height = 800;

    // vec3 playerPos = vec3(0, 0, 0);
    // int camRes[2] = {width, height};
    // float camSize[2] = {1, 0.625};
    // vec3 lookDir = vec3(0, 0, 1);
    // rotData camRot;
    // float fov = 90;
    // float pitch = 0; // x
    // float yaw = 0; // y
    Camera camera(vec3(0, 0, 0), rotData(), 90, 800, 800, 10, 10);

    // Scene scene;
    // scene.add(make_unique<Sphere>(Sphere(vec3(-20, -20, 100) + vec3(200, -200, 500), 20, randColor())));
    // for (int i = 0; i < 128; i++) {
    //     scene.add(make_unique<Box>(Box(randPos(), vec3(100, 100, 100), randColor())));
    //     scene.add(make_unique<Sphere>(Sphere(randPos(), 20, randColor())));
    // }
    // scene.bvh.build();
    // cout << scene.bvh.str() << "\n";

    // cout<<"building bvh!\n";
    // Scenev2 scenev2;
    // for (int i = 0; i < 128; i++) {
    //     scenev2.addBox(randPos(), vec3(100, 100, 100), randColor());
    //     scenev2.addSphere(randPos(), 20, randColor());
    // }
    // scenev2.buildBVH();
    // cout<<"build done\n";
    // cout<<scenev2.bvhString();

    cout<<"starting!\n";
    Scenev3 scenev3;
    for (int i = 0; i < 32; i++) {
        scenev3.addBox(randPos(), vec3(100, 100, 100), randColor());
        scenev3.addSphere(randPos(), 20, randColor());
    }
    scenev3.buildBVH();
    cout<<scenev3.BVHNodes.size()<<", "<<scenev3.BVHRelations.size()<<"\n";
    cout<<scenev3.BVHString()<<"\n";

    // for (int i = 0; i < scenev3.BVHRelations.size(); i++) {
    //     cout<<"node "<<i<<": "<<scenev3.BVHRelations[i].str()<<"\n";
    // }
    
    
    // scenev2.add(BoxType, Boxv2(randPos(), vec3(50, 50, 50), randColor());


    // BVH bvh(scene.objects);
    // bvh.build();
    // scene.add(make_unique<Box>(Box(vec3(0, -20, 300), vec3(50, 50, 50), randColor())));
    // scene.add(make_unique<Sphere>(Sphere(vec3(70, -20, 290), 20, randColor())));




    // const int threadsNum = std::thread::hardware_concurrency();
    const int threadsNum = 6;
    const int rowsPerThread = camera.resY / threadsNum;


    cout << "cam pos: " << camera.pos << "\n";
    cout << "cam dir: " << camera.camRot.dir << "\n";
    cout << "top left: " << camera.topLeft << "\n";
    cout << "right: " << camera.right << "\n";
    cout << "down: " << camera.down << "\n";
    




    sf::RenderWindow window(sf::VideoMode({width, height}), "Pixel Array");
    vector<uint8_t> pixels(width * height * 4);
    sf::Texture texture;
    bool success = texture.resize({width, height});
    sf::Sprite sprite(texture);



    int frameNum = 0;
    auto start = chrono::high_resolution_clock::now();
    auto frameStart = chrono::high_resolution_clock::now();
    float dt = 0;
    while (window.isOpen())
    {
        
        if (frameNum % 50 == 0) {
            start = chrono::high_resolution_clock::now();
        }
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        float speed = 0.1;
        float turnSpeed = 0.1;
        bool camMoved = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) {
            camera.pos += camera.camRot.apply(vec3(0, 0, speed * dt));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(0, 0, speed * dt));
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
            camera.pos += camera.camRot.apply(vec3(-speed * dt, 0, 0));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(-speed * dt, 0, 0));
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
            camera.pos += camera.camRot.apply(vec3(0, 0, -speed * dt));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(0, 0, -speed * dt));
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
            camera.pos += camera.camRot.apply(vec3(speed * dt, 0, 0));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(speed * dt, 0, 0));
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space)) {
            camera.pos += camera.camRot.apply(vec3(0, speed * dt, 0));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(0, speed * dt, 0));
        } 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift)) {
            camera.pos += camera.camRot.apply(vec3(0, -speed * dt, 0));
            camMoved = true;
            // camera.topLeft += camera.camRot.apply(vec3(0, -speed * dt, 0));
        }
       
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
            camera.camRot.yaw -= turnSpeed * dt;
            camMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
            camera.camRot.yaw += turnSpeed * dt;
            camMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) {
            camera.camRot.pitch -= turnSpeed * dt;
            camMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down)) {
            camera.camRot.pitch += turnSpeed * dt;
            camMoved = true;
        }
        if (camMoved) {
            // camera.camRot.update();
            // cout << camera.pos.str()<<"\n";
            camera.update();
            // cout << camera.pos.str()<<"\n";
        }


        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        int mouseX = min(max(0, mouse.x), camera.resX - 1);
        int mouseY = min(max(0, mouse.y), camera.resY - 1);

        // vector<thread> threads;
        // int rowsPerThread = camera.resY / threadsNum;
        // for (int i = 0; i < threadsNum; i++) {
        //     int start = i * rowsPerThread;
        //     int end = (i + 1) * rowsPerThread;
        //     if (i == threadsNum - 1) {
        //         end = camera.resY;
        //     }
        //     threads.emplace_back(&Camera::renderRows, &camera, start, end, ref(pixels), cref(scene));
        // }
        // for (auto& t : threads) {
        //     t.join();
        // }

        // camera.renderRows(0, camera.resY, pixels, scene);
        // camera.renderRowsv2(0, camera.resY, pixels, scenev2);
        camera.renderRowsv3(0, camera.resY, pixels, scenev3);

        // camera.camRot.yaw += 0.1;
        // camera.camRot.pitch += 0.1;
        // camera.update();
        
        



        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
        if (frameNum % 50 == 49) {
            auto end = chrono::high_resolution_clock::now();
            auto ms = chrono::duration<double, milli>(end - start).count();
            cout << "\r" << round(5000000 / ms) * 0.01 << "fps, mouse: " << mouseX << ", " << mouseY; //<< ", " << camera.cast(mouseX, mouseY, scene) << "                ";
            // cout << camera.camRot.dir.str();
    //         cout << "\ncam pos: " << camera.pos << "\n";
    // cout << "cam dir: " << camera.camRot.dir << "\n";
    // cout << "top left: " << camera.topLeft << "\n";
    // cout << "right: " << camera.right << "\n";
    // cout << "down: " << camera.down << "\n";
        }
        frameNum += 1;
        auto frameEnd = chrono::high_resolution_clock::now();
        dt = chrono::duration<double, milli>(frameEnd - frameStart).count();
        // cout << "\r"<<dt<<"ms/frame";
        frameStart = frameEnd;
    }

    return 0;
}