	//Terrain.cpp
#include "Terrain.h"


using glm::vec2;


bool Terrain::Init(ResourceManager<Texture>* texMan,ResourceManager<Shader>* shaderMan,ResourceManager<StaticMesh>* meshMan){
	
	InitHeightMap(texMan);
	InitNormals();
	InitTextureCoordinates();
	ReshuffleVertices();

	m_quadRoot = new QuadTree();
	m_quadRoot->Init(this,shaderMan,texMan,meshMan);

	delete [] m_vertices;
	delete [] m_normals;
	delete [] m_textures;
	
	return true;
}

void  Terrain::InitHeightMap(ResourceManager<Texture>* textureManager){
	//load the texture for the heightmap
	m_heightMap = textureManager->Add("newHeight1.bmp","textures\\");

	Texture* tex = textureManager->GetElement(m_heightMap);

	//get info for width,height

	glBindTexture(GL_TEXTURE_2D,tex->GetHandle());


	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&m_heightMapWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&m_heightMapHeight);

	m_numVertices   = m_heightMapHeight * m_heightMapWidth;

	GLubyte* data = new GLubyte[m_numVertices];

	glGetTexImage(GL_TEXTURE_2D,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,data);

	glBindTexture(GL_TEXTURE_2D,0);


	m_heightMapData = new heightMapData[m_numVertices];

	int halfWidth  = m_heightMapWidth/2;
	int halfHeight = m_heightMapHeight/2;


	int index = 0;
	//create a buffer of vertex info using the height map
	for(int x = 0; x < m_heightMapHeight; x++){
		for(int z = 0; z < m_heightMapWidth; z++){
			index = x * m_heightMapWidth + z;
			m_heightMapData[index].pos.x = (x - halfWidth) * LANDSCAPE_SCALE; 
			m_heightMapData[index].pos.y = (float)data[index] * HEIGHT_SCALE;
			m_heightMapData[index].pos.z = (z - halfHeight) * LANDSCAPE_SCALE; 
		}
	}

}

void Terrain::InitNormals(){

	for(int z = 0; z < m_heightMapHeight; z++){
		for(int x = 0; x < m_heightMapWidth; x++){
			int index = z * m_heightMapWidth + x;
			vec3 vec1,vec2;

			if(x < m_heightMapWidth -1){
				vec1 = m_heightMapData[index+1].pos - m_heightMapData[index].pos;
			}else{
				vec1 = m_heightMapData[index].pos - m_heightMapData[index-1].pos;
			}

			
			if(z < m_heightMapHeight -1){
				vec2 = m_heightMapData[index+m_heightMapHeight].pos - m_heightMapData[index].pos;
			}else{
				vec2 = m_heightMapData[index].pos - m_heightMapData[index-m_heightMapHeight].pos;
			}

			m_heightMapData[index].normal = glm::normalize(glm::cross(vec1,vec2));
		}
	}
}

void Terrain::InitTextureCoordinates(){


	//create a buffer of texture info using the height map
	for(int z = 0; z < m_heightMapHeight; z++){
		for(int x = 0; x < m_heightMapWidth; x++){
			int index = z * m_heightMapWidth + x;
			m_heightMapData[index].texture.x = x * 0.025f;
			m_heightMapData[index].texture.y = z * 0.025f;
		}
	}

}


void  Terrain::CopyVertices(vec3** newVertices,vec3** newNormals,vec2** newTextures){
	*newVertices = m_vertices;
	*newNormals  = m_normals;
	*newTextures = m_textures;
}

void Terrain::ReshuffleVertices(){
	int index,x,y,index1,index2,index3,index4;
	float u,v;

	m_numVertices = (m_heightMapWidth) * (m_heightMapHeight) * 6; 

	m_vertices = new vec3[m_numVertices];
	m_textures = new vec2[m_numVertices];
	m_normals  = new vec3[m_numVertices];

	index = 0;

	for(int y = 0; y < m_heightMapHeight-1; y++){
		for(int x =0; x < m_heightMapWidth-1;x++){
			index1 = (m_heightMapHeight * y) + x;
			index2 = (m_heightMapHeight * y) + x + 1;
			index3 = (m_heightMapHeight * (y+1)) + x;
			index4 = (m_heightMapHeight * (y+1)) + x + 1;


			m_vertices[index]   = m_heightMapData[index1].pos;
			m_textures[index]   = m_heightMapData[index1].texture;
			m_normals[index++]  = m_heightMapData[index1].normal;

			m_vertices[index]   = m_heightMapData[index4].pos;
			m_textures[index]   = m_heightMapData[index4].texture;
			m_normals[index++]  = m_heightMapData[index4].normal;

			m_vertices[index]   = m_heightMapData[index3].pos;
			m_textures[index]   = m_heightMapData[index3].texture;
			m_normals[index++]  = m_heightMapData[index3].normal;

			m_vertices[index]   = m_heightMapData[index2].pos;
			m_textures[index]   = m_heightMapData[index2].texture;
			m_normals[index++]  = m_heightMapData[index2].normal;

			m_vertices[index]   = m_heightMapData[index4].pos;
			m_textures[index]   = m_heightMapData[index4].texture;
			m_normals[index++]  = m_heightMapData[index4].normal;

			m_vertices[index]   = m_heightMapData[index1].pos;
			m_textures[index]   = m_heightMapData[index1].texture;
			m_normals[index++]  = m_heightMapData[index1].normal;
		}
	}
}


void  Terrain::DrawGround(){

	m_quadRoot->DrawGround(Frustum::GetInstance());
		
}

void  Terrain::DrawObjects(){
	m_quadRoot->DrawTrees(Frustum::GetInstance());
}

void Terrain::Destroy(){
	m_heightMapHeight = 0;
	m_heightMapWidth  = 0;    


	if(m_quadRoot){
		delete m_quadRoot;
	}


	//glDeleteTextures(1,&texHandle);
}