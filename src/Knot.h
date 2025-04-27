#ifndef KNOT_H
#define KNOT_H

#include <BasicGraphics.h>
#include <GLSLProgram.h>
#include <glm/glm.hpp>

class Knot {
public:
    Knot(vec3 new_color);
    ~Knot() {};

    void incNodeCount();
    int getNodeCount();
    void incEdgeCount();
    int getEdgeCount();
    void addNode(std::shared_ptr<Node> node);
    void addEdge(std::shared_ptr<Edge> edge);
    void setNodeGuide(std::shared_ptr<Node> new_node_guide);
    void setEdgeGuide(std::shared_ptr<Edge> new_edge_guide);
    void setComponentMix(float new_component_mix);
    std::vector<std::shared_ptr<Node>> getNodes();
    std::vector<std::shared_ptr<Edge>> getEdges();
    std::shared_ptr<Node> getNodeGuide();
    std::shared_ptr<Edge> getEdgeGuide();
    void draw(basicgraphics::GLSLProgram& shader);

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    std::shared_ptr<Node> node_guide;
    std::shared_ptr<Edge> edge_guide;

    float component_mix = 0.4f;
    vec3 color = vec3(0.0f);

    int node_count = 0;
    int edge_count = 0;

};
#endif