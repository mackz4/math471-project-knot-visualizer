#include "App.h"
#include "Node.h"
#include "Knot.h"
#include "Edge.h"
#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

#include <config/VRDataIndex.h>

App::App(int argc, char** argv) : VRApp(argc, argv)
{
	_lastTime = 0.0;
	_curFrameTime = 0.0;

	currTheta = glm::radians(90.0);
	currPhi = glm::radians(90.0);

    eye_world = angleToSpherePoint(currTheta, currPhi);
    vec3 look_vector = -normalize(eye_world);
    if (glm::degrees(currPhi) == 0.0) {
        up_vector = vec3(-cos(currTheta), 0.0, -sin(currTheta));
    }
    else if (glm::degrees(currPhi) == 180.0) {
        up_vector = vec3(cos(currTheta), 0.0, sin(currTheta));
    } else {
        vec3 right_vector = normalize(cross(look_vector, vec3(0.0, 1.0, 0.0)));
        up_vector = normalize(cross(right_vector, look_vector));
    }

	mouseDown = false;
    mouseRightDown = false;

    //Test Knot init
    std::shared_ptr<Knot> knot;
    knot.reset(new Knot(knot_color_index[knot_count % 3]));
    knots.push_back(knot);
    knot_curr = knots.at(knot_count);
    //Test Knot init end
}

App::~App()
{
	glfonsDelete(fs);
	shutdown();
}

void App::onAnalogChange(const VRAnalogEvent &event) {
    // This routine is called for all Analog_Change events.  Check event->getName()
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue(). 

	if (event.getName() == "FrameStart") {
		_lastTime = _curFrameTime;
		_curFrameTime = event.getValue();
	}
    if (event.getName() == "MouseWheel_Spin") {
        if (event.getValue() > 0) {

        }
        else if (event.getValue() < 0) {

        }
    }


}

