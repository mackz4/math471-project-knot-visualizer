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

Edge::Edge(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3, glm::vec3 new_color) {

    position_prev = vert1;
    position_curr = vert2;
    color = new_color;

    setupStitchedGeometry(vert0, vert1, vert2, vert3);
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
        x_pos = EDGE_RADIUS * glm::sin(starting_angle);
        z_pos = EDGE_RADIUS * glm::cos(starting_angle);

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

        starting_angle -= 2 * glm::pi<float>() / (N_EDGE_SIDES);
    }

    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
}

void Edge::setupStitchedGeometry(vec3 vert0, vec3 vert1, vec3 vert2, vec3 vert3) {
    float edge_length = length(vert2 - vert1);
    float vert_displacement_y = 0.0f;
    float angle = 0.0f;

    vec3 a = glm::normalize(vert0 - vert1);
    vec3 b = glm::normalize(vert2 - vert1);
    angle = (180.0 - glm::degrees(glm::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b))))) / 2.0;
    vert_displacement_y = EDGE_RADIUS * glm::tan(glm::radians(angle));

    float hyp_len = glm::sqrt(EDGE_RADIUS * EDGE_RADIUS + vert_displacement_y * vert_displacement_y);

    ////
    float vert_displacement_y_2 = 0.0f;
    float angle_2 = 0.0f;

    vec3 a_2 = glm::normalize(vert3 - vert2);
    vec3 b_2 = glm::normalize(vert1 - vert2);
    angle_2 = (180.0 - glm::degrees(glm::acos(glm::dot(a_2, b_2) / (glm::length(a_2) * glm::length(b_2))))) / 2.0;
    vert_displacement_y_2 = EDGE_RADIUS * glm::tan(glm::radians(angle_2));

    float hyp_len_2 = glm::sqrt(EDGE_RADIUS * EDGE_RADIUS + vert_displacement_y_2 * vert_displacement_y_2);


    /////
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;

    const int N_CAN_SIDES = 40;
    float starting_angle = -glm::pi<float>() / 2.0;

    float x_pos;
    float z_pos;

    float vert_height_y = vert_displacement_y;
    float vert_height_y_2 = vert_displacement_y_2;
    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_CAN_SIDES; i++) {
        x_pos = EDGE_RADIUS * glm::sin(starting_angle);
        z_pos = EDGE_RADIUS * glm::cos(starting_angle);

        vert.position = vec3(x_pos, -edge_length / 2.0 - vert_height_y, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 1.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        vert.position = vec3(x_pos, edge_length / 2.0 + vert_height_y_2, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        starting_angle -= 2 * glm::pi<float>() / (N_CAN_SIDES);

        vert_height_y = vert_displacement_y * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_CAN_SIDES + (glm::pi<float>() * 0.5f));
        vert_height_y_2 = vert_displacement_y_2 * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_CAN_SIDES + (glm::pi<float>() * 0.5f));
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

    float rotation2;
    if (new_vec.z <= 0) {
        rotation2 = glm::atan(new_vec.y / new_vec.z) + (glm::pi<float>() / 2.0);
    }
    else {
        rotation2 = glm::pi<float>() + glm::atan(new_vec.y / new_vec.z) + (glm::pi<float>() / 2.0);
    }


    glm::mat4 mat_identity = mat4(1.0);
    glm::mat4 mat_rotZ = glm::rotate(mat_identity, rotation, vec3(0, 0, 1));
    glm::mat4 mat_rotX = glm::rotate(mat_identity, rotation2, vec3(1, 0, 0));
    glm::mat4 mat_from_origin = glm::translate(mat_identity, point_middle);

    //shader.setUniform("model_mat", mat_from_origin * mat_rotZ * mat_rotX);
    shader.setUniform("model_mat", mat_from_origin * mat_rotZ);
    shader.setUniform("materialColor", color);

    mesh->draw(shader);
}