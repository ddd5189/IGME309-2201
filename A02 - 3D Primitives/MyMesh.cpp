#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// cone
	// Replace this with your code
	// create a list of vector3s - the vertices for all the created triangles
	std::vector<vector3> lVertices;
	// create the variable that will rotate the next drawn triangle based on the ammount of sub divisions
	float rotate = ((2 * PI) / a_nSubdivisions);
	// set the start angle to 0
	float angle = 0;
	// divide height by 2 to center the shape
	float height = a_fHeight/2;

	// create triangles and add them to the list of vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// create a new vertex using cos and sin with the current angle and scale by radius 
		vector3 newVertex = vector3(cos(angle) * a_fRadius, -height, sin(angle) * a_fRadius);
		// add the created vertex to the list of vertices
		lVertices.push_back(newVertex);
		// add the rotate value to the angle
		angle += rotate;
	}

	// use the AddTri method to create triangles using the list of vertices for each triangle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// base
		// param1: start with the y at -height so it's centered 
		// param2: use the current vertex from the list of vertices
		// param3: use the next after current vertex from the list, but make sure it never exceeds the amount in the list
		AddTri(vector3(0, -height, 0), lVertices[i], lVertices[(i + 1) % a_nSubdivisions]);
		// sides
		// set the same point for the height, and swap the other two params for the base
		AddTri(vector3(0, height, 0) , lVertices[(i + 1) % a_nSubdivisions], lVertices[i] );
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Cylinder
	std::vector<vector3> lVertices;
	// create the variable that will rotate the next drawn triangle based on the ammount of sub divisions
	float rotate = ((2 * PI) / a_nSubdivisions);
	// set the start angle to 0
	float angle = 0;
	float height = a_fHeight;
	vector3 heightVec = vector3(0, height, 0);

	// create triangles and add them to the list of vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// create a new vertex using cos and sin with the current angle and scale by radius
		// use -height/2 to make it centered 
		vector3 newVertex = vector3(cos(angle) * a_fRadius, -height/2, sin(angle) * a_fRadius);
		// add the created vertex to the list of vertices
		lVertices.push_back(newVertex);
		// add the rotate value to the angle
		angle += rotate;
	}

	// use the AddTri method to create triangles using the list of vertices for each triangle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// base
		// param1: start with the y at -height/2 so it's centered
		// param2: use the current vertex from the list of vertices
		// param3: use the next after current vertex from the list, but make sure it never exceeds the amount in the list
		AddTri(vector3(0, -height/2, 0), lVertices[i], lVertices[(i + 1) % a_nSubdivisions]);
		// base 2, set the y of all the vectors to the requested height, by adding a vector with the height as the y
		AddTri(vector3(0, height/2, 0), lVertices[(i + 1) % a_nSubdivisions] + heightVec, lVertices[i] + heightVec);
		// sides
		// use the vertecies from the above bases to connect them with triangles, using the AddQuad function.
		AddQuad(lVertices[i], lVertices[i] + heightVec, lVertices[(i + 1) % a_nSubdivisions], lVertices[(i + 1) % a_nSubdivisions] + heightVec);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// Tube
	// 2 separate lists for the inner and outer vertices 
	std::vector<vector3> lVerticesInner;
	std::vector<vector3> lVerticesOuter;
	// create the variable that will rotate the next drawn triangle based on the ammount of sub divisions
	float rotate = ((2 * PI) / a_nSubdivisions);
	// set the start angle to 0
	float angle = 0;
	// store the height in an easier var
	float height = a_fHeight;
	// create the height vector used to translate the top 
	vector3 heightVec = vector3(0, height, 0);

	// create triangles (quads) and add them to the list of vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// create a new vertex using cos and sin with the current angle and scale by inner radius 
		// use -height/2 to make it centered
		vector3 newVertex = vector3(cos(angle) * a_fInnerRadius, -height/2, sin(angle) * a_fInnerRadius);
		// add the created vertex to the list of inner vertices
		lVerticesInner.push_back(newVertex);

		// create a new vertex using cos and sin with the current angle and scale by outer radius 
		vector3 newVertex1 = vector3(cos(angle) * a_fOuterRadius, -height / 2, sin(angle) * a_fOuterRadius);
		// add the created vertex to the list of outer vertices
		lVerticesOuter.push_back(newVertex1);

		// add the rotate value to the angle
		angle += rotate;
	}

	// use the AddQuad method to create triangles (quads) using each list of vertices
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// top
		AddQuad(lVerticesOuter[i] + heightVec, lVerticesInner[i] + heightVec, lVerticesOuter[(i + 1) % a_nSubdivisions] + heightVec, lVerticesInner[(i + 1) % a_nSubdivisions] + heightVec);
		// base
		AddQuad(lVerticesInner[(i + 1) % a_nSubdivisions], lVerticesInner[i], lVerticesOuter[(i + 1) % a_nSubdivisions], lVerticesOuter[i]);

		// outer sides
		// use the vertecies from the above bases to connect them with triangles, using the AddQuad function.
		AddQuad(lVerticesOuter[i], lVerticesOuter[i] + heightVec, lVerticesOuter[(i + 1) % a_nSubdivisions], lVerticesOuter[(i + 1) % a_nSubdivisions] + heightVec);
		// inner sides
		// use the vertecies from the above bases to connect them with triangles, using the AddQuad function.
		AddQuad(lVerticesInner[(i + 1) % a_nSubdivisions] + heightVec, lVerticesInner[i] + heightVec, lVerticesInner[(i + 1) % a_nSubdivisions], lVerticesInner[i]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	// Torus
	// list of all vertices
	std::vector<vector3> lVertices;
	// create the variables that will rotate the next drawn triangle (quad) based on the ammount of sub divisions
	float rotateA = ((2 * PI) / a_nSubdivisionsA);
	float rotateB = ((2 * PI) / a_nSubdivisionsB);
	// variable for the Poloidal direction
	float v = 0;
	// variable for the Toroidal direction
	float u = 0;
	// variable for the center of the tube from center of hole
	float c = (a_fOuterRadius + a_fInnerRadius) / 2;
	// variable for radius of the tube
	float a = (a_fOuterRadius - a_fInnerRadius) / 2;
	// helper variable for the total number of vertices 
	float allVertices = a_nSubdivisionsA * a_nSubdivisionsB;

	// a_nSubdivisionsA is how many subdivisions the whole torus has
	// for every torus subdivision find every point around the tube
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		// a_nSubdivisionsB is how many subdivisions the tube has
		for (int i = 0; i < a_nSubdivisionsB; i++)
		{
			// v changes as this is finding every point at a subdivision in the tube
			// u stays the same, making sure each point is at the same subdivision
			float x = (c + a * cos(v)) * cos(u);
			float y = (c + a * cos(v)) * sin(u);
			float z = a * sin(v);
			vector3 newVertex = vector3(x, y, z);

			lVertices.push_back(newVertex);

			v += rotateA;
		}
		// change the torus subdivision for the next set up vertices
		u += rotateB;
	}


	for (int i = 0; i < allVertices; i++)
	{
		// create all the necessary quads making sure that the index is never out of range
		// using allVertices (casting it to an int)
		AddQuad(lVertices[i], lVertices[(i + a_nSubdivisionsB) % (int)allVertices] , lVertices[(i + 1) % (int)allVertices], lVertices[(i + a_nSubdivisionsB + 1) % (int)allVertices]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	// Replace this with your code
	// Sphere
	std::vector<vector3> lVertices;
	// create the variables that will rotate the next drawn triangle (quad) based on the ammount of sub divisions
	// longitude
	float rotateA = (2 * PI) / a_nSubdivisions;
	// latitude
	float rotateB = PI / a_nSubdivisions;
	// set the start angles to 0
	float angleA = 0;
	float angleB = 0;
	// helper var for radius
	float radius = a_fRadius;
	// helper var for the total number of verices
	float allVertices = a_nSubdivisions * a_nSubdivisions;

	// create triangles (quads) and add them to the list of vertices
	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		for (int i = 0; i < a_nSubdivisions; i++)
		{
			// create the points for the sphere
			float x = radius * cos(angleA) * sin(angleB);
			float y = radius * sin(angleA) * sin(angleB);
			float z = radius * cos(angleB);
			// create a new vector3 with the points generate above
			vector3 newVertex = vector3(x, y, z);
			// push it to the list
			lVertices.push_back(newVertex);
			// change angleA
			angleA += rotateA;
		}
		// change angleB
		angleB += rotateB;
	}

	// add an extra point to fill the bottom of the sphere
	lVertices.push_back(vector3(1, 0, 0) * -radius);

	for (int i = 0; i < allVertices; i++)
	{
		// add quads to render the sphere
		AddQuad(
			lVertices[(i + a_nSubdivisions)],
			lVertices[(i + a_nSubdivisions + 1)],
			lVertices[i], 
			lVertices[(i + 1)]);

		// *the shape is a bit odd, and it looks weird in one of the sectors, but it is a complete sphere.*
	}


	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}