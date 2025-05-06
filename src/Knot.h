#ifndef KNOT_H
#define KNOT_H

#include <BasicGraphics.h>
#include <GLSLProgram.h>
#include <glm/glm.hpp>
#include "Color.h"

class Knot {
public:
    Knot(vec3 new_color);
    ~Knot() {};

    void incNodeCount();
    void incEdgeCount();
    void incCrossingCount();
    int getNodeCount();
    int getEdgeCount();
    int getCrossingCount();
    void addNode(std::shared_ptr<Node> node);
    void addEdge(std::shared_ptr<Edge> edge);
    void addStitchedEdge(std::shared_ptr<Edge> stitched_edge);
    void setNodeGuide(std::shared_ptr<Node> new_node_guide);
    void setEdgeGuide(std::shared_ptr<Edge> new_edge_guide);
    void setComponentMix(float new_component_mix);
    std::vector<std::shared_ptr<Node>> getNodes();
    std::vector<std::shared_ptr<Edge>> getEdges();
    std::vector<std::shared_ptr<Edge>> getStitchedEdges();
    std::shared_ptr<Node> getNodeGuide();
    std::shared_ptr<Edge> getEdgeGuide();
    void draw(basicgraphics::GLSLProgram& shader);
    void drawStitched(basicgraphics::GLSLProgram& shader);
    bool isPColorable(int p);
    void eraseEdge(std::shared_ptr<Edge> edge_to_erase);
    void insertNode(int index, std::shared_ptr<Node> node_crossed);
    void insertEdge(int index, std::shared_ptr<Edge> edge_crossed_1, std::shared_ptr<Edge> edge_crossed_2);

private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    std::vector<std::shared_ptr<Edge>> stitched_edges;
    std::shared_ptr<Node> node_guide;
    std::shared_ptr<Edge> edge_guide;

    float component_mix = 0.4f;
    vec3 color = COLOR_WHITE;

    int node_count = 0;
    int edge_count = 0;
    int crossing_count = 0;

    std::vector<std::vector<int>> p_colorability_matrix = {{2, 2}, {3, 4}};
};
#endif