void App::onButtonDown(const VRButtonEvent& event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.
    //You can respond to individual events like this:
    /*
    if (event.getName() == _paintOnEvent) {
        _painting = true;
    }
    else if (event.getName() == _grabOnEvent) {
        _grabbing = true;
    }
    */
    string name = event.getName();
    if (name == "MouseBtnLeft_Down") {


        vec3 node_position_curr = vec3((mousePosX - 512.0) / 20.48, -(mousePosY - 512.0) / 20.48, 0.0);
        if (canNodeBePlaced == true) {
            if (node_position_curr.x >= -25.0 && node_position_curr.x <= 25.0 && node_position_curr.y >= -25.0 && node_position_curr.y <= 25.0) { // within screen range      

                int new_knot_count = 0;
                for (int k = intersection_nodes.size() - 1; k >= 0; k--) {
                    std::cout << "node" << std::endl;
                    vec2 direction_vector = normalize(vec2(knot_curr->getEdgeGuide()->getPositionCurr().x - knot_curr->getEdgeGuide()->getPositionPrev().x, knot_curr->getEdgeGuide()->getPositionCurr().y - knot_curr->getEdgeGuide()->getPositionPrev().y));

                    vec2 back_node_vector;
                    if (k == intersection_nodes.size() - 1) {
                        back_node_vector = vec2(knot_curr->getEdgeGuide()->getPositionPrev().x - intersection_nodes.at(k)->getNodePosition().x, knot_curr->getEdgeGuide()->getPositionPrev().y - intersection_nodes.at(k)->getNodePosition().y);
                    }
                    else {
                        back_node_vector = vec2(intersection_nodes.at(k)->getNodePosition().x - intersection_nodes.at(k + 1)->getNodePosition().x, intersection_nodes.at(k)->getNodePosition().y - intersection_nodes.at(k + 1)->getNodePosition().y);
                    }
                    if (length(back_node_vector) > KNOT_SPACING_DIST_MIN * 2.0f) {
                        std::shared_ptr<Node> back_node;
                        back_node.reset(new Node(intersection_nodes.at(k)->getNodePosition() - (vec3(direction_vector.x, direction_vector.y, 0.0f) * KNOT_SPACING_DIST_MIN), KNOT_SPACING_DIST_MIN + 2.0f, knot_color_index_2[knot_count % 3])); //FIRST NODE
                        knot_curr->addNode(back_node);
                        knot_curr->incNodeCount();
                        node_count_total++;
                        new_knot_count++;

                        std::shared_ptr<Edge> edge1;
                        edge1.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 2)->getNodePosition(), knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition(), knot_color_index[knot_count % 3]));
                        edge1->setArcLabel(knot_curr->getCrossingCount());
                        knot_curr->addEdge(edge1);
                        knot_curr->incEdgeCount();

                        edge_count_total++;
                    }

                    vec3 intersection_node_position = intersection_nodes.at(k)->getNodePosition();
                    std::shared_ptr<Node> intersection_node;
                    if (keyShiftDown == true) {
                        intersection_node.reset(new Node(vec3(intersection_node_position.x, intersection_node_position.y, intersection_node_position.z - KNOT_SPACING_DIST_MIN), KNOT_SPACING_DIST_MIN + 3.0f, intersection_nodes.at(k)->getColor())); //FIRST NODE
                    }
                    else {
                        intersection_node.reset(new Node(vec3(intersection_node_position.x, intersection_node_position.y, intersection_node_position.z + KNOT_SPACING_DIST_MIN), KNOT_SPACING_DIST_MIN + 3.0f, intersection_nodes.at(k)->getColor())); //FIRST NODE
                    }
                    knot_curr->addNode(intersection_node);
                    knot_curr->incNodeCount();
                    node_count_total++;
                    new_knot_count++;

                    std::shared_ptr<Edge> edge2;
                    edge2.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 2)->getNodePosition(), knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition(), knot_color_index[knot_count % 3]));
                    edge2->setArcLabel(knot_curr->getCrossingCount());
                    knot_curr->addEdge(edge2);
                    knot_curr->incEdgeCount();
                    edge_count_total++;

                    vec3 forward_node_vector;
                    if (k != 0) {
                        forward_node_vector = intersection_nodes.at(k - 1)->getNodePosition() - intersection_nodes.at(k)->getNodePosition();
                    }
                    else {
                        forward_node_vector = knot_curr->getEdgeGuide()->getPositionCurr() - intersection_nodes.at(0)->getNodePosition();
                    }
                    if (length(forward_node_vector) > KNOT_SPACING_DIST_MIN * 2.0f) {
                        std::shared_ptr<Node> forward_node;
                        forward_node.reset(new Node(intersection_nodes.at(k)->getNodePosition() + (vec3(direction_vector.x, direction_vector.y, 0.0f) * KNOT_SPACING_DIST_MIN), KNOT_SPACING_DIST_MIN + 2.0f, knot_color_index_2[knot_count % 3])); //FIRST NODE
                        knot_curr->addNode(forward_node);
                        knot_curr->incNodeCount();
                        node_count_total++;
                        new_knot_count++;

                        std::shared_ptr<Edge> edge3;
                        edge3.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 2)->getNodePosition(), knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition(), knot_color_index[knot_count % 3]));
                        edge3->setArcLabel(knot_curr->getCrossingCount());
                        knot_curr->addEdge(edge3);
                        knot_curr->incEdgeCount();
                        edge_count_total++;
                    }
                }

                if (knot_curr->getNodeCount() == 0) {
                    std::shared_ptr<Node> node;
                    node.reset(new Node(node_position_curr, KNOT_SPACING_DIST_MIN + 5.0f, COLOR_WHITE/*knot_color_index_2[knot_count % 3]*/)); //FIRST NODE
                    knot_curr->addNode(node);
                    knot_curr->incNodeCount();
                    node_count_total++;
                }
                else {
                    float knot_connection_dist = glm::sqrt(glm::pow(node_position_curr.x - knot_curr->getNodes().at(0)->getNodePosition().x, 2) + glm::pow(node_position_curr.y - knot_curr->getNodes().at(0)->getNodePosition().y, 2));
                    if (knot_connection_dist < KNOT_CONNECTION_DIST_MAX) {
                        std::shared_ptr<Edge> edge;
                        edge.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition(), knot_curr->getNodes().at(0)->getNodePosition(), knot_color_index[knot_count % 3]));
                        edge->setArcLabel(knot_curr->getCrossingCount());
                        knot_curr->addEdge(edge);
                        knot_curr->incEdgeCount();
                        edge_count_total++;
                        knot_curr->setNodeGuide(NULL);
                        knot_curr->setEdgeGuide(NULL);
                        knot_curr->getNodes().at(0)->setColor(knot_color_index_2[knot_count % 3]);

                        knot_count++;
                        std::shared_ptr<Knot> knot;
                        knot.reset(new Knot(knot_color_index[knot_count % 3]));
                        knots.push_back(knot);
                        knot_curr = knots.at(knot_count);
                        canKnotBeConnected = false;
                    }
                    else {
                        float edge_guide_length = glm::sqrt(glm::pow(node_position_curr.x - knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x, 2) + glm::pow(node_position_curr.y - knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y, 2));
                        if (edge_guide_length >= KNOT_SPACING_DIST_MIN) {
                            knot_curr->getNodeGuide()->setColor(knot_color_index_2[knot_count % 3]);

                            std::shared_ptr<Node> node;
                            node.reset(new Node(node_position_curr, KNOT_SPACING_DIST_MIN, knot_color_index_2[knot_count % 3]));
                            knot_curr->addNode(node);
                            knot_curr->incNodeCount();
                            node_count_total++;


                            if (knot_curr->getNodeCount() >= 0) {
                                knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->setOffset(KNOT_SPACING_DIST_MIN + 5.0f);
                            }
                            if (knot_curr->getNodeCount() >= 0) {
                                knot_curr->getNodes().at(knot_curr->getNodeCount() - 2 - new_knot_count)->setOffset(KNOT_SPACING_DIST_MIN);
                            }
                            knot_curr->setEdgeGuide(NULL);

                            std::shared_ptr<Edge> edge;
                            edge.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 2)->getNodePosition(), node_position_curr, knot_color_index[knot_count % 3]));
                            edge->setArcLabel(knot_curr->getCrossingCount());
                            knot_curr->addEdge(edge);
                            knot_curr->incEdgeCount();
                            edge_count_total++;
                        }
                    }
                }


                // OVERLAPPING EDGE DETECTION START
                for (int k = 0; k < intersection_nodes.size(); k++) {

                    int overlapping_edge = intersection_nodes.at(k)->getOverlappingEdge();
                    if (overlapping_edge != NULL) {
                        std::cout << "Overlapping edge: " << overlapping_edge << std::endl;
                        std::shared_ptr<Node> node_crossed;
                        node_crossed.reset(new Node(intersection_nodes.at(k)->getNodePosition(), 0.0f, COLOR_RED));

                        std::shared_ptr<Edge> edge_crossed_1;
                        edge_crossed_1.reset(new Edge(knot_curr->getEdges().at(overlapping_edge)->getPositionPrev(), intersection_nodes.at(k)->getNodePosition(), 0.0f, COLOR_DARK_YELLOW));
                        edge_crossed_1->setArcLabel(knot_curr->getCrossingCount());

                        std::shared_ptr<Edge> edge_crossed_2;
                        edge_crossed_2.reset(new Edge(intersection_nodes.at(k)->getNodePosition(), knot_curr->getEdges().at(overlapping_edge)->getPositionCurr(), 0.0f, COLOR_GREEN));
                        edge_crossed_2->setArcLabel(knot_curr->getCrossingCount());

                        knot_curr->insertNode(overlapping_edge, node_crossed);
                        knot_curr->incNodeCount();
                        node_count_total++;

                        knot_curr->eraseEdge(knot_curr->getEdges().at(overlapping_edge));
                        knot_curr->insertEdge(overlapping_edge, edge_crossed_1, edge_crossed_2);

                        knot_curr->incEdgeCount();
                        edge_count_total++;


                        for (int l = 0; l < intersection_nodes.size(); l++) {
                            if (intersection_nodes.at(l)->getOverlappingEdge() > overlapping_edge) {
                                intersection_nodes.at(l)->setOverlappingEdge(intersection_nodes.at(l)->getOverlappingEdge() + 1);
                            }
                        }
                    }


                    //knot_curr->incCrossingCount();
                    //crossing_count_total++;
                    if (intersection_nodes.at(k)->getColor() == glm::vec3(1.0 / 3.0, 1.0, 1.0 / 3.0)) { // IF GREEN
                        std::cout << "GREEN" << std::endl;
                        //knot_curr->getEdges().at(knot_curr->getEdgeCount() - 2)->setArcLabel(knot_curr->getEdges().at(knot_curr->getEdgeCount() - 2)->getArcLabel() + 1);
                        knot_curr->incCrossingCount();
                        crossing_count_total++;

                    }
                    else if (intersection_nodes.at(k)->getColor() == glm::vec3(1.0, 1.0, 1.0 / 3.0)) { // IF YELLOW
                        std::cout << "YELLOW" << std::endl;
                        std::cout << "edge overlap: " << overlapping_edge << std::endl;
                        for (int m = overlapping_edge + 1; m < knot_curr->getEdgeCount(); m++) {
                            std::cout << "increased" << std::endl;
                            knot_curr->getEdges().at(m)->setArcLabel(knot_curr->getEdges().at(m)->getArcLabel() + 1);
                        }
                        knot_curr->incCrossingCount();
                        crossing_count_total++;
                    }
                }
                // END OVERLAPPING EDGE DETECTION

            }
        }
        mouseDown = true;
    }
    else if (name == "MouseBtnRight_Down") {
        mouseRightDown = true;
        is_painting = false;
    }
    else if (name == "KbdLeftAlt_Down") {
        if (visualizer_mode == 1) {
            currTheta = glm::radians(90.0);
            currPhi = glm::radians(90.0);

            eye_world = angleToSpherePoint(currTheta, currPhi);
            vec3 look_vector = -normalize(eye_world);
            if (glm::degrees(currPhi) == 0.0) {
                up_vector = vec3(-cos(currTheta), 0.0, -sin(currTheta));
            }
            else if (glm::degrees(currPhi) == 180.0) {
                up_vector = vec3(cos(currTheta), 0.0, sin(currTheta));
            }
            else {
                vec3 right_vector = normalize(cross(look_vector, vec3(0.0, 1.0, 0.0)));
                up_vector = normalize(cross(right_vector, look_vector));
            }
        }
    }
    else if (name == "KbdLeftShift_Down" || name == "KbdRightShift_Down") {
        keyShiftDown = true;
    }
    else if (name == "KbdF2_Down") {
        if (isShadersEnabled == true) {
            isShadersEnabled = false;
        }
        else {
            isShadersEnabled = true;
        }

    }
    else if (name == "KbdF3_Down") {
        if (visualizer_mode == 0) {
            visualizer_mode = 1;
            drawFancyKnots();
        }
        else {
            visualizer_mode = 0;

            currTheta = glm::radians(90.0);
            currPhi = glm::radians(90.0);

            eye_world = angleToSpherePoint(currTheta, currPhi);
            vec3 look_vector = -normalize(eye_world);
            if (glm::degrees(currPhi) == 0.0) {
                up_vector = vec3(-cos(currTheta), 0.0, -sin(currTheta));
            }
            else if (glm::degrees(currPhi) == 180.0) {
                up_vector = vec3(cos(currTheta), 0.0, sin(currTheta));
            }
            else {
                vec3 right_vector = normalize(cross(look_vector, vec3(0.0, 1.0, 0.0)));
                up_vector = normalize(cross(right_vector, look_vector));
            }
        }
    }
    else if (name == "KbdF1_Down") {
        if (isDebugEnabled == true) {
            isDebugEnabled = false;
        }
        else {
            isDebugEnabled = true;
        }
    }
    else if (name == "KbdF4_Down") {
        currTheta = glm::radians(90.0);
        currPhi = glm::radians(90.0);

        eye_world = angleToSpherePoint(currTheta, currPhi);
        vec3 look_vector = -normalize(eye_world);
        if (glm::degrees(currPhi) == 0.0) {
            up_vector = vec3(-cos(currTheta), 0.0, -sin(currTheta));
        }
        else if (glm::degrees(currPhi) == 180.0) {
            up_vector = vec3(cos(currTheta), 0.0, sin(currTheta));
        }
        else {
            vec3 right_vector = normalize(cross(look_vector, vec3(0.0, 1.0, 0.0)));
            up_vector = normalize(cross(right_vector, look_vector));
        }

        // reset

        mouseDown = false;
        mouseRightDown = false;

        knots.clear();

        knot_count = 0;
        node_count_total = 0;
        edge_count_total = 0;
        visualizer_mode = 0;

        std::shared_ptr<Knot> knot;
        knot.reset(new Knot(knot_color_index[knot_count % 3]));
        knots.push_back(knot);
        knot_curr = knots.at(knot_count);

        canNodeBePlaced = true;
        canKnotBeConnected = false;
        keyShiftDown = false;

    }
    else if (name == "KbdF5_Down") {
        for (int i = 0; i < knot_curr->getEdgeCount(); i++) {
            if (knot_curr->getEdges().at(i)->getArcLabel() == 0) {
                knot_curr->getEdges().at(i)->setColor(COLOR_RED);
            } else if (knot_curr->getEdges().at(i)->getArcLabel() == 1) {
                knot_curr->getEdges().at(i)->setColor(COLOR_DARK_YELLOW);
            }
            else if (knot_curr->getEdges().at(i)->getArcLabel() == 2) {
                knot_curr->getEdges().at(i)->setColor(COLOR_BLUE);
            }
            else if (knot_curr->getEdges().at(i)->getArcLabel() == 3) {
                knot_curr->getEdges().at(i)->setColor(COLOR_MAGENTA);
            } 
            else if (knot_curr->getEdges().at(i)->getArcLabel() == 4) {
                knot_curr->getEdges().at(i)->setColor(COLOR_LIGHT_BLUE);
            }
            else if (knot_curr->getEdges().at(i)->getArcLabel() == 5) {
                knot_curr->getEdges().at(i)->setColor(COLOR_GREEN);
            }
            else {
                knot_curr->getEdges().at(i)->setColor(COLOR_WHITE);
            }
        }
    }
}

