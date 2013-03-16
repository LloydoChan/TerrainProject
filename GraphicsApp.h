//GraphicsApp.h - the main class that contains all the data for the application - matrices, resource managers etc
//and the main rendering functions too
//
//Graphics app is a singleton class only because of the GLFW callback functions. GLFW callback functions need to be 
//either static member functions or void non-member functions. If access to graphics app's class member variables
//is needed then the only way to access them is to call member functions using a singleton instance from these
//non-member callback functions.
//AUTHOR: LLOYD CRAWLEY
//Date last modified: 31/01/2013

#pragma once

#include <OpenGL_3_3Engine.h>
#include "Terrain.h"
#include "PalmTree.h"

using namespace OpenGL_3_3Engine;

const float FORWARDS_SPEED     = 100.0f;
const float SIDEWAYS_SPEED     = 50.0f;
const float ROTATE_YAW_SPEED   = 80.0f;
const float ROTATE_PITCH_SPEED = 80.0f;
const float CLIMB_SPEED        = 80.0f;

const int APP_WIDTH  = 640;
const int APP_HEIGHT = 480;

const float FOV = 60.0f;
const float NEAR = 0.1f;
const float FAR  = 1400.0f;

class GraphicsApp{
	
	public:
		bool				Init();
		void				MainLoop();
		void				PreRender();
		void				Render();
		void				RenderIsland(bool debug,bool depth,bool reflection);
		void                RenderSea();
		void				Shutdown();
		void				ReadInput(int key,int action);
		void				ReadMouse(int x,int y);
		void				WindowChange(int width,int height);
		void				Update(double time);
		const glm::mat4x4&	GetProjection()const {return m_persp;}
		const glm::mat4x4&	GetViewMatrix()const {return m_viewMatrix;}
		void				SetShadowMatrices();

		static				GraphicsApp* GetInstance();
		static void			DestroyInstance();

	private:
		GraphicsApp();
		~GraphicsApp(){m_timer->DestroyInstance();}

		void InitTextures();
		void InitShaders();
		void InitGeometry();

		static GraphicsApp* m_instance;
		mat4x4				m_persp;
		mat4x4				m_sunMatrix;
		mat4x4				m_viewMatrix;

		mat4x4				m_debugView;
		mat4x4				m_debugPersp;
		
		FrameBuffer 		m_reflectionBuffer;
		FrameBuffer			m_shadowBuffer;
		FrameBuffer			m_atmosphere;
		Camera*				m_cam;
		Terrain*            m_terrain;
		SkyDome*            m_sky;

		unsigned long       m_grassTex;
		unsigned long       m_waterNormalMap;
		unsigned long       m_rockTexture;
		unsigned long       m_sand;
		unsigned long		m_treeTexture;

		unsigned long		m_terrainShader;
		unsigned long		m_seaShader;
		unsigned long		m_treeShader;
		unsigned long		m_palmShader;
		unsigned long		m_skyShader;

		ResourceManager<Texture>*		m_textureManager;
		ResourceManager<Shader>*		m_shaderManager;
		ResourceManager<StaticMesh>*	m_meshManager;
		

		float				m_forwardsAmount;
		float				m_sidewaysAmount;
		float				m_yRotationAmount;
		float				m_xRotationAmount;
		float				m_climbAmount;
		bool				m_triangles;
		bool				m_debug;
		
		Quad*				m_seaQuad;
		Quad*				m_quad;

		int					m_winWidth;
		int					m_winHeight;

		vec4				m_sunVector;
		vec2				m_sunAngle;

		Timer*				m_timer;
};

void GLFWCALL Resize(int width,int height);
void GLFWCALL KeyCallback(int key,int action);
void GLFWCALL MouseCallback(int x,int y);