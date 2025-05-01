#include "App.h"
#include "Edge.h"

using namespace std;
using namespace glm;
using namespace basicgraphics;

Edge::Edge(glm::vec3 node_position_prev, glm::vec3 node_position_curr) {

    position_prev = node_position_prev;
    position_curr = node_position_curr;

    setupGeometry();
}

Edge::Edge(glm::vec3 node_position_prev, glm::vec3 node_position_curr, glm::vec3 new_color) {

    position_prev = node_position_prev;
    position_curr = node_position_curr;
    color = new_color;

    setupGeometry();
}

void Edge::setupGeometry() {
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;

    float edge_length = glm::sqrt(glm::pow(position_prev.x - position_curr.x, 2) + glm::pow(position_prev.y - position_curr.y, 2));

    float starting_angle = -glm::pi<float>() / 2.0;

    float x_pos;
    float z_pos;

    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_EDGE_SIDES; i++) {
        z_pos = EDGE_RADIUS * glm::cos(starting_angle);
        x_pos = EDGE_RADIUS * glm::sin(starting_angle);

        vert.position = vec3(x_pos, -edge_length / 2.0, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_EDGE_SIDES), 1.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        vert.position = vec3(x_pos, edge_length / 2.0, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_EDGE_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        starting_angle += 2 * glm::pi<float>() / (N_EDGE_SIDES);
    }

    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
}

vec3 Edge::getPositionPrev() {
    return position_prev;
}

vec3 Edge::getPositionCurr() {
    return position_curr;
}

void Edge::setColor(vec3 new_color) {
    color = new_color;
}

void Edge::draw(GLSLProgram& shader) {
    vec3 new_vec = normalize(vec3(position_curr - position_prev));

    vec3 point_middle = (position_curr + position_prev) / 2.0f;

    float rotation;
    if (new_vec.x <= 0) {
        rotation = glm::atan(new_vec.y / new_vec.x) + (glm::pi<float>() / 2.0);
    }
    else {
        rotation = glm::pi<float>() + glm::atan(new_vec.y / new_vec.x) + (glm::pi<float>() / 2.0);
    }
    glm::mat4 mat_identity = mat4(1.0);
    glm::mat4 mat_rotZ = glm::rotate(mat_identity, rotation, vec3(0, 0, 1));
    glm::mat4 mat_from_origin = glm::translate(mat_identity, point_middle);

    shader.setUniform("model_mat", mat_from_origin * mat_rotZ);
    shader.setUniform("materialColor", color);

    mesh->draw(shader);
}