void App::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.

	//std::cout << "ButtonUp: " << event.getName() << std::endl;
	if (event.getName() == "MouseBtnLeft_Up") {
		mouseDown = false;
	}
    else if (event.getName() == "MouseBtnRight_Up") {
        mouseRightDown = false;
    }
    else if (event.getName() == "KbdLeftShift_Up" || event.getName() == "KbdRightShift_Up") {
        keyShiftDown = false;
    }
}

void App::onCursorMove(const VRCursorEvent &event) {
    mousePosX = event.getPos()[0];
    mousePosY = event.getPos()[1];

    node_cursor_position = vec3((mousePosX - 512.0) / 20.48, -(mousePosY - 512.0) / 20.48, 0.0);

    if (node_cursor_position.x >= -25.0 && node_cursor_position.x <= 25.0 && node_cursor_position.y >= -25.0 && node_cursor_position.y <= 25.0) { // within bounds
        std::shared_ptr<Node> node_guide;
        node_guide.reset(new Node(vec3(node_cursor_position.x, node_cursor_position.y, node_cursor_position.z), KNOT_SPACING_DIST_MIN + 6.0f, COLOR_WHITE/*knot_color_index_2[knot_count % 3]*/));
        knot_curr->setNodeGuide(node_guide);


        //TEST
        intersection_nodes.clear();
        if (knot_curr->getEdgeGuide() != NULL) {
            calcEdgeIntersection();
        }

        //ENDTEST








        // HANDLES KNOT CONNECTION
        float knot_connection_dist = 1000.0f;
        if (knot_curr->getNodeCount() > 2) {
            knot_connection_dist = glm::sqrt(glm::pow(node_cursor_position.x - knot_curr->getNodes().at(0)->getNodePosition().x, 2) + glm::pow(node_cursor_position.y - knot_curr->getNodes().at(0)->getNodePosition().y, 2));
        }
        if (knot_connection_dist < KNOT_CONNECTION_DIST_MAX) {
            std::shared_ptr<Edge> edge_guide;
            edge_guide.reset(new Edge(
                vec3(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().z),
                vec3(knot_curr->getNodes().at(0)->getNodePosition().x,
                    knot_curr->getNodes().at(0)->getNodePosition().y,
                    knot_curr->getNodes().at(0)->getNodePosition().z), 
                KNOT_SPACING_DIST_MIN + 4.0f,
                COLOR_WHITE/*knot_color_index[knot_count % 3]*/));
            knot_curr->setEdgeGuide(edge_guide);

            std::shared_ptr<Node> node_guide;
            node_guide.reset(new Node(knot_curr->getNodes().at(0)->getNodePosition(), KNOT_SPACING_DIST_MIN + 5.0f, knot_color_index_2[knot_count % 3]));
            knot_curr->setNodeGuide(node_guide);

            canKnotBeConnected = true;
            canNodeBePlaced = true;
        }
        else {
            canKnotBeConnected = false;
            // HANDLES KNOT VALID PLACEMENT
            float edge_dist_nearest = calcEdgeDistNearest(node_cursor_position); // calculate distance of nearest edge to cursor
            if (edge_dist_nearest < KNOT_SPACING_DIST_MIN && knot_curr->getNodeCount() == 0 && node_count_total != 0) {
                std::shared_ptr<Node> node_guide;
                node_guide.reset(new Node(vec3(node_cursor_position.x, node_cursor_position.y, node_cursor_position.z), KNOT_SPACING_DIST_MIN + 6.0f, COLOR_RED));
                knot_curr->setNodeGuide(node_guide);
                canNodeBePlaced = false;
            }


            else if (edge_dist_nearest >= KNOT_SPACING_DIST_MIN && knot_curr->getNodeCount() > 1) { // cursor node can be placed in valid spot
                canNodeBePlaced = true;

                //if (mouseDown == true) {
                //    std::shared_ptr<Node> node;
                //    node.reset(new Node(node_cursor_position, COLOR_WHITE));
                //    knot_curr->addNode(node);
                //    knot_curr->incNodeCount();
                //    node_total_count++;

                //    std::shared_ptr<Edge> edge;
                //    edge.reset(new Edge(knot_curr->getNodes().at(knot_curr->getNodeCount() - 2)->getNodePosition(), node_cursor_position, knot_color_index[knot_count % 3]));
                //    knot_curr->addEdge(edge);
                //    knot_curr->incEdgeCount();
                //}
                std::shared_ptr<Edge> edge_guide;
                edge_guide.reset(new Edge(vec3(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().z),
                    vec3(node_cursor_position.x,
                        node_cursor_position.y,
                        node_cursor_position.z),
                    KNOT_SPACING_DIST_MIN + 4.0f, COLOR_DARK_GRAY/*knot_color_index[knot_count % 3]*/));
                knot_curr->setEdgeGuide(edge_guide);
            }
            else if (edge_dist_nearest < KNOT_SPACING_DIST_MIN && node_count_total > 1) {
                std::shared_ptr<Edge> edge_guide;
                edge_guide.reset(new Edge(vec3(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y,
                    knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().z),
                    vec3(node_cursor_position.x,
                        node_cursor_position.y,
                        node_cursor_position.z),
                    KNOT_SPACING_DIST_MIN + 4.0f,
                    COLOR_RED));
                knot_curr->setEdgeGuide(edge_guide);
                canNodeBePlaced = false;
            }

            else if (knot_curr->getNodeCount() == 1) {
                vec3 dist = vec3(node_cursor_position - knot_curr->getNodes().at(0)->getNodePosition());
                float edge_dist_nearest = glm::sqrt(glm::pow(dist.x, 2) + glm::pow(dist.y, 2) + glm::pow(dist.z, 2));; // calculate distance of vertex to cursor
                if (edge_dist_nearest < KNOT_SPACING_DIST_MIN) { // cursor node can be placed in valid spot
                    std::shared_ptr<Edge> edge_guide;
                    edge_guide.reset(new Edge(vec3(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x,
                        knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y,
                        knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().z),
                        vec3(node_cursor_position.x,
                            node_cursor_position.y,
                            node_cursor_position.z),
                        KNOT_SPACING_DIST_MIN + 4.0f,
                        COLOR_RED));
                    knot_curr->setEdgeGuide(edge_guide);

                    canNodeBePlaced = false;
                }
                else {
                    std::shared_ptr<Edge> edge_guide;
                    edge_guide.reset(new Edge(vec3(knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().x,
                        knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().y,
                        knot_curr->getNodes().at(knot_curr->getNodeCount() - 1)->getNodePosition().z),
                        vec3(node_cursor_position.x,
                            node_cursor_position.y,
                            node_cursor_position.z),
                        KNOT_SPACING_DIST_MIN + 4.0f,
                        COLOR_DARK_GRAY/*knot_color_index[knot_count % 3]*/));
                    knot_curr->setEdgeGuide(edge_guide);


                    canNodeBePlaced = true;
                }
            }
            else {
                canNodeBePlaced = true;
            }
        }
    }
    
    if (mouseRightDown) {
        if (visualizer_mode == 0 || visualizer_mode == 1) {
            vec2 dxy = vec2(event.getPos()[0], event.getPos()[1]) - lastMousePos;

            float changeTheta = glm::atan(dxy.x * _CAMERA_SENSITIVITY, _CAMERA_RADIUS);
            float changePhi = glm::atan(-dxy.y * _CAMERA_SENSITIVITY, _CAMERA_RADIUS);
            currTheta += changeTheta;
            currPhi += changePhi;

            currPhi = glm::clamp(currPhi, glm::radians(0.0f), glm::radians(180.0f));

            eye_world = angleToSpherePoint(currTheta, currPhi);
            vec3 look_vector = -normalize(eye_world);
            if (glm::degrees(currPhi) == 0.0) {
                up_vector = vec3(-cos(currTheta), 0.0, -sin(currTheta));
            }
            else if (glm::degrees(currPhi) == 180.0) {
                up_vector = vec3(cos(currTheta), 0.0, sin(currTheta));
            }
            else {
                vec3 right_vector = normalize(cross(look_vector, vec3(0.0, 1.0, 0.0)));
                up_vector = normalize(cross(right_vector, look_vector));
            }
        }
	}
    
    
	lastMousePos = vec2(event.getPos()[0], event.getPos()[1]);
}

