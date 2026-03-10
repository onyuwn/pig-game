#include "terrain.hpp"

Terrain::Terrain(Model &terrainModel, btVector3 defaultPos) : terrainModel(terrainModel) {
    this->position = defaultPos;
    this->initialized = false;
}

void Terrain::initTerrain() {
    if(!this->terrainModel.isLoaded()) {
        std::cout << "terrain mesh not loaded" << std::endl;
        return;
    } else {
        std::cout << "loading terrain mesh....." << std::endl;
    }
    // create terrain collision mesh
    btTriangleMesh* triMesh = new btTriangleMesh();
    std::vector<Mesh> terrainMeshes = terrainModel.getMeshes();
    for(int h = 0; h < terrainMeshes.size(); h++) {
        Mesh terrainMesh = terrainMeshes[h];
        //std::cout << "ahhhhhh" << std::endl;
        for(int i = 0; i < terrainMesh.indices.size(); i+=3) {
            //std::cout << "buh" << std::endl;
            glm::vec3 vertex1 = terrainMesh.vertices[terrainMesh.indices[i]].Position;
            glm::vec3 vertex2 = terrainMesh.vertices[terrainMesh.indices[i + 1]].Position;
            glm::vec3 vertex3 = terrainMesh.vertices[terrainMesh.indices[i + 2]].Position;

            btVector3 btVert1 = btVector3(vertex1.x, vertex1.y, vertex1.z);
            btVector3 btVert2 = btVector3(vertex2.x, vertex2.y, vertex2.z);
            btVector3 btVert3 = btVector3(vertex3.x, vertex3.y, vertex3.z);

            triMesh->addTriangle(btVert1, btVert2, btVert3);
        }
    }

    btBvhTriangleMeshShape* triangleMesh = new btBvhTriangleMeshShape(triMesh, true);
    this->terrainCollisionShape = triangleMesh;
    // create rigid body
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(this->position);
    btDefaultMotionState* motionstate = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        0, // 0 mass - static
        motionstate,
        this->terrainCollisionShape,
        this->position
    );
    btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
    this->terrainRigidBody = rigidBody;
    this->terrainRigidBody->setFriction(.5);
    this->initialized = true;
}

void Terrain::addToWorld(btDiscreteDynamicsWorld * world) {
    if(initialized) {
        world->addRigidBody(this->terrainRigidBody);
        this->terrainRigidBody->activate(true);
    }
}

void Terrain::render(Shader &shader, glm::mat4 model) {
    shader.use();
    btTransform curTransform = this->terrainRigidBody->getWorldTransform();
    btVector3 curPos = curTransform.getOrigin();
    btQuaternion curRot = curTransform.getRotation();
    model = glm::translate(model, glm::vec3(position.x(), position.y(), position.z()));
    shader.setMat4("model", model);
    this->terrainModel.draw(shader);
}