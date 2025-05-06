#include "App.h"
#include "Knot.h"

using namespace std;
using namespace glm;
using namespace basicgraphics;

Knot::Knot(vec3 new_color) {
    color = new_color;
}

void Knot::addNode(std::shared_ptr<Node> node) {
    nodes.push_back(node);
}

void Knot::addEdge(std::shared_ptr<Edge> edge) {
    edges.push_back(edge);
}

void Knot::addStitchedEdge(std::shared_ptr<Edge> stitched_edge) {
    stitched_edges.push_back(stitched_edge);
}

void Knot::setNodeGuide(std::shared_ptr<Node> new_node_guide) {
    node_guide = new_node_guide;
}

void Knot::setEdgeGuide(std::shared_ptr<Edge> new_edge_guide) {
    edge_guide = new_edge_guide;
}

void Knot::incNodeCount() {
    node_count++;
}

void Knot::incEdgeCount() {
    edge_count++;
}

void Knot::incCrossingCount() {
    crossing_count++;
}

int Knot::getNodeCount() {
    return node_count;
}

int Knot::getEdgeCount() {
    return edge_count;
}

int Knot::getCrossingCount() {
    return crossing_count;
}

std::vector<std::shared_ptr<Node>> Knot::getNodes() {
    return nodes;
}

std::vector<std::shared_ptr<Edge>> Knot::getEdges() {
    return edges;
}

std::vector<std::shared_ptr<Edge>> Knot::getStitchedEdges() {
    return stitched_edges;
}

std::shared_ptr<Node> Knot::getNodeGuide() {
    return node_guide;
}

std::shared_ptr<Edge> Knot::getEdgeGuide() {
    return edge_guide;
}

void Knot::eraseEdge(std::shared_ptr<Edge> edge_to_erase) {
    edges.erase(find(edges.begin(), edges.end(), edge_to_erase));


}
void Knot::insertNode(int index, std::shared_ptr<Node> node_crossed) {
    nodes.insert(nodes.begin() + index, node_crossed);
}

void Knot::insertEdge(int index, std::shared_ptr<Edge> edge_crossed_1, std::shared_ptr<Edge> edge_crossed_2) {
    edges.insert(edges.begin() + index, { edge_crossed_1, edge_crossed_2 });
}
void Knot::setComponentMix(float new_component_mix) {
    component_mix = new_component_mix;
}

void Knot::draw(GLSLProgram& shader) {
    shader.setUniform("light_direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.setUniform("component_mix", component_mix);

    // Nodes draw
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes.at(i)->draw(shader);
    }

    // Edges draw
    for (size_t i = 0; i < edges.size(); i++) {
        edges.at(i)->draw(shader);
    }

    // Node guide draw
    if (node_guide != NULL) {
        node_guide->draw(shader);
    }

    // Edge guide draw
    if (edge_guide != NULL) {
        edge_guide->draw(shader);
    }
}

void Knot::drawStitched(basicgraphics::GLSLProgram& shader) {
    shader.setUniform("light_direction", glm::vec3(0.0f, -1.0f, 0.0f));
    shader.setUniform("component_mix", component_mix);

    // Stitched edges draw
    for (size_t i = 0; i < stitched_edges.size(); i++) {
        stitched_edges.at(i)->draw(shader);
    }
}

bool Knot::isPColorable(int p) {
    for (int i = 0; i < p_colorability_matrix.size(); i++)
    {
        for (int j = 0; j < p_colorability_matrix[i].size(); j++)
        {
            std::cout << p_colorability_matrix[i][j];
        }
        std::cout << std::endl;
    }


    return false;
}