#include "rigidbodyentity.hpp"

RigidBodyEntity::RigidBodyEntity(Model& entityModel, btVector3 defaultPos, CollisionShapeType collisionShapeType, float mass, btVector3 boxShape) : entityModel(entityModel), collisionShapeType(collisionShapeType) {
    // build collision shape (box for rn)
    this->defaultPos = defaultPos;
    this->mass = mass;
    this->initialized = false;
    this->boundingBox = boxShape;
    std::cout << "init rigid body" << std::endl;
    // create rigid body
} 

// RigidBodyEntity::~RigidBodyEntity() {
//     if (this->entityRigidBody) {
//         delete this->entityRigidBody->getMotionState();
//         delete this->entityRigidBody;
//     }
//     if (this->entityCollisionShape) {
//         delete this->entityCollisionShape;
//     }
// }

void RigidBodyEntity::initialize(glm::mat4 model) {
    glm::vec3 meshOrigin(0.0f, 0.0f, 0.0f);
    if(this->collisionShapeType == BOX) {
        glm::vec3 minVertex(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 maxVertex(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        std::vector<Mesh> entityMeshes = this->entityModel.getMeshes();  // Get mesh data
        // Iterate through all vertices in the mesh to calculate the bounding box
        for (int h = 0; h < entityMeshes.size(); h++) {
            Mesh terrainMesh = entityMeshes[h];
            
            for (int i = 0; i < terrainMesh.vertices.size(); i++) {
                glm::vec3 vertex = terrainMesh.vertices[i].Position;
                // Update the bounding box limits
                minVertex = glm::min(minVertex, vertex);
                maxVertex = glm::max(maxVertex, vertex);
            }
        }

        meshOrigin = (minVertex + maxVertex) * 0.5f;
        printf("meshorigin: %f, %f, %f\n", meshOrigin.x, meshOrigin.y, meshOrigin.z);
        // Calculate the size (width, height, depth) of the bounding box
        glm::vec3 size = maxVertex - minVertex;

        // Create the btBoxShape using the half-extents
        btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
        btBoxShape* boxShape = new btBoxShape(halfExtents);
        this->entityCollisionShape = boxShape;
    } else if(this->collisionShapeType == MESH) {
        if(!this->entityModel.isLoaded()) {
            std::cout << "entity model mesh not loaded" << std::endl;
            return;
        }

        btConvexHullShape* convexHullShape = new btConvexHullShape();
        std::vector<Mesh> entityMeshes = this->entityModel.getMeshes();
        for(int h = 0; h < entityMeshes.size(); h++) {
            Mesh terrainMesh = entityMeshes[h];
            for(int i = 0; i < terrainMesh.indices.size(); i+=3) {
                glm::vec3 vertex = terrainMesh.vertices[i].Position;
                glm::vec4 transformedVertex = model * glm::vec4(vertex, 1.0f);  // Apply transformation
                // Convert the vertex to btVector3 for Bullet
                btVector3 btVertex(transformedVertex.x, transformedVertex.y, transformedVertex.z);

                // Add the vertex to the convex hull shape
                convexHullShape->addPoint(btVertex);
            }
        }

        this->entityCollisionShape = convexHullShape;
    }

    glm::quat rotationQuat = glm::quat_cast(model);
    btQuaternion btRot(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);


    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(defaultPos);
    startTransform.setRotation(btRot);

    btDefaultMotionState* motionstate = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        mass,
        motionstate,
        this->entityCollisionShape,
        btVector3(0,0,0)
    );
    this->entityRigidBody = new btRigidBody(rigidBodyCI);
    this->entityRigidBody->activate();
    this->initialized = true;
}

void RigidBodyEntity::addToWorld(btDynamicsWorld * world) {
    if(initialized) {
        world->addRigidBody(this->entityRigidBody);
        this->entityRigidBody->setActivationState(1);
        this->entityRigidBody->activate(true);
    }
}

glm::vec3 RigidBodyEntity::getPos() {
    btTransform curTransform = this->entityRigidBody->getWorldTransform();
    btVector3 curPos = curTransform.getOrigin();
    return glm::vec3(curPos.x(), curPos.y(), curPos.z());
}

void RigidBodyEntity::render(Shader &shader, glm::mat4 model, bool positionOverride) {
    shader.use();
    if(positionOverride) {
        this->entityRigidBody->activate(false);
    } else {
        btTransform curTransform = this->entityRigidBody->getWorldTransform();
        btVector3 curPos = curTransform.getOrigin();
        btQuaternion curRot = curTransform.getRotation();
        model = glm::translate(model, glm::vec3(curPos.x(), curPos.y(), curPos.z()));
        glm::quat glmRot(curRot.w(), curRot.x(), curRot.y(), curRot.z());
        // Convert glm::quat to glm::mat4 (rotation matrix)
        glm::mat4 rotationMatrix = glm::toMat4(glmRot);
        model = model * rotationMatrix;
        finalModelMatrix = model;
    }
    shader.setMat4("model", model);
    this->entityModel.draw(shader);
}

glm::mat4 RigidBodyEntity::getFinalModelMatrix() {
    return finalModelMatrix;
}

void RigidBodyEntity::activateInteraction() {
    std::cout << "HITTTT";
}

void RigidBodyEntity::setPos(glm::vec3 newPos) {
    this->defaultPos = btVector3(newPos.x, newPos.y, newPos.z);
    btTransform currentTransform = this->entityRigidBody->getWorldTransform();
    currentTransform.setOrigin(this->defaultPos);
    this->entityRigidBody->setWorldTransform(currentTransform);
}