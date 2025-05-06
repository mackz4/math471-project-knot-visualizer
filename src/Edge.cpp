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

Edge::Edge(glm::vec3 node_position_prev, glm::vec3 node_position_curr, float new_offset, glm::vec3 new_color) {

    position_prev = node_position_prev;
    position_curr = node_position_curr;
    offset = new_offset;
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

    float edge_length = length(position_prev - position_curr);
    //float edge_length = glm::sqrt(glm::pow(position_prev.x - position_curr.x, 2) + glm::pow(position_prev.y - position_curr.y, 2));

    //float starting_angle = -glm::pi<float>() / 2.0f;
    float starting_angle = 0.0f;

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

    //std::cout << "---------------------------" << std::endl;
    //std::cout << "vert0: " << vert0.x << " " << vert0.y << " " << vert0.z << std::endl;
    //std::cout << "vert1: " << vert1.x << " " << vert1.y << " " << vert1.z << std::endl;
    //std::cout << "vert2: " << vert2.x << " " << vert2.y << " " << vert2.z << std::endl;
    //std::cout << "vert3: " << vert3.x << " " << vert3.y << " " << vert3.z << std::endl;

    // ******** XY-STITCH: SIDE 1 ********
    float displacement_xy_1 = 0.0f;
    float angle_xy_1 = 0.0f;

    float x1_xy_1 = vert0.x - vert1.x;
    float y1_xy_1 = vert0.y - vert1.y;
    float x2_xy_1 = vert2.x - vert1.x;
    float y2_xy_1 = vert2.y - vert1.y;

    float dot_xy_1 = x1_xy_1 * x2_xy_1 + y1_xy_1 * y2_xy_1;
    float det_xy_1 = x1_xy_1 * y2_xy_1 - y1_xy_1 * x2_xy_1;
    angle_xy_1 = (glm::pi<float>() - glm::atan(det_xy_1, dot_xy_1)) / 2.0f;
    displacement_xy_1 = EDGE_RADIUS * glm::tan(angle_xy_1);

    // ******** XY-STITCH: SIDE 2 ********
    float displacement_xy_2 = 0.0f;
    float angle_xy_2 = 0.0f;

    vec2 a_xy_2 = glm::normalize(vec2(vert3.x - vert2.x, vert3.y - vert2.y));
    vec2 b_xy_2 = glm::normalize(vec2(vert1.x - vert2.x, vert3.y - vert2.y));

    float x1_xy_2 = vert3.x - vert2.x;
    float y1_xy_2 = vert3.y - vert2.y;
    float x2_xy_2 = vert1.x - vert2.x;
    float y2_xy_2 = vert1.y - vert2.y;

    float dot_xy_2 = x1_xy_2 * x2_xy_2 + y1_xy_2 * y2_xy_2;
    float det_xy_2 = x1_xy_2 * y2_xy_2 - y1_xy_2 * x2_xy_2;
    angle_xy_2 = (glm::pi<float>() - glm::atan(det_xy_2, dot_xy_2)) / 2.0f;
    displacement_xy_2 = EDGE_RADIUS * glm::tan(angle_xy_2);

    // ******** Z-STITCH: SIDE 1 ********
    float displacement_z_1 = 0.0f;
    float angle_z_1 = 0.0f;

    vec2 a_z_1 = glm::normalize(vec2(vert0.x - vert1.x, vert0.y - vert1.y));
    vec2 b_z_1 = glm::normalize(vec2(vert2.y - vert1.y, vert2.y - vert1.y));

    float x1_z_1 = vert0.x - vert1.x;
    float y1_z_1 = vert0.y - vert1.y;
    float z1_z_1 = vert0.z - vert1.z;
    float x2_z_1 = vert2.x - vert1.x;
    float y2_z_1 = vert2.y - vert1.y;
    float z2_z_1 = vert2.z - vert1.z;

    float xy1_z_1 = x1_z_1 + y1_z_1;
    float xy2_z_1 = x2_z_1 + y2_z_1;

    float dot_z_1 = xy1_z_1 * xy2_z_1 + z1_z_1 * z2_z_1;
    float det_z_1 = xy1_z_1 * z2_z_1 - z1_z_1 * xy2_z_1;
    angle_z_1 = (glm::pi<float>() - glm::atan(det_z_1, dot_z_1)) / 2.0f;
    if (angle_z_1 == glm::pi<float>() / 2.0f) {
        displacement_z_1 = 0.0f;
    }
    else {
        displacement_z_1 = EDGE_RADIUS * glm::tan(angle_z_1);
    }

    // ******** Z-STITCH: SIDE 2 ********
    float displacement_z_2 = 0.0f;
    float angle_z_2 = 0.0f;

    float x1_z_2 = vert3.x - vert2.x;
    float y1_z_2 = vert3.y - vert2.y;
    float z1_z_2 = vert3.z - vert2.z;
    float x2_z_2 = vert1.x - vert2.x;
    float y2_z_2 = vert1.y - vert2.y;
    float z2_z_2 = vert1.z - vert2.z;

    float xy1_z_2 = x1_z_2 + y1_z_2;
    float xy2_z_2 = x2_z_2 + y2_z_2;

    float dot_z_2 = xy1_z_2 * xy2_z_2 + z1_z_2 * z2_z_2;
    float det_z_2 = xy1_z_2 * z2_z_2 - z1_z_2 * xy2_z_2;

    angle_z_2 = (glm::pi<float>() - glm::atan(det_z_2, dot_z_2)) / 2.0f;
    if (angle_z_2 == glm::pi<float>() / 2.0f) {
        displacement_z_2 = 0.0f;
    }
    else {
        displacement_z_2 = EDGE_RADIUS * glm::tan(angle_z_2);
    }

    //std::cout << "angle_z_1: " << degrees(angle_z_1) << std::endl;
    //std::cout << "displacement_z_1: " << displacement_z_1 << std::endl;
    //std::cout << "angle_z_2: " << degrees(angle_z_2) << std::endl;
    //std::cout << "displacement_z_2: " << displacement_z_2 << std::endl;
    //std::cout << " " << std::endl;

    // MESH
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;

    float starting_angle = 0.0f;
    float x_pos;
    float z_pos;
    float height_xy_1 = displacement_xy_1;
    float height_xy_2 = displacement_xy_2;
    float height_z_1 = displacement_z_1;
    float height_z_2 = displacement_z_2;
    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_EDGE_SIDES; i++) {
        x_pos = EDGE_RADIUS * glm::sin(starting_angle);
        z_pos = EDGE_RADIUS * glm::cos(starting_angle);

        vert.position = vec3(x_pos, -edge_length / 2.0 - height_xy_1 - height_z_1, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_EDGE_SIDES), 1.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        vert.position = vec3(x_pos, edge_length / 2.0 - height_xy_2 - height_z_2, z_pos);
        vert.normal = vec3(glm::sin(starting_angle), 0, glm::cos(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_EDGE_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        starting_angle -= 2 * glm::pi<float>() / (N_EDGE_SIDES);

        height_xy_1 = displacement_xy_1 * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_EDGE_SIDES + (glm::pi<float>() * 0.5f));
        height_xy_2 = displacement_xy_2 * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_EDGE_SIDES + (glm::pi<float>() * 0.5f));
        height_z_1 = displacement_z_1 * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_EDGE_SIDES + (glm::pi<float>() * 1.0f));
        height_z_2 = displacement_z_2 * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_EDGE_SIDES + (glm::pi<float>() * 1.0f));
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

