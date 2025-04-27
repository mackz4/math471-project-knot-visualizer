#ifndef EDGE_H
#define EDGE_H

#include <BasicGraphics.h>
#include <GLSLProgram.h>
#include <glm/glm.hpp>

class Edge {
public:
    Edge(glm::vec3 node_position_prev, glm::vec3 node_position_curr);
    Edge(glm::vec3 node_position_prev, glm::vec3 node_position_curr, glm::vec3 new_color);
    ~Edge() {};

    void setupGeometry();
    void setColor(vec3 new_color);
    void draw(basicgraphics::GLSLProgram& shader);

private:
    const float EDGE_RADIUS = 0.5f;
    const int N_EDGE_SIDES = 40;

    glm::vec3 position_prev;
    glm::vec3 position_curr;
    glm::vec3 color = vec3(0.0);

    std::unique_ptr<basicgraphics::Mesh> mesh;

};
#endif