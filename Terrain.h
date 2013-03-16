//Terrain.h
//Last Modified 01/15/2013
#pragma once
#include <OpenGL_3_3Engine.h>
#include "QuadTree.h"

using namespace OpenGL_3_3Engine;

const float LANDSCAPE_SCALE = 2.0f;
const float HEIGHT_SCALE = 0.7f;


class Terrain{
	public:
		Terrain(const std::string& name,const std::string& path):m_heightMapHeight(0),m_heightMapWidth(0),
												m_heightMapName(name),m_heightMapPath(path),m_quadRoot(0){};
		~Terrain(){Destroy();}

		bool				Init(ResourceManager<Texture>* texMan,ResourceManager<Shader>* shaderMan,ResourceManager<StaticMesh>* meshMan);
		void				Destroy();
		void				DrawGround();
		void				DrawObjects();
		int					GetVertexCount()const {return m_numVertices;}
		void				CopyVertices(vec3** newVertices,vec3** newNormals,vec2** newTextures);
		void				DrawBoundingRectangles(const mat4x4& persp,const mat4x4& view)
							{m_quadRoot->DrawBoundingRectangles(persp,view);}
	    QuadTree*			GetTree()const{return m_quadRoot;}

	private:

		struct heightMapData{
			vec3 pos;
			vec3 normal;
			vec2 texture;
		};

		
		

		void				InitHeightMap(ResourceManager<Texture>* texMan);
		void				InitNormals();
		void				InitTextureCoordinates();
		void				ReshuffleVertices();

		heightMapData*		m_heightMapData;
		vec3*				m_vertices;
		vec3*				m_normals;
		vec2*				m_textures;
		int					m_numVertices;

		ResourceManager<Texture>*	m_textureManager;
		ResourceManager<Shader>*	m_shaderManager;

		GLint				m_heightMapWidth;
		GLint				m_heightMapHeight;
		std::string			m_heightMapName;
		std::string			m_heightMapPath;

		unsigned long       m_heightMap;
		QuadTree*           m_quadRoot;
};