void App::onTrackerMove(const VRTrackerEvent &event) {
    // This routine is called for all Tracker_Move events.  Check event->getName()
    // to see exactly which tracker has moved, and then access the tracker's new
    // 4x4 transformation matrix with event->getTransform().

	// We will use trackers when we do a virtual reality assignment. For now, you can ignore this input type.
}

    
void App::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame.
    
	// Is this the first frame that we are rendering after starting the app?
    if (renderState.isInitialRenderCall()) {

		//For windows, we need to initialize a few more things for it to recognize all of the
		// opengl calls.
		#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
		#endif     

        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        // Set the Environment with a simple Binn-Phong shader
        _environment_shader.compileShader("vertex.vert", GLSLShader::VERTEX);
        _environment_shader.compileShader("environment.frag", GLSLShader::FRAGMENT);
        _environment_shader.link();
        _environment_shader.use();
        //initEnvironment();

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();

		initializeText();
        
        // Initialize the texture environment map
        // Order should be:
        // +X (right)
        // -X (left)
        // +Y (top)
        // -Y (bottom)
        // +Z (front)
        // -Z (back)
        //string textureFiles[] = {"desert_evening_east.jpg", "desert_evening_west.jpg", "desert_evening_up.jpg", "desert_evening_down.jpg", "desert_evening_north.jpg", "desert_evening_south.jpg"};
        //string textureFiles[] = { "skybox_px.jpg", "skybox_nx.jpg", "skybox_py.jpg", "skybox_ny.jpg", "skybox_pz.jpg", "skybox_nz.jpg" };
        //string textureFiles[] = { "partcloudy_px.jpg", "partcloudy_nx.jpg", "partcloudy_py.jpg", "partcloudy_ny.jpg", "partcloudy_pz.jpg", "partcloudy_nz.jpg" };
        string textureFiles[] = { "blackskybox.jpg", "blackskybox.jpg", "blackskybox.jpg", "blackskybox.jpg", "blackskybox.jpg", "blackskybox.jpg" };
        environmentMap = Texture::createCubeMapFromFiles(textureFiles, true, 4);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        environmentMap->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        
        //_tex = Texture::create2DTextureFromFile("images/poolTile.jpg");
        //_tex->setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
        //_tex->setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
        //_tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //_tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

}


