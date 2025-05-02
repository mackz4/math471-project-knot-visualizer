#ifndef NODE_H
#define NODE_H

#include <BasicGraphics.h>
#include "Color.h"
#include <GLSLProgram.h>
#include <glm/glm.hpp>

class Node {
public:
    Node(glm::vec3 position);
    Node(glm::vec3 position, glm::vec3 new_color);
    Node(glm::vec3 position, float new_offset, glm::vec3 new_color);
    ~Node(){};

    glm::vec3 getNodePosition();
    void setupGeometry();
    void setColor(vec3 new_color);
    vec3 getColor();
    void draw(basicgraphics::GLSLProgram &shader);


private:
    const float NODE_RADIUS = 1.5f; //0.6
    const int NODE_STACKS = 20;
    const int NODE_SLICES = 40;

    glm::vec3 position;
    float offset = 0.0f;
    glm::vec3 color = COLOR_DARK_YELLOW;

    std::unique_ptr<basicgraphics::Mesh> meshes[20];

};
#endif