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

#include "Skybox.h"
#include "WaterShallow.h"
#include <BasicGraphics.h>
#include <memory>

// CONSTANTS
#define ENV_TILES_X 50
#define ENV_TILES_Z 50
#define ENV_TILE_LEN_X 2.0f
#define ENV_TILE_LEN_Z 2.0f
#define ENV_WIDTH_X (ENV_TILES_X * ENV_TILE_LEN_X)
#define ENV_WIDTH_Z (ENV_TILES_Z * ENV_TILE_LEN_Z)

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
    const float _ENV_WIDTH = 24.0; // orig _ENV_WIDTH
    const float _ENV_HEIGHT = 14.0;
    const float _TILE_SIZE = 0.5f; // orig _TILE_WIDTH
    const float KNOT_RADIUS = 1.0f;
    const int   KNOT_STACKS = 10;
    const int   KNOT_SLICES = KNOT_STACKS * 2;
    const float KNOT_VERTEX_DIST_MIN = 1.0f;

    const glm::vec3 _LIGHT_DIRECTION = glm::vec3(0.0f, -1.0f, 0.0f);

    float _CAMERA_RADIUS = 25.0f;
    const float _CAMERA_SENSITIVITY = 0.15f;
   
    std::shared_ptr<basicgraphics::Texture> environmentMap;
    std::shared_ptr<basicgraphics::Skybox> skyBox;

    std::unique_ptr<basicgraphics::Mesh> _waterMesh;    

	double _lastTime;
	double _curFrameTime;

    bool mouseDown; // Signifies whether the left mouse button is currently held down.
    bool mouseRightDown;
    float mousePosX;
    float mousePosY;
    float currTheta;
    float currPhi;
    glm::vec3 angleToSpherePoint(float theta, float phi);
    void addGeometryEdge();
    void addGeometryGuide(vec3 knot_vertex_prev, vec3 knot_vertex_curr, float knot_vertex_distance);
    void updateLastEdge(float vert_displacement_y);

    glm::vec2 lastMousePos;
    glm::vec3 eye_world;
    glm::vec3 up_vector;
    glm::vec4 lightPosition;
    glm::vec3 materialColor = vec3(1.0, 1.0, 1.0);

	virtual void reloadShaders();
	GLSLProgram _shader;
    GLSLProgram _environment_shader;
    std::shared_ptr<Texture> _tex;

	void initializeText();
	void drawText(const std::string text, float xPos, float yPos, GLfloat windowHeight, GLfloat windowWidth);
	struct FONScontext* fs;
	GLSLProgram _textShader;

    //KnotMeshVars
    bool is_painting = true;

    std::vector<std::unique_ptr<basicgraphics::Mesh>> knot_edge_meshes;
    std::vector<glm::vec3> knot_vertices;

    std::unique_ptr<basicgraphics::Mesh> knot_guide_mesh;
    glm::vec3 knot_guide_vertex;
    glm::vec3 knot_guide_color;

    int knot_vertex_stack_count = 0;
    int knot_vertex_count = 0;

    std::vector<glm::vec3> knot_guide_vertices;
    std::vector <glm::vec3> last_knot_vertices;
    //EndKnotMeshVars
};


#endif //EXAMPLEAPP_H
