//PalmTree.h - container class for a palmTreeMesh
#pragma once

#include "GraphicsApp.h"
using namespace OpenGL_3_3Engine;

using glm::mat4x4;

class PalmTree{
	public:
		PalmTree():m_treeMesh(NULL),m_texturedShader(NULL),m_texture(NULL){};
		~PalmTree();

		bool			Init();
		void			Render();
	private:
		StaticMesh*		m_treeMesh;
		Shader*			m_texturedShader;
		Texture*		m_texture;
};