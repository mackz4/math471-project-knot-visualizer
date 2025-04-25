#include "App.h"
#include "Node.h"

using namespace std;
using namespace glm;
using namespace basicgraphics;

Node::Node(glm::vec3 new_vertex) {

    position = new_vertex;

    setupGeometry();
}

glm::vec3 Node::getNodePosition() {
    return position;
}

void Node::setupGeometry() {
    float latitude = radians(90.0f);
    float longitude = radians(90.0f);

    for (int i = 0; i < NODE_STACKS; i++) {
        Mesh::Vertex vert;
        int vert_count = 0;
        longitude = radians(90.0f);

        float xPos;
        float yPos;
        float zPos;

        std::vector<Mesh::Vertex> cpuVertexArray;
        std::vector<int> cpuIndexArray;
        std::vector<std::shared_ptr<Texture>> textures;

        for (int j = 0; j <= NODE_SLICES; j++) {
            //TEST
            float offset = glm::pi<float>() / 1.0;
            xPos = position.x + NODE_RADIUS * cos(latitude) * cos(longitude);
            yPos = position.y + NODE_RADIUS * sin(latitude);
            zPos = position.z + NODE_RADIUS * cos(latitude) * sin(longitude);

            vert.position = vec3(xPos, yPos, zPos);
            vert.normal = vec3(xPos, yPos, zPos);
            //vert.texCoord0 = glm::vec2(j / (float)SLICES, 1.0 - (sin(latitude) + 1.0) / 2.0);
            vert.texCoord0 = glm::vec2(1.0 - (longitude + offset) / (2.0 * glm::pi<float>()), 1.0 - (latitude + (glm::pi<float>() / 2.0f)) / glm::pi<float>());
            cpuVertexArray.push_back(vert);
            cpuIndexArray.push_back(vert_count++);

            xPos = position.x + NODE_RADIUS * cos(latitude - (glm::pi<float>() / NODE_STACKS)) * cos(longitude);
            yPos = position.y + NODE_RADIUS * sin(latitude - (glm::pi<float>() / NODE_STACKS));
            zPos = position.z + NODE_RADIUS * cos(latitude - (glm::pi<float>() / NODE_STACKS)) * sin(longitude);

            vert.position = vec3(xPos, yPos, zPos);
            vert.normal = vec3(xPos, yPos, zPos);
            //vert.texCoord0 = glm::vec2(j / (float)SLICES, 1.0 - (sin(latitude - glm::pi<float>() / (float)STACKS) + 1.0) / 2.0);
            //std::cout << 2.0 * glm::pi<float>() << std::endl;
            vert.texCoord0 = glm::vec2(1.0 - (longitude + offset) / (2.0 * glm::pi<float>()), 1.0 - (latitude - (glm::pi<float>() / (float)NODE_STACKS) + (glm::pi<float>() / 2.0f)) / glm::pi<float>());
            cpuVertexArray.push_back(vert);
            cpuIndexArray.push_back(vert_count++);

            longitude -= (2.0 * glm::pi<float>()) / (float)NODE_SLICES;
        }
        latitude -= (glm::pi<float>() / (float)NODE_STACKS);

        const int numVertices = cpuVertexArray.size();
        const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
        const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

        meshes[i].reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
    }

}

void Node::draw(GLSLProgram &shader) {
    for (int i = 0; i < 20; i++) {
        meshes[i]->draw(shader);
    }
}