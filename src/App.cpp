#include "App.h"
#include "Node.h"

#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

#include <config/VRDataIndex.h>

App::App(int argc, char** argv) : VRApp(argc, argv)
{
	_lastTime = 0.0;
	_curFrameTime = 0.0;

	currTheta = glm::radians(0.0);
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

void App::onButtonDown(const VRButtonEvent &event) {
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
        vec3 node_vertex = vec3(0.0, -(mousePosY - 512.0) / 20.48, -(mousePosX - 512.0) / 20.48);
        if (node_vertex.y >= -25.0 && node_vertex.y <= 25.0 && node_vertex.z >= -25.0 && node_vertex.z <= 25.0) { // within screen range
            if (node_count == 0) {
                std::shared_ptr<Node> node;
                node.reset(new Node(node_vertex));
                nodes.push_back(node);
                node_count++;
            }
            else {
                float edge_guide_length = glm::sqrt(glm::pow(node_vertex.y - nodes.at(node_count - 1)->getNodePosition().y, 2) + glm::pow(node_vertex.z - nodes.at(node_count - 1)->getNodePosition().z, 2));
                if (edge_guide_length >= NODE_EDGE_DISTANCE_MIN) {
                    std::shared_ptr<Node> node;
                    node.reset(new Node(node_vertex));
                    nodes.push_back(node);
                    node_count++;


                    //std::cout << "CLIK" << std::endl;
                    //knot_edge_meshes.push_back(std::move(knot_guide_mesh));
                    //last_knot_vertices = knot_guide_vertices;
                }
            }
        }
        mouseDown = true;
    }
    else if (name == "MouseBtnRight_Down") {
        mouseRightDown = true;
        is_painting = false;
    }
    else if (name == "KbdLeftAlt_Down") {
        if (is_painting == false) {
            currTheta = glm::radians(0.0);
            currPhi = glm::radians(0.0);

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
        is_painting = !is_painting;
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
}

void App::onCursorMove(const VRCursorEvent &event) {
	// This routine is called for all mouse move events. You can get the absolute position
	// or the relative position within the window scaled 0--1.
	//std::cout << "MouseMove: "<< event.getName() << " " << event.getPos()[0] << " " << event.getPos()[1] << std::endl;
    mousePosX = event.getPos()[0];
    mousePosY = event.getPos()[1];

    vec3 knot_vertex = vec3(0.0, -(event.getPos()[1] - 512.0) / 20.48, -(event.getPos()[0] - 512.0) / 20.48);
    if (knot_vertex_count > 0) {
        if (knot_vertex.y >= -25.0 && knot_vertex.y <= 25.0 && knot_vertex.z >= -25.0 && knot_vertex.z <= 25.0) {
            float knot_guide_length = glm::sqrt(glm::pow(knot_vertex.y - knot_vertices.at(knot_vertex_count - 1).y, 2) + glm::pow(knot_vertex.z - knot_vertices.at(knot_vertex_count - 1).z, 2));
            
            if (knot_guide_length < NODE_EDGE_DISTANCE_MIN) {
                knot_guide_color = vec3(1.0, 0.0, 0.0);
            }
            else {
                knot_guide_color = vec3(1.0, 1.0, 1.0);
            }
            knot_guide_vertex = knot_vertex;
            addGeometryGuide(knot_vertices.at(knot_vertex_count - 1), knot_guide_vertex, knot_guide_length);            
        }
    }

    //if (mouseDown) {
    //    float node_pos_y = -(event.getPos()[1] - 512.0) / 20.48;
    //    float node_pos_z = -(event.getPos()[0] - 512.0) / 20.48;
    //    if (node_pos_y >= -25.0 && node_pos_y <= 25.0 && node_pos_z >= -25.0 && node_pos_z <= 25.0) {
    //        float knot_vertex_distance = glm::sqrt(glm::pow(node_pos_y - knot_vertices.at(node_count - 1).y, 2) + glm::pow(node_pos_z - knot_vertices.at(node_count - 1).z, 2));
    //        if (knot_vertex_distance > KNOT_VERTEX_DIST_MIN) {
    //            //addGeometryNode(node_pos_y, node_pos_z);
    //            //knot_vertices.push_back(vec3(0.0, node_pos_y, node_pos_z));
    //            //addGeometryEdge(knot_vertices.at(node_count - 1), knot_vertices.at(node_count));
    //            //node_count++;
    //            //edge_count++;

    //        }
    //        else {
    //            addGeometryEdgeLead(knot_vertices.at(node_count - 1), knot_vertices.at(node_count));
    //        }
    //    }
    //}
    if (mouseRightDown) {
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
_shader.setUniform("light_direction", _LIGHT_DIRECTION);

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
_environment_shader.setUniform("lightDirection", _LIGHT_DIRECTION);

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

    // Nodes draw
    for (size_t i = 0; i < nodes.size(); i++) {
        nodes.at(i)->draw(_environment_shader);
    }

    // Draw text
    double deltaTime = _curFrameTime - _lastTime;
    std::string fps = "FPS: " + std::to_string(1.0 / deltaTime);
    drawText(fps, 10, 10, windowHeight, windowWidth);
    std::string node_count_text = "Nodes: " + std::to_string(node_count);
    drawText(node_count_text, 10, 30, windowHeight, windowWidth);
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

void App::initializeText() {
    int fontNormal = FONS_INVALID;
    fs = nullptr;

    fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
    if (fs == NULL) {
        assert(false);//Could not create stash
    }

    fontNormal = fonsAddFont(fs, "sans", "DroidSansMono.ttf");
    if (fontNormal == FONS_INVALID) {
        assert(false);// Could not add font normal.
    }

    unsigned int white = glfonsRGBA(255, 255, 255, 255);

    fonsClearState(fs);
    fonsSetSize(fs, 20);
    fonsSetFont(fs, fontNormal);
    fonsSetColor(fs, white);
    fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);

    _textShader.compileShader("textRendering.vert", GLSLShader::VERTEX);
    _textShader.compileShader("textRendering.frag", GLSLShader::FRAGMENT);
    _textShader.link();
}

