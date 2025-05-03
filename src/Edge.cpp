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

    float starting_angle = -glm::pi<float>() / 2.0;
    starting_angle = glm::pi<float>() / 2.0f;

    float x_pos;
    float z_pos;

    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_EDGE_SIDES; i++) {
        x_pos = EDGE_RADIUS * glm::sin(starting_angle);
        z_pos = offset + EDGE_RADIUS * glm::cos(starting_angle);

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



    // TEMP
    float x1 = vert0.x - vert1.x;
    float y1 = vert0.y - vert1.y;
    float x2 = vert2.x - vert1.x;
    float y2 = vert2.y - vert1.y;

    float dot = x1 * x2 + y1 * y2;
    float det = x1 * y2 - y1 * x2;
    angle = (180.0 - glm::degrees(glm::atan(det, dot))) / 2.0f;
    vert_displacement_y = EDGE_RADIUS * glm::tan(glm::radians(angle));
    std::cout << "T-angle: " << angle << std::endl;
    // ENDTEMP

    float hyp_len = glm::sqrt(EDGE_RADIUS * EDGE_RADIUS + vert_displacement_y * vert_displacement_y);

    ////
    float vert_displacement_y_2 = 0.0f;
    float angle_2 = 0.0f;

    vec3 a_2 = glm::normalize(vert3 - vert2);
    vec3 b_2 = glm::normalize(vert1 - vert2);
    angle_2 = (180.0 - glm::degrees(glm::acos(glm::dot(a_2, b_2) / (glm::length(a_2) * glm::length(b_2))))) / 2.0;
    vert_displacement_y_2 = EDGE_RADIUS * glm::tan(glm::radians(angle_2));


    // TEMP
    float x1_2 = vert3.x - vert2.x;
    float y1_2 = vert3.y - vert2.y;
    float x2_2 = vert1.x - vert2.x;
    float y2_2 = vert1.y - vert2.y;

    float dot_2 = x1_2 * x2_2 + y1_2 * y2_2;
    float det_2 = x1_2 * y2_2 - y1_2 * x2_2;
    angle_2 = (180.0 - glm::degrees(glm::atan(det_2, dot_2))) / 2.0f;
    vert_displacement_y_2 = EDGE_RADIUS * glm::tan(glm::radians(angle_2));
    //std::cout << "T-angle: " << angle << std::endl;
    // ENDTEMP

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

        vert.position = vec3(x_pos, edge_length / 2.0 - vert_height_y_2, z_pos);
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
// https://gamedev.stackexchange.com/questions/118960/convert-a-direction-vector-normalized-to-rotation
    vec3 dir = normalize(vec3(position_curr - position_prev));

    vec3 point_middle = (position_curr + position_prev) / 2.0f;

    glm::mat4 mat_identity = mat4(1.0);
    glm::mat4 mat_from_origin = glm::translate(mat_identity, point_middle);

    float angle = std::atan2(-dir.z, dir.x);

    // Make the rotation matrix around the vertical (z) axis, adjusts the 'yaw'
    glm::mat4 glmrotXY = glm::rotate(mat4(1.0), angle, vec3(0.0, 1.0, 0.0));

    // Find the angle with the xy with plane (0, 0, 1); the - there is because we want to 
    // 'compensate' for that angle (a 'counter-angle')
    float angleZ = -std::asin(dir.y);

    // Make the matrix for that, assuming that Y is your 'side' vector; makes the model 'pitch'
    glm::mat4 glmrotZ = glm::rotate(mat4(1.0), angleZ + (glm::pi<float>() / 2.0f), vec3(0.0, 0.0, -1.0));

    shader.setUniform("model_mat", glm::lookAt(vec3(0.0, 0.0, 0.0), dir, vec3(0.0, 1.0, 0.0)));
    shader.setUniform("model_mat", mat_from_origin * glmrotXY * glmrotZ);
    shader.setUniform("materialColor", color);

    mesh->draw(shader);
}