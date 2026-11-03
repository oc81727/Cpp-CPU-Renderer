# C++ CPU Renderer

## V1
<img width="398" height="250" alt="image" src="https://github.com/user-attachments/assets/107786ad-c39d-4711-bac5-9fec44a7d80d" />

Uses Sphere marching to render objects.


### Optimisations:

BVH Tree, Multi-threaded rendering

too slow...

## V2
<img width="399" height="398" alt="image" src="https://github.com/user-attachments/assets/fab75ecf-7bc1-437d-86e8-f38fa43e6fce" />

Abandoned sphere marching, uses normal raycasting instead.

Added camera controls. WASD/space/shift to move, arrow keys to rotate camera.

V2 contains code for 3 successive iterations of the renderer.

### V2.1
Object Oriented design. Spheres and Boxes inherit from a base class, and are stored in the same vector of the BVH leaf nodes. Each object contained its own data about position, size, color, etc.

Pointer-based BVH traversal.

### V2.2
Boxes and Spheres now stored separately. A new vector `vector<PrimativeRef> primatives` now stores references to each object. Each `PrimativeRef` contains object type, and array index, so objects can be identified by its reference.

BVH tree is now a single contiguous vector, and traversal uses array indexing instead of pointers.

BVH tree construction is now iterative instead of recursive.

Final primative intersection check uses the object type of `PrimativeRef` instead of virtual functions.

### V2.3
Most Data-Oriented design. Object boundaries (min/max pos) and object data (type, color) are now stored separately. All object types are stored in the same vector again. Each object's data is accessed by indexing its corresponding position in the object data vector.
Additional data, eg sphere radius, center are no longer stored, but recalculated when needed.

BVH data are now stored separately, into `BVHNodes` (info about boundaries of each node) and `BVHRelations` (start and end indicies of objects, and node children).

BVH construction now sorts both object and object data vectors to ensure the 2 vectors stay aligned, and allows for using indicies to access primative objects in the BVH.

Also, I just realised that it's spelt "primitive", not "primative".

The overall transition from an Object-Oriented design to a Data-Oriented design with Structures-of-Arrays instead of Arrays-of-Structures simplified memory organisation, reduced overhead, improved cache locality and led to an overall improvement of framerate, from ~10fps to ~30fps. 