void App::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
	//glm::vec3 eye_world = glm::vec3(0, 30, 0);
	glm::mat4 view = glm::lookAt(eye_world, glm::vec3(0, 0, 0), up_vector);
	// When we use virtual reality, this will be replaced by:
	// eye_world = glm::make_vec3(renderState.getCameraPos())
	// view = glm::make_mat4(renderState.getViewMatrix());

	// Setup the projection matrix so that things are rendered in perspective
	GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
	GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
	//glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
    glm::mat4 projection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 0.01f, 100.0f);
    //glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
	// When we use virtual reality, this will be replaced by:
	// projection = glm::make_mat4(renderState.getProjectionMatrix())



// Tell opengl we want to use this specific shader.
_shader.use();

_shader.setUniform("view_mat", view);
_shader.setUniform("projection_mat", projection);

_shader.setUniform("model_mat", mat4(1.0));
_shader.setUniform("normal_mat", mat3(transpose(inverse(mat4(1.0)))));
_shader.setUniform("eye_world", eye_world);


// Set Uniforms
//_shader.setUniform("light_direction", _LIGHT_DIRECTION);

// Set Uniforms

float glassR0 = 0.4;
//float eta = 0.67;
vec3 glassEta(0.65, 0.67, 0.68);

_shader.setUniform("glassR0", glassR0);
_shader.setUniform("glassEta", glassEta);

glDisable(GL_CULL_FACE);
//_waterMesh->draw(_shader);

// TODO: enable then draw walls
_environment_shader.use();
// Properties of the material the model is made out of (the "K" terms in the equations discussed in class)
// These values should make the model look like it is made out of a metal, like brass
//vec3 ambientReflectionCoeff = vec3(0.22);
//vec3 diffuseReflectionCoeff = vec3(0.22);
//vec3 specularReflectionCoeff = vec3(0.15);

vec3 ambientReflectionCoeff = vec3(0.4125, 0.275, 0.0375);
vec3 diffuseReflectionCoeff = vec3(0.78, 0.57, 0.11);
vec3 specularReflectionCoeff = vec3(0.99, 0.94, 0.80);
float specularExponent = 27.9;

// Properties of the light source (the "I" terms in the equations discussed in class)
// These values are for a white light so the r,g,b intensities are all the same
//vec3 ambientLightIntensity = vec3(0.5, 0.5, 0.5);
//vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
//vec3 specularLightIntensity = vec3(0.9, 0.9, 0.9);
vec3 ambientLightIntensity = vec3(0.4, 0.4, 0.4);
vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);

_environment_shader.setUniform("materialColor", materialColor);
//_environment_shader.setUniform("lightDirection", _LIGHT_DIRECTION);

_environment_shader.setUniform("ambientReflectionCoeff", ambientReflectionCoeff);
_environment_shader.setUniform("diffuseReflectionCoeff", diffuseReflectionCoeff);
_environment_shader.setUniform("specularReflectionCoeff", specularReflectionCoeff);

_environment_shader.setUniform("specularExponent", specularExponent);

_environment_shader.setUniform("ambientLightIntensity", ambientLightIntensity);
_environment_shader.setUniform("diffuseLightIntensity", diffuseLightIntensity);
_environment_shader.setUniform("specularLightIntensity", specularLightIntensity);

_environment_shader.setUniform("view_mat", view);
_environment_shader.setUniform("projection_mat", projection);

_environment_shader.setUniform("model_mat", mat4(1.0));
_environment_shader.setUniform("normal_mat", mat3(transpose(inverse(mat4(1.0)))));
_environment_shader.setUniform("eye_world", eye_world);
glEnable(GL_CULL_FACE);  // Use GL_frontfacing if needed

