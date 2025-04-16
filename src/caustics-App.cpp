#include "caustics-App.h"

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
	currPhi = glm::radians(0.0);
	eye_world = angleToSpherePoint(currTheta, currPhi);

	up_vector = vec3(0, 0, 1);

	mouseDown = false;

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

	//std::cout << "ButtonDown: " << event.getName() << std::endl;
	string name = event.getName();
	if (name == "MouseBtnLeft_Down") {
		mouseDown = true;
	}

}

void App::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.

	//std::cout << "ButtonUp: " << event.getName() << std::endl;
	if (event.getName() == "MouseBtnLeft_Up") {
		mouseDown = false;
	}
}

void App::onCursorMove(const VRCursorEvent &event) {
	// This routine is called for all mouse move events. You can get the absolute position
	// or the relative position within the window scaled 0--1.
	
	//std::cout << "MouseMove: "<< event.getName() << " " << event.getPos()[0] << " " << event.getPos()[1] << std::endl;
	if (mouseDown) {
		vec2 dxy = vec2(event.getPos()[0], event.getPos()[1]) - lastMousePos;

		float theta = glm::atan(dxy.x / 10.0f, _CAMERA_RADIUS);
		float phi = glm::atan(dxy.y / 10.0f, _CAMERA_RADIUS);
		currTheta -= theta;
		currPhi += phi;
        
        currPhi = glm::clamp(currPhi, glm::radians(-90.0f), glm::radians(90.0f));

		eye_world = angleToSpherePoint(currTheta, currPhi);
		mat4 rotationX = glm::rotate(mat4(1.0), theta, vec3(0.0, 1.0, 0.0));
		mat4 rotationY = glm::rotate(mat4(1.0), phi, vec3(1.0, 0.0, 0.0));
		up_vector = rotationY * rotationX * vec4(up_vector, 0.0);
		
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
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        
        // Set the Environment with a simple Binn-Phong shader
        _environment_shader.compileShader("vertex.vert", GLSLShader::VERTEX);
        _environment_shader.compileShader("environment.frag", GLSLShader::FRAGMENT);
        _environment_shader.link();
        _environment_shader.use();
        initEnvironment();

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
        string textureFiles[] = {"desert_evening_east.jpg", "desert_evening_west.jpg", "desert_evening_up.jpg", "desert_evening_down.jpg", "desert_evening_north.jpg", "desert_evening_south.jpg"};
        environmentMap = Texture::createCubeMapFromFiles(textureFiles, true, 4);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        environmentMap->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        environmentMap->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        skyBox.reset(new Skybox(environmentMap));
        
        // set the light position to roughly match up with the sun position in the skybox texture
        float radius = 10.0;
        lightPosition = vec4(-1.7*radius, 0.3*radius, -1.0*radius, 1.0);
        
        //tex = Texture::create2DTextureFromFile("C:\\Users\\mackz\\Downloads\\poolWall.jpg");
        _tex = Texture::create2DTextureFromFile("/Users/miril/COMP465/comp465-project-ooooo-water-pretty/resources/images/poolTile.jpg");
        _tex->setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
        _tex->setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
        _tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        _tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
	initWaterMesh("SimpleZ");
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
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
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
	
	double deltaTime = _curFrameTime - _lastTime;
	std::string fps = "FPS: " + std::to_string(1.0/deltaTime);
	drawText(fps, 10, 10, windowHeight, windowWidth);
    
    glDisable(GL_CULL_FACE);
    _waterMesh->draw(_shader);
    
    // TODO: enable then draw walls
    _environment_shader.use();
    // Properties of the material the model is made out of (the "K" terms in the equations discussed in class)
    // These values should make the model look like it is made out of a metal, like brass
    vec3 ambientReflectionCoeff = vec3(0.4125, 0.275, 0.0375);
    vec3 diffuseReflectionCoeff = vec3(0.78, 0.57, 0.11);
    vec3 specularReflectionCoeff = vec3(0.99, 0.94, 0.80);
    float specularExponent = 27.9;
    
    // Properties of the light source (the "I" terms in the equations discussed in class)
    // These values are for a white light so the r,g,b intensities are all the same
    vec3 ambientLightIntensity = vec3(0.4, 0.4, 0.4);
    vec3 diffuseLightIntensity = vec3(0.6, 0.6, 0.6);
    vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);
    
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
    _environment_shader.setUniform("lightPosition", lightPosition);
    glEnable(GL_CULL_FACE);  // Use GL_frontfacing if needed
    _wallsMesh->draw(_shader);
    
    // Draw the skybox. Should be the last thing to draw
    skyBox->draw(view, projection);
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

vec3 App::angleToSpherePoint(float theta, float phi) {
	float x = _CAMERA_RADIUS * sin(phi) * sin(theta);
	float y = _CAMERA_RADIUS * cos(phi);
	float z = _CAMERA_RADIUS * sin(phi) * cos(theta);
	//std::cout << theta << " " << phi << std::endl;
	//
	//std::cout << x << " " << y << " " << z << std::endl;
	return vec3(x, y, z);
}

void App::initWaterMesh(string waveType) {
	// Update Water Mesh
	std::vector<Mesh::Vertex> cpuVertexArray;  // VBO
	std::vector<int> cpuIndexArray;  // Index list
	std::vector<std::shared_ptr<Texture>> textures;
    
    // TODO: add switch statement to choose a water function based on waveType
    simpleZWater(&cpuVertexArray, &cpuIndexArray);

	// Set the Water mesh
	const int numVertices = cpuVertexArray.size();
	const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
	const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

	_waterMesh.reset(new Mesh(textures, GL_TRIANGLES , GL_DYNAMIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));

	//_waterMesh->setMaterialColor(vec4(0.0, 0.0, 1.0, 1.0));

}

void App::simpleZWater(std::vector<Mesh::Vertex> *cpuVertexArray, std::vector<int> *cpuIndexArray) {
    // Create the vertices
    int counter = 0;
    for (float z = -_ENV_WIDTH / 2.0; z < _ENV_WIDTH / 2.0; z += _TILE_SIZE) {
        for (float x = -_ENV_WIDTH / 2.0; x < _ENV_WIDTH / 2.0; x += _TILE_SIZE) {
            float amplitude = 1.0f;
            float timeDis = _curFrameTime;
            float y_coord1 = amplitude * glm::sin(z + timeDis) + _ENV_HEIGHT / 2.0f - amplitude;  // The y-coordinate is constant (for now)
            float y_coord2 = amplitude * glm::sin(z + _TILE_SIZE + timeDis) + _ENV_HEIGHT / 2.0f - amplitude;

            Mesh::Vertex vert1;  // Top left
            vert1.position = vec3(x, y_coord1, z);
            vert1.normal = normalize(vec3(0, glm::sin(z + timeDis), glm::cos(z + timeDis)));
            vert1.texCoord0 = vec2(0, 0);
            cpuVertexArray->push_back(vert1);
            cpuIndexArray->push_back(counter);
            counter++;

            Mesh::Vertex vert2;  // Bottom left
            vert2.position = vec3(x, y_coord2, z + _TILE_SIZE);
            vert2.normal = normalize(vec3(0, glm::sin(z + _TILE_SIZE + timeDis), glm::cos(z + _TILE_SIZE + timeDis)));
            vert2.texCoord0 = vec2(0, 0);
            cpuVertexArray->push_back(vert2);
            cpuIndexArray->push_back(counter);
            counter++;

            Mesh::Vertex vert3;  // Top Right
            vert3.position = vec3(x + _TILE_SIZE, y_coord1, z );
            vert3.normal = normalize(vec3(0, glm::sin(z + timeDis), glm::cos(z + timeDis)));
            vert3.texCoord0 = vec2(0, 0);
            cpuVertexArray->push_back(vert3);
            cpuIndexArray->push_back(counter);
            cpuIndexArray->push_back(counter);  // Dupe
            counter++;

            cpuIndexArray->push_back(counter - 2); // Dupe the previous vertex

            Mesh::Vertex vert4;  // Bottom right
            vert4.position = vec3(x + _TILE_SIZE, y_coord2, z + _TILE_SIZE);
            vert4.normal = normalize(vec3(0, glm::sin(z + _TILE_SIZE + timeDis), glm::cos(z + _TILE_SIZE + timeDis)));
            vert4.texCoord0 = vec2(0, 0);
            cpuVertexArray->push_back(vert4);
            cpuIndexArray->push_back(counter);
            counter++;
        }
    }
}

void App::initEnvironment() {
    std::vector<Mesh::Vertex> cpuVertexArray;  // VBO
    std::vector<int> cpuIndexArray;  // Index list
    std::vector<std::shared_ptr<Texture>> textures;

    // Set the mesh
    Mesh::Vertex backTopLeft;  // 0
    backTopLeft.position = vec3(-_ENV_WIDTH/2.0f, _ENV_HEIGHT/2.0f, -_ENV_WIDTH/2.0f);
    backTopLeft.normal = vec3(0, 0, 1);
    backTopLeft.texCoord0 = vec2(0.0, 0.0);
    cpuVertexArray.push_back(backTopLeft);
    cpuIndexArray.push_back(0);
    
    Mesh::Vertex backBottomLeft;  // 1
    backBottomLeft.position = vec3(-_ENV_WIDTH/2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomLeft.normal = vec3(0, 0, 1);
    backBottomLeft.texCoord0 = vec2(0.0, _ENV_HEIGHT);
    cpuVertexArray.push_back(backBottomLeft);
    cpuIndexArray.push_back(1);
    
    Mesh::Vertex backTopRight;  // 2
    backTopRight.position = vec3(_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backTopRight.normal = vec3(0, 0, 1);
    backTopRight.texCoord0 = vec2(_ENV_WIDTH, 0.0);
    cpuVertexArray.push_back(backTopRight);
    cpuIndexArray.push_back(2);
    
    cpuIndexArray.push_back(2);  // Dupe backTopRight
    cpuIndexArray.push_back(1);  // Dupe backBottomLeft
    
    Mesh::Vertex backBottomRight;  // 3
    backBottomRight.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomRight.normal = vec3(0, 0, 1);
    backBottomRight.texCoord0 = vec2(_ENV_WIDTH, _ENV_HEIGHT);
    cpuVertexArray.push_back(backBottomRight);
    cpuIndexArray.push_back(3);
    
    Mesh::Vertex backTopRight2;  // 4
    backTopRight2.position = vec3(_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backTopRight2.normal = vec3(-1, 0, 0);
    backTopRight2.texCoord0 = vec2(0, 0);
    cpuVertexArray.push_back(backTopRight2);
    cpuIndexArray.push_back(4);
    
    Mesh::Vertex backBottomRight2;  // 5
    backBottomRight2.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomRight2.normal = vec3(-1, 0, 0);
    backBottomRight2.texCoord0 = vec2(0.0, _ENV_HEIGHT);
    cpuVertexArray.push_back(backBottomRight2);
    cpuIndexArray.push_back(5);
    
    Mesh::Vertex frontTopRight;  // 6
    frontTopRight.position = vec3(_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontTopRight.normal = vec3(-1, 0, 0);
    frontTopRight.texCoord0 = vec2(_ENV_WIDTH, 0.0);
    cpuVertexArray.push_back(frontTopRight);
    cpuIndexArray.push_back(6);
    
    cpuIndexArray.push_back(6);  // Dupe backTopRight
    cpuIndexArray.push_back(5);  // Dupe backBottomLeft
    
    Mesh::Vertex frontBottomRight;  // 7
    frontBottomRight.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomRight.normal = vec3(-1, 0, 0);
    frontBottomRight.texCoord0 = vec2(_ENV_WIDTH, _ENV_HEIGHT);
    cpuVertexArray.push_back(frontBottomRight);
    cpuIndexArray.push_back(7);
    
    Mesh::Vertex frontTopRight2;  // 8
    frontTopRight2.position = vec3(_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontTopRight2.normal = vec3(0, 0, -1);
    frontTopRight2.texCoord0 = vec2(0.0, 0.0);
    cpuVertexArray.push_back(frontTopRight2);
    cpuIndexArray.push_back(8);
    
    Mesh::Vertex frontBottomRight2;  // 9
    frontBottomRight2.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomRight2.normal = vec3(0, 0, -1);
    frontBottomRight2.texCoord0 = vec2(0.0, _ENV_HEIGHT);
    cpuVertexArray.push_back(frontBottomRight2);
    cpuIndexArray.push_back(9);
    
    Mesh::Vertex frontTopLeft;  // 10
    frontTopLeft.position = vec3(-_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontTopLeft.normal = vec3(0, 0, -1);
    frontTopLeft.texCoord0 = vec2(_ENV_WIDTH, 0.0);
    cpuVertexArray.push_back(frontTopLeft);
    cpuIndexArray.push_back(10);
    
    cpuIndexArray.push_back(10);  // Dupe backTopRight
    cpuIndexArray.push_back(9);  // Dupe backBottomLeft
    
    Mesh::Vertex frontBottomLeft;  // 11
    frontBottomLeft.position = vec3(-_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomLeft.normal = vec3(0, 0, -1);
    frontBottomLeft.texCoord0 = vec2(_ENV_WIDTH, _ENV_HEIGHT);
    cpuVertexArray.push_back(frontBottomLeft);
    cpuIndexArray.push_back(11);
    
    Mesh::Vertex frontTopLeft2;  // 12
    frontTopLeft2.position = vec3(-_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontTopLeft2.normal = vec3(1, 0, 0);
    frontTopLeft2.texCoord0 = vec2(0.0, 0.0);
    cpuVertexArray.push_back(frontTopLeft2);
    cpuIndexArray.push_back(12);
    
    Mesh::Vertex frontBottomLeft2;  // 13
    frontBottomLeft2.position = vec3(-_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomLeft2.normal = vec3(1, 0, 0);
    frontBottomLeft2.texCoord0 = vec2(0.0, _ENV_HEIGHT);
    cpuVertexArray.push_back(frontBottomLeft2);
    cpuIndexArray.push_back(13);
    
    Mesh::Vertex backTopLeft2;  // 14
    backTopLeft2.position = vec3(-_ENV_WIDTH /2.0f, _ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backTopLeft2.normal = vec3(1, 0, 0);
    backTopLeft2.texCoord0 = vec2(_ENV_WIDTH, 0.0);
    cpuVertexArray.push_back(backTopLeft2);
    cpuIndexArray.push_back(14);
    
    cpuIndexArray.push_back(14);  // Dupe backTopRight
    cpuIndexArray.push_back(13);  // Dupe backBottomLeft
    
    Mesh::Vertex backBottomLeft2;  // 15
    backBottomLeft2.position = vec3(-_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomLeft2.normal = vec3(1, 0, 0);
    backBottomLeft2.texCoord0 = vec2(_ENV_WIDTH, _ENV_HEIGHT);
    cpuVertexArray.push_back(backBottomLeft2);
    cpuIndexArray.push_back(15);
    
    Mesh::Vertex backBottomLeft3;  // 16
    backBottomLeft3.position = vec3(-_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomLeft3.normal = vec3(0, 1, 0);
    backBottomLeft3.texCoord0 = vec2(0.0, 0.0);
    cpuVertexArray.push_back(backBottomLeft3);
    cpuIndexArray.push_back(16);
    
    Mesh::Vertex frontBottomLeft3;  // 17
    frontBottomLeft3.position = vec3(-_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomLeft3.normal = vec3(0, 1, 0);
    frontBottomLeft3.texCoord0 = vec2(0.0, _ENV_WIDTH);
    cpuVertexArray.push_back(frontBottomLeft3);
    cpuIndexArray.push_back(17);
    
    Mesh::Vertex backBottomRight3;  // 18
    backBottomRight3.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, -_ENV_WIDTH /2.0f);
    backBottomRight3.normal = vec3(0, 1, 0);
    backBottomRight3.texCoord0 = vec2(_ENV_WIDTH, 0.0);
    cpuVertexArray.push_back(backBottomRight3);
    cpuIndexArray.push_back(18);
    
    cpuIndexArray.push_back(18);  // Dupe backTopRight
    cpuIndexArray.push_back(17);  // Dupe backBottomLeft
    
    Mesh::Vertex frontBottomRight3;  // 19
    frontBottomRight3.position = vec3(_ENV_WIDTH /2.0f, -_ENV_HEIGHT /2.0f, _ENV_WIDTH /2.0f);
    frontBottomRight3.normal = vec3(0, 1, 0);
    frontBottomRight3.texCoord0 = vec2(_ENV_WIDTH, _ENV_WIDTH);
    cpuVertexArray.push_back(frontBottomRight3);
    cpuIndexArray.push_back(19);
    
    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

    std::shared_ptr<Texture> tex;
    //tex = Texture::create2DTextureFromFile("C:\\Users\\mackz\\Downloads\\poolWall.jpg");
    tex = Texture::create2DTextureFromFile("/Users/miril/COMP465/comp465-project-ooooo-water-pretty/resources/images/poolTile.jpg");
    tex->setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    tex->setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    textures.push_back(tex);

    _wallsMesh.reset(new Mesh(textures, GL_TRIANGLES, GL_STATIC_DRAW, cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray, cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));

    //_wallsMesh->setMaterialColor(vec4(1.0, 1.0, 1.0, 1.0));
}
