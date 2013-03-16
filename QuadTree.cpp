//QuadTree.cpp
#include "QuadTree.h"
#include "Terrain.h"

bool QuadTree::Init(Terrain* terrain,ResourceManager<Shader>* shaderMan,ResourceManager<Texture>* texMan,ResourceManager<StaticMesh>* meshMan){


	float centerX,centerZ,width;

	m_nodeCount = 0;

	int vertexCount = terrain->GetVertexCount();

	m_triangleCount = vertexCount/3;

	terrain->CopyVertices(&m_vertices,&m_normals,&m_textures);

	CalculateMeshDimensions(vertexCount,&centerX,&centerZ,&width);

	m_width = width;

	m_root = new QuadTreeNode();

	double StartTime = glfwGetTime();

	CreateTreeNode(m_root,vec2(0.0f,0.0f),width,MAX_TREES);

	std::cout<<"time taken: "<<(glfwGetTime() - StartTime)<<std::endl;

	m_vertexBuffer = new ObjectData(3,m_triangleCount);
	m_vertexBuffer->Init(NULL,vertexCount,(float*)m_vertices,(float*)m_normals,(float*)m_textures);

	unsigned int meshHandle = meshMan->Add("palm_lod0.obj","tree\\");

	m_tree = meshMan->GetElement(meshHandle);
	m_tree->Init(texMan,shaderMan);

	m_treeShader = shaderMan->Add("TexturedMeshShader","shaders\\");

	m_shaderMan  = shaderMan;

	return true;
}

void QuadTree::DrawGround(Frustum* frust){

	m_drawCount = 0;

	glBindVertexArray(m_vertexBuffer->GetVAO());
		DrawGround(m_root,frust);
	glBindVertexArray(0);
}

void QuadTree::DrawTrees(Frustum* frust){

	
	  DrawObjects(m_root,frust);

}

vec4   QuadTree::GetMinAndMax()const{
	vec2 min = m_root->position - m_root->size;
	vec2 max = m_root->position + m_root->size;
	return vec4(min,max);
}

void QuadTree::CalculateMeshDimensions(int vertCount,float* centerX,float* centerZ,float* meshWidth){


	//init mesh center pos to 0

	*centerX = 0.0f;
	*centerZ = 0.0f;

	for(int vertex = 0; vertex < vertCount; vertex++){
		*centerX += m_vertices[vertex].x;
		*centerZ += m_vertices[vertex].z;
	}

	*centerX /= (float)vertCount;
	*centerZ /= (float)vertCount;

	float minWidth = fabsf(m_vertices[0].x - *centerX);
	float minDepth = fabsf(m_vertices[0].z - *centerZ);

	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float width,depth;

	for(int vertex = 0; vertex < vertCount; vertex++){
		width = fabsf(m_vertices[vertex].x - *centerX);
		depth = fabsf(m_vertices[vertex].z - *centerZ);

		if(width > maxWidth) maxWidth = width;
		if(width < minWidth) minWidth = width;
		if(depth > maxDepth) maxDepth = depth;
		if(depth < minDepth) minDepth = depth;
	}


	float maxX = (float)glm::max(fabs(minWidth),fabs(maxWidth));
	float maxZ = (float)glm::max(fabs(minDepth),fabs(maxDepth));

	*meshWidth = glm::max(maxX,maxZ) * 2.0f;
}

void QuadTree::CreateTreeNode(QuadTreeNode* node,vec2 pos,int width,int numObjs){
  
	float offsetX,offsetZ;

	node->position.x = pos.x;
	node->position.y = pos.y;
	node->size       = width;
	node->culled     = false;

	node->triangleCount = 0;
	node->indices    = NULL;

	node->objPositions = NULL;
	node->numObjs	   = MAX_TREES;

	for(int child = 0; child < 4; child++)
	  node->nodes[child] = NULL;

	std::vector<unsigned int> triList;


	int numTriangles = CountTriangles(pos,width,triList);

	if(numTriangles == 0) return;


	if(numTriangles > MAX_TRIANGLES){

	  for(int child = 0; child < 4; child++){
		offsetX = (((child % 2) < 1) ? -1.0f : 1.0f) * (width/4.0f);
		offsetZ = (((child % 4) < 2) ? -1.0f : 1.0f) * (width/4.0f);

		node->nodes[child] = new QuadTreeNode();
		CreateTreeNode(node->nodes[child],vec2(pos.x + offsetX,pos.y + offsetZ),width/2,numObjs/2); 
	  }

	  return;
	}

	node->triangleCount = numTriangles;

	int vertexCount = numTriangles * 3;

	node->indices = new unsigned int[vertexCount];

	int index = 0;

	int numTris = triList.size();

	double startTime2 = glfwGetTime();

	for(int triangle = 0; triangle < numTris; triangle++){

		  int vertIndex = triList[triangle] * 3;

		  node->indices[index++]  = vertIndex++;
		  node->indices[index++]  = vertIndex++;
		  node->indices[index++]  = vertIndex++;
		
	}


	node->objPositions = new vec3[numObjs];
	node->numObjs = numObjs;

	//generate tree coords
	for(int obj = 0; obj < numObjs; obj++){
		int index = (rand() << 3) % vertexCount;

		float slope = 1.0f - m_normals[node->indices[index]].y;

		if(slope > 0.7f) continue;

		node->objPositions[obj] = m_vertices[node->indices[index]];
	}

}

