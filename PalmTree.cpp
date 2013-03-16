		//PalmTree.cpp
#include "PalmTree.h"

PalmTree::~PalmTree()
{
	if(m_texturedShader){
		delete m_texturedShader;
		m_texturedShader = NULL;
	}

	if(m_treeMesh){
		delete m_treeMesh;
		m_treeMesh = NULL;
	}

	if(m_texture){
		delete m_texture;
		m_texture = NULL;
	}


}

bool PalmTree::Init(int numPalmTrees,const vec3* vertices)
{

	m_treeMesh = new StaticMesh();
	m_treeMesh->Init("palm_lod0.obj","tree");
	m_texturedShader = ResourceManager::GetInstance()->GetShaderRecord("shaders\\TexturedMeshShader");
	//m_texturedShader->InitShaderProgramFromFile("shaders\\TexturedMeshShader.vp","shaders\\TexturedMeshShader.fp",3,"inCoords","inNormal","inTexCoords");
	glUseProgram(m_texturedShader->GetHandle());
		m_texturedShader->SetUniform("texScaleWidth",1.0f/512.0f);
		m_texturedShader->SetUniform("texScaleHeight",1.0f/512.0f);
		m_texturedShader->SetUniform("textureOffset",512.0f/2.0f);
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	m_texture = ResourceManager::GetInstance()->GetTextureRecord("textures\\palm_texture.tga",true);


	return true;
}

void PalmTree::Render()
{
	m_texture->Bind(GL_TEXTURE1);


	//glUseProgram(m_texturedShader->GetHandle())
	m_texturedShader->SetUniform("texture",1);
		

	
	m_texturedShader->SetUniform("translation"));
	m_treeMesh->Render();
		
	//glUseProgram(0);
	m_texture->Unbind(GL_TEXTURE1);
	m_heightMap->Unbind(GL_TEXTURE2);
	
}