int Edge::getArcLabel() {
    return arc_label;
}

void Edge::setColor(vec3 new_color) {
    color = new_color;
}

void Edge::setArcLabel(int new_arc_label) {
    arc_label = new_arc_label;
}

void Edge::draw(GLSLProgram& shader) {
    vec3 midpoint = vec3((position_curr.x + position_prev.x) / 2.0f, (position_curr.y + position_prev.y) / 2.0f, (position_curr.z + position_prev.z) / 2.0f + offset);

    vec3 y = normalize(vec3(position_curr - position_prev));;
    vec3 z = normalize(cross(vec3(0, 0, 1), y));
    vec3 x = cross(y, z);
    //vec3 x = normalize(vec3(position_curr - position_prev));;
    //vec3 y = normalize(cross(vec3(0, 0, 1), x));
    //vec3 z = cross(x, y);

    glm::mat4 matrix;
    matrix[0] = vec4(vec3(x), 0.0f);
    matrix[1] = vec4(vec3(y), 0.0f);
    matrix[2] = vec4(vec3(z), 0.0f);
    //matrix[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    matrix[3] = vec4(midpoint, 1.0f);

    shader.setUniform("model_mat", matrix);
    //shader.setUniform("model_mat", mat_from_origin * glm::inverse(glm::lookAt(vec3(0.0, 0.0, 0.0), dir, vec3(0.0, 1.0, 0.0))));
    shader.setUniform("materialColor", color);

    mesh->draw(shader);
}