bool QuadTree::IsTriangleInside(int index,vec2 pos,float width){
  
	float radius = width/2.0f;

	int vertexIndex = index * 3;

	float x1 = m_vertices[vertexIndex].x;
	float z1 = m_vertices[vertexIndex++].z;

	float x2 = m_vertices[vertexIndex].x;
	float z2 = m_vertices[vertexIndex++].z;

	float x3 = m_vertices[vertexIndex].x;
	float z3 = m_vertices[vertexIndex].z;

	float minX = glm::min(x1,glm::min(x2,x3));
	float maxX = glm::max(x1,glm::max(x2,x3));

	float minZ = glm::min(z1,glm::min(z2,z3));
	float maxZ = glm::max(z1,glm::max(z2,z3));
	
	if(minX > pos.x + radius) return false;
	if(maxX < pos.x - radius) return false;

	if(minZ > pos.y + radius) return false;
	if(maxZ < pos.y - radius) return false;

	return true;
}

int QuadTree::CountTriangles(vec2 pos,float width,std::vector<unsigned int>& triList){

	int count = 0;

	triList.reserve(m_triangleCount);
	
	for(int triangle = 0; triangle < m_triangleCount; triangle++){
		
		if(IsTriangleInside(triangle,pos,width)){
			count++;
			triList.push_back(triangle);
		}
	}

	return count;
}



void QuadTree::DrawGround(QuadTreeNode* node,Frustum* frust){
  
	vec3 testVec(node->position.x,0.0f,node->position.y);
	if(!frust->CheckRect(testVec,node->size/2.0f)){
		node->culled = true;
		return;
	}

	node->culled = false;

	int count = 0;
	for(int child = 0; child < 4; child++){
		if(node->nodes[child] != 0){
		  count++;
		  DrawGround(node->nodes[child],frust);
		}
	}

	if(count!=0)
		return;

	if(node->indices){
		int numIndices = node->triangleCount * 3;
		glDrawElements(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT,node->indices);
		m_drawCount += node->triangleCount;
	}
}

void QuadTree::DrawObjects(QuadTreeNode* node,Frustum* frust){
  
	vec3 testVec(node->position.x,0.0f,node->position.y);
	if(!frust->CheckRect(testVec,node->size/2.0f)){
		node->culled = true;
		return;
	}

	node->culled = false;

	int count = 0;
	for(int child = 0; child < 4; child++){
		if(node->nodes[child] != 0){
		  count++;
		  DrawObjects(node->nodes[child],frust);
		}
	}

	if(count!=0)
		return;


	if(node->indices){

		//get shader
		Shader* treeShader = m_shaderMan->GetElement(m_treeShader);
		for(int obj = 0; obj < node->numObjs; obj++){
			if(node->objPositions[obj].y < 20.0f) continue;
			if(frust->CheckRect(node->objPositions[obj],5.0f)){
				treeShader->SetUniform("translation",glm::translate(node->objPositions[obj]));
				m_tree->Render();
			}
		}
	}
}

void QuadTree::DrawBoundingRectangles(const mat4x4& persp,const mat4x4& view){
	DrawBoundingRectangles(persp,view,m_root);
}

void QuadTree::DrawBoundingRectangles(const mat4x4& persp,const mat4x4& view,QuadTreeNode* node){

	if(!node) return;

	if(node->culled){
		//render
		float posX = node->position.x;
		float posZ = node->position.y;
		float width = node->size/2.0f;

		vec4 points[4];

		points[0] = vec4(posX-width,300.0f,posZ-width,1.0f);
		points[1] = vec4(posX+width,300.0f,posZ-width,1.0f);
		points[2] = vec4(posX+width,300.0f,posZ+width,1.0f);
		points[3] = vec4(posX-width,300.0f,posZ+width,1.0f);

		for(int point = 0; point < 4; point++)
			points[point] = persp * view * vec4(points[point]);

		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINE_LOOP);
		  glVertex3fv((GLfloat*)points);
		  glVertex3fv((GLfloat*)&points[1]);

		  glVertex3fv((GLfloat*)&points[2]);

		  glVertex3fv((GLfloat*)&points[3]);

		glEnd();
		return;
	}
	
	if((!node->nodes[0])) 
		return;

	for(int child = 0; child < 4; child++){
		DrawBoundingRectangles(persp,view,node->nodes[child]);
	}
}

void QuadTree::DeleteNode(QuadTreeNode* node){
	
	if(!node) return;

	if(node->nodes[0]){
		for(int child = 0; child < 4; child++){
			DeleteNode(node->nodes[child]);
		}
	}

	delete [] node->indices;
	delete [] node->objPositions;
}