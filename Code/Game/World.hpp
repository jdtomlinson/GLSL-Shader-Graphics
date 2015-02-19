#ifndef include_World
#define include_World
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Light.hpp"
#include "GameCommon.hpp"
#include "../Engine/Mouse.hpp"
#include "../Engine/Camera.hpp"
#include "../Engine/Vertex.hpp"
#include "../Engine/pugixml.hpp"
#include "../Engine/Texture.hpp"
#include "../Engine/Vector2.hpp"
#include "../Engine/Keyboard.hpp"
#include "../Engine/Material.hpp"
#include "../Engine/BitmapFont.hpp"
#include "../Engine/DebugGraphics.hpp"
#include "../Engine/OpenGLRenderer.hpp"
#include "../Engine/ConsoleCommandArgs.hpp"


//-----------------------------------------------------------------------------------------------
const int SPHERE_NUM_SLICES = 10;
const int SPHERE_NUM_STACKS = 10;
const float SPHERE_RADIUS = 1.f;
const float FIELD_OF_VIEW_Y = 45.f;
const float MOVE_SPEED_POINTS_PER_SECOND = 5.f;
const float ROTATION_DEGREES_PER_SECOND = 70.f;
const float CUBE_X_AXIS_HALF_LENGTH = 1.f;
const float CUBE_Y_AXIS_HALF_LENGTH = 1.f;
const float CUBE_Z_AXIS_HALF_LENGTH = 1.f;
const float SHADER_FOG_START_DISTANCE = 10.f;
const float SHADER_FOG_END_DISTANCE = 20.f;
const float MONITOR_POSITION_X = 5.f;
const float INITIAL_BLUR_SCALE = 5.f;
const Color SHADER_FOG_COLOR = Color( 0.f, 0.2f, 0.4f );
const Vector3 CUBE_CENTER_POSITION = Vector3( 0.f, 2.f, -1.f );
const Vector3 SPHERE_CENTER_POSITION = Vector3( 0.f, -2.f, -1.f );
const std::string SIMPLE_TEXTURE_VERTEX_SHADER_STRING = "Data/Shaders/Simple_Texture_330.vertex.glsl";
const std::string SIMPLE_TEXTURE_FRAGMENT_SHADER_STRING = "Data/Shaders/Simple_Texture_330.fragment.glsl";
const std::string SIMPLE_NO_TEXTURE_VERTEX_SHADER_STRING = "Data/Shaders/Simple_No_Texture_330.vertex.glsl";
const std::string SIMPLE_NO_TEXTURE_FRAGMENT_SHADER_STRING = "Data/Shaders/Simple_No_Texture_330.fragment.glsl";
const std::string GRAYSCALE_VERTEX_SHADER_STRING = "Data/Shaders/Grayscale_330.vertex.glsl";
const std::string GRAYSCALE_FRAGMENT_SHADER_STRING = "Data/Shaders/Grayscale_330.fragment.glsl";
const std::string NORMAL_MAP_VERTEX_SHADER_STRING = "Data/Shaders/Normal_Map_330.vertex.glsl";
const std::string NORMAL_MAP_FRAGMENT_SHADER_STRING = "Data/Shaders/Normal_Map_330.fragment.glsl";
const std::string TOON_VERTEX_SHADER_STRING = "Data/Shaders/Toon_330.vertex.glsl";
const std::string TOON_FRAGMENT_SHADER_STRING = "Data/Shaders/Toon_330.fragment.glsl";
const std::string FBO_VERTEX_SHADER_STRING = "Data/Shaders/FBO_330.vertex.glsl";
const std::string FBO_FRAGMENT_SHADER_STRING = "Data/Shaders/FBO_330.fragment.glsl";
const std::string BLUR_FBO_VERTEX_SHADER_STRING = "Data/Shaders/Blur_FBO_330.vertex.glsl";
const std::string BLUR_FBO_FRAGMENT_SHADER_STRING = "Data/Shaders/Blur_FBO_330.fragment.glsl";
const std::string MONITOR_FBO_VERTEX_SHADER_STRING = "Data/Shaders/Monitor_FBO_330.vertex.glsl";
const std::string MONITOR_FBO_FRAGMENT_SHADER_STRING = "Data/Shaders/Monitor_FBO_330.fragment.glsl";