/*
// CYLINDERS
_environment_shader.setUniform("materialColor", materialColor);
for (int i = 0; i < knot_vertex_count - 1; i++) {
    glm::mat4 model = mat4(1.0);
    vec3 vertex_curr = knot_vertices.at(i + 1);
    vec3 vertex_prev = knot_vertices.at(i);
    vec3 vertex_vec = normalize(vertex_curr - vertex_prev);

    vec3 point_middle = (vertex_curr + vertex_prev) / 2.0f;
    // T
    float rotation;
    if (vertex_vec.z <= 0) {
        rotation = glm::atan(vertex_vec.y / vertex_vec.z) + (glm::pi<float>() / 2.0);
    }
    else {
        rotation = glm::pi<float>() + glm::atan(vertex_vec.y / vertex_vec.z) + (glm::pi<float>() / 2.0);
    }

    // T

    glm::mat4 mat_identity = mat4(1.0);
    glm::mat4 mat_rotX = glm::rotate(mat_identity, -rotation, vec3(1, 0, 0));
    glm::mat4 mat_from_origin = glm::translate(mat_identity, point_middle);

    _shader.setUniform("model_mat", mat_from_origin * mat_rotX);
    knot_edge_meshes.at(i)->draw(_environment_shader);
}

// GUIDE
if (knot_guide_mesh != NULL) {
    glm::mat4 model = mat4(1.0);
    vec3 vertex_curr = knot_guide_vertex;
    vec3 vertex_prev = knot_vertices.at(knot_vertex_count - 1);
    vec3 vertex_vec = normalize(vertex_curr - vertex_prev);

    vec3 point_middle = (vertex_curr + vertex_prev) / 2.0f;

    float rotation;
    if (vertex_vec.z <= 0) {
        rotation = glm::atan(vertex_vec.y / vertex_vec.z) + (glm::pi<float>() / 2.0);
    }
    else {
        rotation = glm::pi<float>() + glm::atan(vertex_vec.y / vertex_vec.z) + (glm::pi<float>() / 2.0);
    }
        //std::cout << vertex_vec.y << " | " << vertex_vec.z << std::endl;
        //std::cout << glm::degrees(rotation) << std::endl;

        glm::mat4 mat_identity = mat4(1.0);
        glm::mat4 mat_rotX = glm::rotate(mat_identity, -rotation, vec3(1, 0, 0));
        glm::mat4 mat_from_origin = glm::translate(mat_identity, point_middle);
       

        _environment_shader.setUniform("model_mat", mat_from_origin * mat_rotX);
        _environment_shader.setUniform("materialColor", knot_guide_color);
        knot_guide_mesh->draw(_environment_shader);
    }
    */
    if (visualizer_mode == 0) {
        // Knots draw   
        for (size_t i = 0; i < knots.size(); i++) {
            knots.at(i)->draw(_environment_shader);
        }

        // Intersection nodes draw
        for (size_t i = 0; i < intersection_nodes.size(); i++) {
            if (keyShiftDown == true) {
                intersection_nodes.at(i)->setColor(COLOR_LIGHT_GREEN);
            }
            else {
                intersection_nodes.at(i)->setColor(COLOR_YELLOW);
                //if (i == 0) {
                //    intersection_nodes.at(i)->setColor(COLOR_YELLOW);
                //}
                //else if (i == 1) {
                //    intersection_nodes.at(i)->setColor(COLOR_MAGENTA);
                //}
                //else if (i == 2) {
                //    intersection_nodes.at(i)->setColor(COLOR_GREEN);
                //}
                //else if (i == 3) {
                //    intersection_nodes.at(i)->setColor(COLOR_CYAN);
                //}
                //else if (i == 4) {
                //    intersection_nodes.at(i)->setColor(COLOR_DARK_YELLOW);
                //}
            }
            intersection_nodes.at(i)->draw(_environment_shader);
        }
    }
    else if (visualizer_mode == 1) {
        for (size_t i = 0; i < knots.size(); i++) {
            knots.at(i)->drawStitched(_environment_shader);
        }
    }

    // Set shaders on or off
    if (isShadersEnabled == true) {
        for (size_t i = 0; i < knots.size(); i++) {
            knots.at(i)->setComponentMix(0.4f);
        }
    }
    else {
        for (size_t i = 0; i < knots.size(); i++) {
            knots.at(i)->setComponentMix(0.0f);
        }
    }

    // Draw text
    double deltaTime = _curFrameTime - _lastTime;

    if (isDebugEnabled == true) {
        fonsSetColor(fs, glfonsRGBA(255, 255, 255, 255));
        fonsSetAlign(fs, FONS_ALIGN_RIGHT | FONS_ALIGN_TOP);
        std::string fps_text = "fps";
        drawText(fps_text, windowWidth, 0, windowHeight, windowWidth);
        fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
        std::string title_text = "Knot Visualizer";
        drawText(title_text, 0, 0, windowHeight, windowWidth);
        std::string mouse_pos_text_x = "mouse_x:";
        drawText(mouse_pos_text_x, 0, 32, windowHeight, windowWidth);
        std::string mouse_pos_text_y = "mouse_y:";
        drawText(mouse_pos_text_y, 0, 48, windowHeight, windowWidth);
        std::string mouse_pos_text_z = "mouse_z:";
        drawText(mouse_pos_text_z, 0, 64, windowHeight, windowWidth);
        std::string world_pos_text_x = "world_x:";
        drawText(world_pos_text_x, 0, (16 * 6), windowHeight, windowWidth);
        std::string world_pos_text_y = "world_y:";
        drawText(world_pos_text_y, 0, (16 * 7), windowHeight, windowWidth);
        std::string world_pos_text_z = "world_z:";
        drawText(world_pos_text_z, 0, (16 * 8), windowHeight, windowWidth);
        std::string theta_text = "theta:";
        drawText(theta_text, 0, (16 * 9), windowHeight, windowWidth);
        std::string phi_text = "phi:";
        drawText(phi_text, 0, (16 * 10), windowHeight, windowWidth);
        std::string knot_count_text = "knot:";
        drawText(knot_count_text, 0, (16 * 12), windowHeight, windowWidth);
        std::string node_count_text = "nodes:";
        drawText(node_count_text, 0, (16 * 13), windowHeight, windowWidth);
        std::string edge_count_text = "edges:";
        drawText(edge_count_text, 0, (16 * 14), windowHeight, windowWidth);
        std::string crossing_count_text = "crossings:";
        drawText(crossing_count_text, 0, (16 * 15), windowHeight, windowWidth);
        std::string colorability_text = "is_p_colorable:";
        drawText(colorability_text, 0, (16 * 17), windowHeight, windowWidth);


        fonsSetColor(fs, glfonsRGBA(255, 255, 0, 255));
        fonsSetAlign(fs, FONS_ALIGN_RIGHT | FONS_ALIGN_TOP);
        std::string fps_text_2 = std::to_string(1.0 / deltaTime) + "    ";
        drawText(fps_text_2, windowWidth, 0, windowHeight, windowWidth);

        fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
        std::string mouse_pos_text_x_2;
        std::string mouse_pos_text_y_2;
        std::string mouse_pos_text_z_2;
        if (mousePosX < 1024.0f && mousePosX > 0.0f) {
            //mouse_pos_text_x_2 = "         " + std::to_string((int)mousePosX);
            mouse_pos_text_x_2 = "         " + std::to_string((mousePosX - 512.0) / 20.48);
            mouse_pos_text_z_2 = "         0.000000";
        }
        else {
            mouse_pos_text_x_2 = "         NULL";
            mouse_pos_text_z_2 = "         NULL";
        }
        if (mousePosY < 1024.0f && mousePosY > 0.0f) {
           // mouse_pos_text_y_2 = "         " + std::to_string((int)mousePosY);
            mouse_pos_text_y_2 = "         " + std::to_string(-(mousePosY - 512.0) / 20.48);
            mouse_pos_text_z_2 = "         0.000000";
        }
        else {
            mouse_pos_text_y_2 = "         NULL";
            mouse_pos_text_z_2 = "         NULL";
        }
        drawText(mouse_pos_text_x_2, 0, 32, windowHeight, windowWidth);
        drawText(mouse_pos_text_y_2, 0, 48, windowHeight, windowWidth);
        drawText(mouse_pos_text_z_2, 0, 64, windowHeight, windowWidth);

        std::string world_pos_text_x_2 = "         " + std::to_string((float)0);
        drawText(world_pos_text_x_2, 0, (16 * 6), windowHeight, windowWidth);
        std::string world_pos_text_y_2 = "         " + std::to_string((float)0);
        drawText(world_pos_text_y_2, 0, (16 * 7), windowHeight, windowWidth);
        std::string world_pos_text_z_2 = "         " + std::to_string((float)0);
        drawText(world_pos_text_z_2, 0, (16 * 8), windowHeight, windowWidth);

        std::string theta_text_2 = "         " + std::to_string(glm::degrees(currTheta));
        drawText(theta_text_2, 0, (16 * 9), windowHeight, windowWidth);
        std::string phi_text_2 = "         " + std::to_string(glm::degrees(currPhi));
        drawText(phi_text_2, 0, (16 * 10), windowHeight, windowWidth);

        std::string knot_count_text_2 = "           " + std::to_string(knot_count);
        drawText(knot_count_text_2, 0, (16 * 12), windowHeight, windowWidth);
        std::string node_count_text_2 = "           " + std::to_string(knot_curr->getNodeCount()) + " / " + std::to_string(node_count_total);
        drawText(node_count_text_2, 0, (16 * 13), windowHeight, windowWidth);
        std::string edge_count_text_2 = "           " + std::to_string(knot_curr->getEdgeCount()) + " / " + std::to_string(edge_count_total);
        drawText(edge_count_text_2, 0, (16 * 14), windowHeight, windowWidth);
        std::string crossing_count_text_2 = "           " + std::to_string(knot_curr->getCrossingCount()) + " / " + std::to_string(crossing_count_total);
        drawText(crossing_count_text_2, 0, (16 * 15), windowHeight, windowWidth);
        std::string colorability_text_2 = "                NULL";
        drawText(colorability_text_2, 0, (16 * 17), windowHeight, windowWidth);
    }
}

