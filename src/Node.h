#ifndef NODE_H
#define NODE_H

#include <BasicGraphics.h>
#include <GLSLProgram.h>
#include <glm/glm.hpp>

class Node {
public:
    Node(glm::vec3 position);
    Node(glm::vec3 position, glm::vec3 new_color);
    ~Node(){};

    glm::vec3 getNodePosition();
    void setupGeometry();
    void setColor(vec3 new_color);
    void draw(basicgraphics::GLSLProgram &shader);


private:
    const float NODE_RADIUS = 1.0f;
    const float NODE_OFFSET = 3.0f;
    const int NODE_STACKS = 20;
    const int NODE_SLICES = 40;

    glm::vec3 position;
    glm::vec3 color = vec3(0.0);

    std::unique_ptr<basicgraphics::Mesh> meshes[20];

};
#endif