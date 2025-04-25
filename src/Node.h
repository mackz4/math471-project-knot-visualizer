#ifndef NODE_H
#define NODE_H

#include <BasicGraphics.h>
#include <GLSLProgram.h>
#include <glm/glm.hpp>

#define SIZE_X 51
#define SIZE_Y 51

class Node {
public:
    Node(glm::vec3 position);
    ~Node(){};

    glm::vec3 getNodePosition();
    void setupGeometry();
    void draw(basicgraphics::GLSLProgram &shader);


private:
    const float NODE_RADIUS = 1.0f;
    const int NODE_STACKS = 20;
    const int NODE_SLICES = 40;

    glm::vec3 position;

    std::unique_ptr<basicgraphics::Mesh> meshes[20];

};
#endif