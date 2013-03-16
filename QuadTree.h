//QuadTree Class - a quad tree to render a landscape. Each node of the quadtree will hold a maximum
//of 10,000 triangles. This will also help frustum culling!

//Last update 02/07/2013

#pragma once
#include <OpenGL_3_3Engine.h>

const int MAX_TRIANGLES = 50000;
const int MAX_TREES     = 150;

class Terrain;
struct QuadTreeNode;
using namespace OpenGL_3_3Engine;

class QuadTree:Uncopyable{
	
	public:
		QuadTree():m_root(NULL),m_vertices(NULL),m_textures(NULL),m_normals(NULL){};
		~QuadTree(){delete m_vertexBuffer;DeleteNode(m_root);delete m_tree;}

		bool		  Init(Terrain* terrain,ResourceManager<Shader>* shaderMan,ResourceManager<Texture>* texMan,
			               ResourceManager<StaticMesh>* meshMan);
		void		  DrawGround(Frustum* frustum);
		void		  DrawTrees(Frustum* frustum);
		int			  GetDrawCount()const {return m_drawCount;}
		vec4	      GetMinAndMax() const;
		void		  DrawBoundingRectangles(const mat4x4& persp,const mat4x4& view);
		
	private:
		
		struct QuadTreeNode{
			vec2 position;
			int triangleCount;
			float size;
			unsigned int* indices;
			QuadTreeNode* nodes[4];
			bool culled;

			vec3* objPositions;
			int numObjs;
		};

		void DrawBoundingRectangles(const mat4x4& persp,const mat4x4& view,QuadTreeNode* node);
		void CalculateMeshDimensions(int vertCount,float* xPos,float* zPos,float* width);
		void CreateTreeNode(QuadTreeNode* node,vec2 pos,int width,int numObjs);
		int  CountTriangles(vec2 pos,float width,std::vector<unsigned int>& vertexList);
		bool IsTriangleInside(int index,vec2 pos,float width);
		void DeleteNode(QuadTreeNode* node);
		void DrawGround(QuadTreeNode* node,Frustum* frust);
		void DrawObjects(QuadTreeNode* node,Frustum* frust);
		

		QuadTreeNode*	m_root;
		vec3*			m_vertices;
		vec2*			m_textures;
		vec3*			m_normals;
		ObjectData*		m_vertexBuffer;
		StaticMesh*     m_tree;
		unsigned long	m_treeShader;

		int				m_width;

		int				m_triangleCount;
		int				m_drawCount;

		int				m_nodeCount;

		ResourceManager<Shader>* m_shaderMan;
};