void App::drawText(const std::string text, float xPos, float yPos, GLfloat windowHeight, GLfloat windowWidth) {
	//float lh = 0;
	//fonsVertMetrics(fs, NULL, NULL, &lh);
	//double width = fonsTextBounds(fs, text.c_str(), NULL, NULL) + 40;
	//double height = lh + 40;

	_textShader.use();
	_textShader.setUniform("projection_mat", glm::ortho(0.f, windowWidth, windowHeight, 0.f, -1.f, 1.f));
	_textShader.setUniform("view_mat", glm::mat4(1.0));
	_textShader.setUniform("model_mat", glm::mat4(1.0));
	_textShader.setUniform("lambertian_texture", 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fonsDrawText(fs, xPos, yPos, text.c_str(), NULL);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	_shader.use();

}

void App::reloadShaders()
{
	_shader.compileShader("vertex.vert", GLSLShader::VERTEX);
	_shader.compileShader("caustic.frag", GLSLShader::FRAGMENT);
	_shader.link();
	_shader.use();
}

vec3 App::angleToSpherePoint(float theta, float phi) {
    float x = _CAMERA_RADIUS * sin(phi) * cos(theta);
    float y = _CAMERA_RADIUS * cos(phi);
    float z = _CAMERA_RADIUS * sin(phi) * sin(theta);

	return vec3(x, y, z);
}

/*
void App::addGeometryGuide(vec3 knot_vertex_prev, vec3 knot_vertex_curr, float knot_vertex_distance) {
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;

    //Test
    float vert_displacement_y = 0.0f;
    float angle = 0.0f;
    if (knot_vertex_count >= 2) {
        vec3 a = glm::normalize(knot_guide_vertex - knot_vertices.at(knot_vertex_count - 1));
        vec3 b = glm::normalize(knot_vertices.at(knot_vertex_count - 2) - knot_vertices.at(knot_vertex_count - 1));
        angle = (180.0 - glm::degrees(glm::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)))) ) / 2.0;
        vert_displacement_y = KNOT_RADIUS * glm::tan(glm::radians(angle));
    }
    float hyp_len = glm::sqrt(KNOT_RADIUS * KNOT_RADIUS + vert_displacement_y * vert_displacement_y);
    

    //Endtest

    const float EDGE_LENGTH = glm::sqrt(glm::pow(knot_vertex_prev.y - knot_vertex_curr.y, 2) + glm::pow(knot_vertex_prev.z - knot_vertex_curr.z, 2));

    const int N_CAN_SIDES = 40;
    float starting_angle = -glm::pi<float>() / 2.0;

    float x_pos;
    float z_pos;

    std::vector<vec3> knot_guide_vertices_temp;

    float vert_height_y = vert_displacement_y;
    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_CAN_SIDES; i++) {
        x_pos = KNOT_RADIUS * glm::cos(starting_angle);
        z_pos = KNOT_RADIUS * glm::sin(starting_angle);

        vert.position = vec3(x_pos, -knot_vertex_distance / 2.0 - vert_height_y, z_pos);
        knot_guide_vertices_temp.push_back(vert.position);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 1.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        vert.position = vec3(x_pos, knot_vertex_distance / 2.0, z_pos);
        knot_guide_vertices_temp.push_back(vert.position);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        starting_angle += 2 * glm::pi<float>() / (N_CAN_SIDES);

        vert_height_y = vert_displacement_y * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_CAN_SIDES + (glm::pi<float>() * 0.5f));
    }
    knot_guide_vertices = knot_guide_vertices_temp;

    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    knot_guide_mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
    if (knot_vertex_count >= 2) {
        updateLastEdge(vert_displacement_y);
    }
}

void App::updateLastEdge(float vert_displacement_y) {
    vec3 knot_vertex_prev = knot_vertices.at(knot_vertex_count - 2);
    vec3 knot_vertex_curr = knot_vertices.at(knot_vertex_count - 1);
    float knot_vertex_distance = glm::sqrt(glm::pow(knot_vertex_prev.y - knot_vertex_curr.y, 2) + glm::pow(knot_vertex_prev.z - knot_vertex_curr.z, 2));


    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;

    const float EDGE_LENGTH = glm::sqrt(glm::pow(knot_vertex_prev.y - knot_vertex_curr.y, 2) + glm::pow(knot_vertex_prev.z - knot_vertex_curr.z, 2));

    const int N_CAN_SIDES = 40;
    float starting_angle = -glm::pi<float>() / 2.0;

    float x_pos;
    float z_pos;

    float vert_height_y = vert_displacement_y;
    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_CAN_SIDES; i++) {
        x_pos = KNOT_RADIUS * glm::cos(starting_angle);
        z_pos = KNOT_RADIUS * glm::sin(starting_angle);

        vert.position = last_knot_vertices.at(vert_count);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);


        vert.position = vec3(x_pos, knot_vertex_distance / 2.0 + vert_height_y, z_pos);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);


        starting_angle += 2 * glm::pi<float>() / (N_CAN_SIDES);
        vert_height_y = vert_displacement_y * glm::sin((float)(i + 1) * glm::pi<float>() * 2.0f / N_CAN_SIDES + (glm::pi<float>() * 0.5f));
    }

    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    std::unique_ptr<basicgraphics::Mesh> knot_guide_mesh;
    knot_guide_mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
    knot_edge_meshes.at(knot_vertex_count - 2) = std::move(knot_guide_mesh);
}

void App::addGeometryEdge() {
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
    std::vector<std::shared_ptr<Texture>> textures;
    std::cout << "A" << std::endl;
    vec3 knot_vertex_prev = knot_vertices.at(knot_vertex_count - 2);
    vec3 knot_vertex_curr = knot_vertices.at(knot_vertex_count - 1);
    std::cout << "B" << std::endl;

    const float EDGE_LENGTH = glm::sqrt(glm::pow(knot_vertex_prev.y - knot_vertex_curr.y, 2) + glm::pow(knot_vertex_prev.z - knot_vertex_curr.z, 2));

    const int N_CAN_SIDES = 40;
    float starting_angle = -glm::pi<float>() / 2.0;

    float x_pos;
    float z_pos;

    Mesh::Vertex vert;
    int vert_count = 0;
    for (int i = 0; i <= N_CAN_SIDES; i++) {
        x_pos = KNOT_RADIUS * glm::cos(starting_angle);
        z_pos = KNOT_RADIUS * glm::sin(starting_angle);

        vert.position = vec3(x_pos, -EDGE_LENGTH / 2.0, z_pos);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 1.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);

        vert.position = vec3(x_pos, EDGE_LENGTH / 2.0, z_pos);
        vert.normal = vec3(glm::cos(starting_angle), 0, glm::sin(starting_angle));
        vert.texCoord0 = glm::vec2(1.0 - float(i) / float(N_CAN_SIDES), 0.0);
        cpuVertexArray.push_back(vert);
        cpuIndexArray.push_back(vert_count++);
   
        starting_angle += 2 * glm::pi<float>() / (N_CAN_SIDES);
    }

    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    std::unique_ptr<basicgraphics::Mesh> knot_edge_mesh;
    knot_edge_mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
    knot_edge_meshes.push_back(std::move(knot_edge_mesh));
}
*/

void App::initializeText() {
    int fontNormal = FONS_INVALID;
    fs = nullptr;

    fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
    if (fs == NULL) {
        assert(false);//Could not create stash
    }

    fontNormal = fonsAddFont(fs, "sans", "customfont.ttf");
    if (fontNormal == FONS_INVALID) {
        assert(false);// Could not add font normal.
    }

    unsigned int white = glfonsRGBA(0, 0, 85, 255);

    fonsClearState(fs);
    fonsSetSize(fs, 16);
    fonsSetFont(fs, fontNormal);
    fonsSetColor(fs, white);
    fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);

    _textShader.compileShader("textRendering.vert", GLSLShader::VERTEX);
    _textShader.compileShader("textRendering.frag", GLSLShader::FRAGMENT);
    _textShader.link();
}

