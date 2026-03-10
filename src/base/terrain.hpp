#ifndef TERRAIN_HPP
#define TERRAIN_HPP
#include "mesh.hpp"
#include "model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class Terrain {
    public:
        btCollisionShape* terrainCollisionShape;
        btRigidBody* terrainRigidBody;

        Terrain(Model &terrainModel, btVector3 defaultPos = btVector3(0,0,0));
        void render(Shader &shader, glm::mat4 model = glm::mat4(1.0));
        void initTerrain();
        void addToWorld(btDiscreteDynamicsWorld * world);
    private:
        Model &terrainModel;
        btVector3 position;
        bool initialized;
};

#endif