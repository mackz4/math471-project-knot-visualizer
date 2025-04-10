#ifndef CAUSTICS_APP_H // Changed from CAUSTICS-APP_H
#define CAUSTICS_APP_H // Changed from CAUSTICS-APP_H

#include <api/MinVR.h>
using namespace MinVR;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <BasicGraphics.h>
using namespace basicgraphics;

class App : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
	App(int argc, char** argv);
    virtual ~App();

    
    /** USER INTERFACE CALLBACKS **/
    virtual void onAnalogChange(const VRAnalogEvent &state);
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
	virtual void onCursorMove(const VRCursorEvent &state);
    virtual void onTrackerMove(const VRTrackerEvent &state);
    
    
    /** RENDERING CALLBACKS **/
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    
    
private:
    
    const float _ENV_SIZE = 10.0; // orig _ENV_WIDTH
    const float _TILE_SIZE = 0.5f; // orig _TILE_WIDTH

    const float _CAMERA_RADIUS = 30.0f;

    std::unique_ptr<basicgraphics::Mesh> _waterMesh;
    std::unique_ptr<basicgraphics::Mesh> _wallsMesh;
    std::unique_ptr<basicgraphics::Mesh> _floorMesh;

	double _lastTime;
	double _curFrameTime;

    bool mouseDown; // Signifies whether the left mouse button is currently held down.
    float currTheta;
    float currPhi;
    vec3 angleToSpherePoint(float theta, float phi);
    void initWaterMesh(string waveType);
    void initWallsMesh();
    void initFloorMesh();
    void simpleZWater(std::vector<Mesh::Vertex> *cpuVertexArray, std::vector<int> *cpuIndexArray);

    glm::vec2 lastMousePos;
    glm::vec3 eye_world;
    glm::vec3 up_vector;

	virtual void reloadShaders();
	GLSLProgram _shader;

	void initializeText();
	void drawText(const std::string text, float xPos, float yPos, GLfloat windowHeight, GLfloat windowWidth);
	struct FONScontext* fs;
	GLSLProgram _textShader;
};


#endif //EXAMPLEAPP_H