float App::calcEdgeDistNearest(vec3 node_cursor_position) {
    float edge_dist_nearest = 1000.0f;
    for (int i = 0; i < knots.size(); i++) {
        std::shared_ptr<Knot> knot = knots.at(i);
        for (int j = 0; j < knot->getEdges().size(); j++) {
            vec3 edge_position_prev = knot->getEdges().at(j)->getPositionPrev();
            vec3 edge_position_curr = knot->getEdges().at(j)->getPositionCurr();

            vec3 vector_cursor_curr = vec3(node_cursor_position - edge_position_curr);
            vec3 vector_prev_curr = vec3(edge_position_prev - edge_position_curr);
            float dot_product = glm::dot(glm::normalize(vector_cursor_curr), glm::normalize(vector_prev_curr));
            float theta_betewen = glm::acos(dot_product);

            if (dot_product > 0.0f) {
                float vector_cursor_curr_len = glm::sqrt(glm::pow(vector_cursor_curr.x, 2) + glm::pow(vector_cursor_curr.y, 2) + glm::pow(vector_cursor_curr.z, 2));
                float vector_prev_curr_len = glm::sqrt(glm::pow(vector_prev_curr.x, 2) + glm::pow(vector_prev_curr.y, 2) + glm::pow(vector_prev_curr.z, 2));
                float projection = vector_cursor_curr_len * glm::cos(theta_betewen);
                if (projection < vector_prev_curr_len) {
                    float edge_dist = vector_cursor_curr_len * glm::sin(theta_betewen);
                    if (edge_dist < edge_dist_nearest) {
                        edge_dist_nearest = edge_dist;
                    }
                }
                else {
                    vec3 vector_cursor_prev = vec3(node_cursor_position - edge_position_prev);
                    float vector_cursor_prev_len = glm::sqrt(glm::pow(vector_cursor_prev.x, 2) + glm::pow(vector_cursor_prev.y, 2) + glm::pow(vector_cursor_prev.z, 2));
                    if (vector_cursor_prev_len < edge_dist_nearest) {
                        edge_dist_nearest = vector_cursor_prev_len;
                    }
                }
            }
            else {
                float vector_cursor_curr_len = glm::sqrt(glm::pow(vector_cursor_curr.x, 2) + glm::pow(vector_cursor_curr.y, 2) + glm::pow(vector_cursor_curr.z, 2));
                if (vector_cursor_curr_len < edge_dist_nearest) {
                    edge_dist_nearest = vector_cursor_curr_len;
                }
            }
        }
    }
    return edge_dist_nearest;
}

void App::calcEdgeIntersection() { 
    // Source: https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    float p0_x = knot_curr->getEdgeGuide()->getPositionPrev().x;
    float p0_y = knot_curr->getEdgeGuide()->getPositionPrev().y;
    float p1_x = knot_curr->getEdgeGuide()->getPositionCurr().x;
    float p1_y = knot_curr->getEdgeGuide()->getPositionCurr().y;
    //std::cout << "0X: " << p0_x << " | 0Y: " << p0_y << " | 1X: " << p1_x << " | 1Y: " << p1_y << std::endl;
    
    for (int i = 0; i < knots.size(); i++) {
        for (int j = 0; j < knots.at(i)->getEdges().size(); j++) {
            float p2_x = knots.at(i)->getEdges().at(j)->getPositionPrev().x;
            float p2_y = knots.at(i)->getEdges().at(j)->getPositionPrev().y;
            float p3_x = knots.at(i)->getEdges().at(j)->getPositionCurr().x;
            float p3_y = knots.at(i)->getEdges().at(j)->getPositionCurr().y;

            float s1_x, s1_y, s2_x, s2_y;
            s1_x = p1_x - p0_x;
            s1_y = p1_y - p0_y;
            s2_x = p3_x - p2_x;
            s2_y = p3_y - p2_y;

            float s, t;
            s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
            t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

            if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
            {
                float i_x = p0_x + (t * s1_x);
                float i_y = p0_y + (t * s1_y);

                if (i_x != p0_x && i_y != p0_y && i_x != knots.at(i)->getNodes().at(0)->getNodePosition().x && i_y != knots.at(i)->getNodes().at(0)->getNodePosition().y) {
                    std::shared_ptr<Node> node;
                    if (keyShiftDown) {
                        node.reset(new Node(vec3(i_x, i_y, 0.0f), KNOT_SPACING_DIST_MIN + 5.0f, COLOR_LIGHT_GREEN)); //FIRST NODE
                    }
                    else {
                        node.reset(new Node(vec3(i_x, i_y, 0.0f), KNOT_SPACING_DIST_MIN + 5.0f, COLOR_YELLOW)); //FIRST NODE
                        node->setOverlappingEdge(j);
                        //std::cout << "Overlapping edge set to " << j << std::endl;
                    }




                    // test for distances
                    float new_distFromGuidePrev = length(vec3(knot_curr->getEdgeGuide()->getPositionPrev().x - i_x, knot_curr->getEdgeGuide()->getPositionPrev().y - i_y, 0.0f));
                    int index = 0;
                    for (int i = 0; i < intersection_nodes.size(); i++) {
                        float index_distFromGuidePrev = length(vec3(knot_curr->getEdgeGuide()->getPositionPrev().x - intersection_nodes.at(i)->getNodePosition().x, knot_curr->getEdgeGuide()->getPositionPrev().y - intersection_nodes.at(i)->getNodePosition().y, 0.0f));
                        if (index_distFromGuidePrev > new_distFromGuidePrev) {
                            index++;
                        } else {
                            break;
                        }
                    }

                    intersection_nodes.insert(intersection_nodes.begin() + index, node);
                }
            }
        }
    }
}


void App::drawFancyKnots() {
    for (size_t i = 0; i < knots.size(); i++) {
        for (size_t j = 0; j < knots.at(i)->getEdgeCount(); j++) {
            vec3 vert0, vert1, vert2, vert3;
            //std::cout << "        " << std::endl;
            //std::cout << "PREV: " << knots.at(i)->getEdges().at(j)->getPositionPrev().x << " " << knots.at(i)->getEdges().at(j)->getPositionPrev().y << " " << knots.at(i)->getEdges().at(j)->getPositionPrev().z << std::endl;
            //std::cout << "CURR: " << knots.at(i)->getEdges().at(j)->getPositionCurr().x << " " << knots.at(i)->getEdges().at(j)->getPositionCurr().y << " " << knots.at(i)->getEdges().at(j)->getPositionCurr().z << std::endl;
            if (j == 0) {
                vert0 = knots.at(i)->getEdges().at(knots.at(i)->getEdgeCount() - 1)->getPositionPrev();
            }
            else {
                vert0 = knots.at(i)->getEdges().at(j - 1)->getPositionPrev();
            }
            vert1 = knots.at(i)->getEdges().at(j)->getPositionPrev();
            vert2 = knots.at(i)->getEdges().at(j)->getPositionCurr();
            if (j == knots.at(i)->getEdgeCount() - 1) {
                vert3 = knots.at(i)->getEdges().at(0)->getPositionCurr();
            }
            else {
                vert3 = knots.at(i)->getEdges().at(j + 1)->getPositionCurr();
            }

            std::shared_ptr<Edge> stitched_edge;
            stitched_edge.reset(new Edge(vert0, vert1, vert2, vert3, knot_color_index[i % 3]));
            knot_curr->addStitchedEdge(stitched_edge);
        }
    }
}

void App::calculatePColorability() {
    std::cout << "Calculating is p-colorable..." << std::endl;
    for (size_t i = 0; i < knots.size(); i++) {
        knots.at(i)->isPColorable(3);
    }
}