//-----------------------------------------------------------------------------------------------
bool ConsoleFunctionChangeShader( const ConsoleCommandArgs& args );
bool ConsoleFunctionChangeFBO( const ConsoleCommandArgs& args );
bool ConsoleFunctionChangeBlurScale( const ConsoleCommandArgs& args );


//-----------------------------------------------------------------------------------------------
class World
{
public:
	World( float worldWith, float worldHeight );
	Material* InitalizeFBOAndGetMaterial( unsigned int* FBOid, const std::string& vertexShader, const std::string& fragmentShader );
	void Initialize();
	void Deconstruct();
	bool ProcessKeyDownEvent( unsigned char virtualKeyCode );
	bool ProcessKeyUpEvent( unsigned char virtualKeyCode );
	bool ProcessCharDownEvent( unsigned char charCode );
	void SetCameraPositionAndOrientation( const Camera& camera );
	void CircleTable( float** sint, float** cost, const int n );
	void SetCubeVertices();
	void SetSphereVertices();
	void SetScreenVertices();
	void SetMonitorVertices();
	void SetVertices();
	void AddLight( const Vector3& position, const Vector3& direction, const Color& colorAndBrightness, float innerRadius, float outerRadius, float innerApertureDot, float outerApertureDot, float fractionAmbient, bool isPositionless );
	void SetLightsInShader();
	void SetSimpleLightScene();
	void SetPointLightsScene();
	void SetSpotlightsScene();
	void SetAmbientLightsScene();
	void SetGlobalDirectionalLightScene();
	void SetAllLightsScene();
	void UpdateCameraPositionFromInput( float deltaSeconds );
	void UpdateCameraOrientationFromInput( float deltaSeconds );
	void UpdateFromInput( float deltaSeconds );
	void UpdateLights();
	void Update();
	void BindMonitorToFramebuffer();
	void RenderLights();
	void RenderMonitor();
	void RenderFBO();
	void RenderObjects3D();
	void RenderObjects2D();
	void Render();

	Camera					m_camera;
	Camera					m_monitorView;
	Mouse					m_mouse;
	Keyboard				m_keyboard;
	Vector2					m_size;
	Texture*				m_diffuseTexture;
	Texture*				m_normalTexture;
	Texture*				m_specularTexture;
	Texture*				m_emissiveTexture;
	Texture*				m_depthTexture;
	Texture*				m_obamaTexture;
	std::vector<Vertex>		m_triangleVertices;
	unsigned int			m_VBOid;
	BitmapFont				m_font;
	OpenGLRenderer			m_glRenderer;
	float					m_lightPointLifeSeconds;
	Material*				m_currentMaterial;
	Material*				m_simpleTextureMaterial;
	Material*				m_simpleNoTextureMaterial;
	Material*				m_grayscaleMaterial;
	Material*				m_normalMapMaterial;
	Material*				m_toonMaterial;
	std::vector<Light>		m_lights;

	Material*				m_currentFBOMaterial;
	unsigned int			m_currentFBOid;

	unsigned int			m_screenVBOid;
	std::vector<Vertex>		m_screenVertices;
	unsigned int			m_FBOid;
	Material*				m_FBOMaterial;

	unsigned int			m_blurFBOid;
	Material*				m_blurFBOMaterial;
	float					m_blurScale;

	unsigned int			m_monitorVBOid;
	std::vector<Vertex>		m_monitorVertices;
	unsigned int			m_monitorFBOid;
	Material*				m_monitorFBOMaterial;
};


#endif // include_World