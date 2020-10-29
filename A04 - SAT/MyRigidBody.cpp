#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false; // change to false
	m_bVisibleOBB = true;
	m_bVisibleARBB = false; // change to false

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	Your code goes here instead of this comment;

	For this method, if there is an axis that separates the two objects
	then the return will be different than 0; 1 for any separating axis
	is ok if you are not going for the extra credit, if you could not
	find a separating axis you need to return 0, there is an enum in
	Simplex that might help you [eSATResults] feel free to use it.
	(eSATResults::SAT_NONE has a value of 0)
	*/

	/*
	*All the tests that need to be done*
	Plane parallel to face normals of object A
	Plane parallel to face normals of object B
	Plane parallel to the vectors resulting from the cross products of all edges in A with all edges in B
	*/
	
	//this->GetCenterGlobal();
	//this->GetMaxLocal();
	//this->GetMinGlobal();
	//this->GetHalfWidth();

#pragma region store corners
	// const for number of corners 
	const uint numCorners = 8;
	// vector to store the normals
	std::vector<vector3> normals;
	//Calculate the 8 corners of this box
	vector3 v3ThisCorner[numCorners];
	//Back square
	v3ThisCorner[0] = m_v3MinL;
	v3ThisCorner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3ThisCorner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3ThisCorner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);
	//Front square
	v3ThisCorner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3ThisCorner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3ThisCorner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3ThisCorner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < numCorners; ++uIndex)
	{
		v3ThisCorner[uIndex] = vector3(m_m4ToWorld * vector4(v3ThisCorner[uIndex], 1.0f));
	}

	//Calculate the 8 corners of the other box
	vector3 v3OtherCorner[numCorners];
	// min and max of other 
	vector3 otherMax = a_pOther->GetMaxLocal();
	vector3 otherMin = a_pOther->GetMinLocal();
	// other model matrix
	matrix4 otherModelMatrix = a_pOther->GetModelMatrix();
	//otherModelMatrix = matrix4(0.0f);
	//otherModelMatrix = IDENTITY_M4;
	// multiply each by the model matrix so it's in the proper orientation
	//Back square
	v3OtherCorner[0] = otherModelMatrix * vector4(otherMin, 1.0f);
	v3OtherCorner[1] = otherModelMatrix * vector4(otherMax.x, otherMin.y, otherMin.z, 1.0f);
	v3OtherCorner[2] = otherModelMatrix * vector4(otherMin.x, otherMax.y, otherMin.z, 1.0f);
	v3OtherCorner[3] = otherModelMatrix * vector4(otherMax.x, otherMax.y, otherMin.z, 1.0f);
	//Front square
	v3OtherCorner[4] = otherModelMatrix * vector4(otherMin.x, otherMin.y, otherMax.z, 1.0f);
	v3OtherCorner[5] = otherModelMatrix * vector4(otherMax.x, otherMin.y, otherMax.z, 1.0f);
	v3OtherCorner[6] = otherModelMatrix * vector4(otherMin.x, otherMax.y, otherMax.z, 1.0f);
	v3OtherCorner[7] = otherModelMatrix * vector4(otherMax, 1.0f);

	//Place them in world space
	//for (uint uIndex = 0; uIndex < numCorners; ++uIndex)
	//{
	//	v3OtherCorner[uIndex] = vector3(m_m4ToWorld * vector4(v3OtherCorner[uIndex], 1.0f));
	//}
#pragma endregion

#pragma region axes
	std::vector<vector3> thisAxes;
	std::vector<vector3> otherAxes;

	vector3 thisX = v3ThisCorner[1] - v3ThisCorner[0];	// find the x axis of the cude
	vector3 thisXAxis = glm::normalize(thisX);			// normalize the x axis vector
	thisAxes.push_back(thisXAxis);
	
	vector3 thisY = v3ThisCorner[2] - v3ThisCorner[0];	// find the y axis of the cube
	vector3 thisYAxis = glm::normalize(thisY);			// normalize the y axis vector
	thisAxes.push_back(thisYAxis);
	
	vector3 thisZ = v3ThisCorner[4] - v3ThisCorner[0];	// find the z axis of the cube
	vector3 thisZAxis = glm::normalize(thisZ);			// normalize the z axis vector
	thisAxes.push_back(thisZAxis);
	
	// push them to the normals vector
	normals.push_back(thisXAxis); //1
	normals.push_back(thisYAxis); //2
	normals.push_back(thisZAxis); //3

	vector3 otherX = v3OtherCorner[1] - v3OtherCorner[0];	// find the x axis of the cude
	vector3 otherXAxis = glm::normalize(otherX);			// normalize the x axis vector
	otherAxes.push_back(otherXAxis);
	
	vector3 otherY = v3OtherCorner[2] - v3OtherCorner[0];	// find the y axis of the cube
	vector3 otherYAxis = glm::normalize(otherY);			// normalize the y axis vector
	otherAxes.push_back(otherYAxis);
	
	vector3 otherZ = v3OtherCorner[4] - v3OtherCorner[0];	// find the z axis of the cube
	vector3 otherZAxis = glm::normalize(otherZ);			// normalize the z axis vector
	otherAxes.push_back(otherZAxis);
	
	// push them to the normals vector
	normals.push_back(otherXAxis); //4
	normals.push_back(otherYAxis); //5
	normals.push_back(otherZAxis); //6

#pragma endregion

#pragma region cross products of axes
	// L = Ax * Bx
	vector3 toX = glm::cross(thisX, otherX);		// find the cross product
	if (toX != ZERO_V3) toX = glm::normalize(toX);	// normalize it as long as it isn't = 0
	normals.push_back(toX); //7						// push it to normals vector
													// **repeated for each combination**
	// L = Ax * By
	vector3 toXY = glm::cross(thisX, otherY);	
	if (toXY != ZERO_V3) toXY = glm::normalize(toXY);
	normals.push_back(toXY); //8				

	// L = Ax * Bz
	vector3 toXZ = glm::cross(thisX, otherZ);	
	if (toXZ != ZERO_V3) toXZ = glm::normalize(toXZ);
	normals.push_back(toXZ); //9				

	// L = Ay * Bx
	vector3 toYX = glm::cross(thisY, otherX);	
	if (toYX != ZERO_V3) toYX = glm::normalize(toYX);
	normals.push_back(toYX); //10				

	// L = Ay * By
	vector3 toY = glm::cross(thisY, otherY);
	if (toY != ZERO_V3) toY = glm::normalize(toY);
	normals.push_back(toY); //11

	// L = Ay * Bz
	vector3 toYZ = glm::cross(thisY, otherZ);
	if (toYZ != ZERO_V3) toYZ = glm::normalize(toYZ);
	normals.push_back(toYZ); //12

	// L = Az * Bx
	vector3 toZX = glm::cross(thisZ, otherX);
	if (toZX != ZERO_V3) toZX = glm::normalize(toZX);
	normals.push_back(toZX); //13

	// L = Az * By
	vector3 toZY = glm::cross(thisZ, otherY);
	if (toZY != ZERO_V3) toZY = glm::normalize(toZY);
	normals.push_back(toZY); //14

	// L = Az * Bz
	vector3 toZ = glm::cross(thisZ, otherZ);
	if (toZ != ZERO_V3) toZ = glm::normalize(toZ);
	normals.push_back(toZ); //15
#pragma endregion


	float ra, rb;
	matrix3 R, AbsR;
	vector3 thisHalfWidth = m_v3HalfWidth;
	vector3 otherHalfWidth = a_pOther->m_v3HalfWidth;

	//Compute rotation matrix expressing b in a's coordinate frame
	for (uint i = 0; i < 3; i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			R[i][j] = glm::dot(thisAxes[i], otherAxes[j]);
		}
	}

	// compute translation vector t
	vector3 t = a_pOther->GetCenterGlobal() - GetCenterGlobal();
	//vector3 t = a_pOther->m_v3Center - m_v3Center;
	// bring translation into this coordinate frame
	t = vector3(glm::dot(t, thisAxes[0]), glm::dot(t, thisAxes[1]), glm::dot(t, thisAxes[2]));

	// Compute common subexpressions. Add in epislon term to conteract arithmitac errors when two
	// edges are parrallel and their cross product is (near) null 

	for (uint i = 0; i < 3; i++)
		for (uint j = 0; j < 3; j++)
			AbsR[i][j] = glm::abs(R[i][j]) + DBL_EPSILON;

	// test axes L=A0, L=A1, L=A2
	for (uint i = 0; i < 3; i++)
	{
		ra = thisHalfWidth[i];
		rb = otherHalfWidth.x * AbsR[i][0] + otherHalfWidth.y * AbsR[i][1] + otherHalfWidth.z * AbsR[i][2];
		if (glm::abs(t[i]) > ra + rb)
			return 1;
	}

	// test axes L=B0, L=B1, L=B2
	for (uint i = 0; i < 3; i++)
	{
		ra = thisHalfWidth.x * AbsR[0][i] + thisHalfWidth.y * AbsR[1][i] + thisHalfWidth.z * AbsR[2][i];
		rb = otherHalfWidth[i];
		if (glm::abs(t.x * R[0][i] + t.y * R[1][i] + t.z * R[2][i]) > ra + rb)
			return 1;
	}

	// test axis  L = A0 x B0
	ra = thisHalfWidth.y * AbsR[2][0] + thisHalfWidth.z * AbsR[1][0];
	rb = otherHalfWidth.y * AbsR[2][0] + otherHalfWidth.z * AbsR[0][1];
	if (glm::abs(t.z * R[1][0] - t.y * R[2][0]) > ra + rb)
		return 1;

	// test axis  L = A0 x B1
	ra = thisHalfWidth.y * AbsR[2][1] + thisHalfWidth.z * AbsR[1][1];
	rb = otherHalfWidth.x * AbsR[0][2] + otherHalfWidth.z * AbsR[0][0];
	if (glm::abs(t.z * R[1][1] - t.y * R[2][1]) > ra + rb)
		return 1;

	// test axis  L = A0 x B2
	ra = thisHalfWidth.y * AbsR[2][2] + thisHalfWidth.z * AbsR[1][2];
	rb = otherHalfWidth.x * AbsR[0][1] + otherHalfWidth.y * AbsR[0][0];
	if (glm::abs(t.z * R[1][2] - t.y * R[2][2]) > ra + rb)
		return 1;

	// test axis  L = A1 x B0
	ra = thisHalfWidth.x * AbsR[2][0] + thisHalfWidth.z * AbsR[0][0];
	rb = otherHalfWidth.y * AbsR[1][2] + otherHalfWidth.z * AbsR[1][1];
	if (glm::abs(t.x * R[2][0] - t.z * R[0][0]) > ra + rb)
		return 1;

	// test axis  L = A1 x B1
	ra = thisHalfWidth.x * AbsR[2][1] + thisHalfWidth.z * AbsR[0][1];
	rb = otherHalfWidth.x * AbsR[1][2] + otherHalfWidth.z * AbsR[1][0];
	if (glm::abs(t.x * R[2][1] - t.z * R[0][1]) > ra + rb)
		return 1;

	// test axis  L = A1 x B2
	ra = thisHalfWidth.x * AbsR[2][2] + thisHalfWidth.z * AbsR[0][2];
	rb = otherHalfWidth.x * AbsR[1][1] + otherHalfWidth.y * AbsR[1][0];
	if (glm::abs(t.x * R[2][2] - t.z * R[0][2]) > ra + rb)
		return 1;

	// test axis  L = A2 x B0
	ra = thisHalfWidth.x * AbsR[1][0] + thisHalfWidth.y * AbsR[0][0];
	rb = otherHalfWidth.y * AbsR[2][2] + otherHalfWidth.z * AbsR[2][1];
	if (glm::abs(t.y * R[0][0] - t.x * R[1][0]) > ra + rb)
		return 1;

	// test axis  L = A2 x B1
	ra = thisHalfWidth.x * AbsR[1][1] + thisHalfWidth.y * AbsR[0][1];
	rb = otherHalfWidth.x * AbsR[2][2] + otherHalfWidth.z * AbsR[2][0];
	if (glm::abs(t.y * R[0][1] - t.x * R[1][1]) > ra + rb)
		return 1;

	// test axis  L = A2 x B2
	ra = thisHalfWidth.x * AbsR[1][2] + thisHalfWidth.y * AbsR[0][2];
	rb = otherHalfWidth.x * AbsR[2][1] + otherHalfWidth.y * AbsR[2][0];
	if (glm::abs(t.y * R[0][2] - t.x * R[1][2]) > ra + rb)
		return 1;

/*
#pragma region test each axes
	float thisMaxProj = 0.0f;		// store the max proj of this
	float thisMinProj = INFINITY;	// store the min proj of this
	float otherMaxProj = 0.0f;		// store the max proj of other
	float otherMinProj = INFINITY;	// store the min proj of other
	// test every point on each normal
	for (uint i = 0; i < normals.size(); i++)
	{
		for (uint j = 0; j < numCorners; j++)
		{
			// formula: A + dot(AP, AB) / dot(AB, AB) * AB
			//vector3 temp = v3ThisCorner[0]
			//	+ glm::dot(v3ThisCorner[j] - normals[i], normals[i])
			//	/ glm::dot(normals[i], normals[i]) * normals[i];

			// projection of corner[j] onto normal[i] of this object
			float tempThis = (normals[i].x * v3ThisCorner[j].x) +
				(normals[i].y * v3ThisCorner[j].y) +
				(normals[i].z * v3ThisCorner[j].z);

			//float tempThis = glm::dot(v3ThisCorner[j], normals[i]);

			// projection of corner[j] onto normal[i] of other object
			float tempOther = (normals[i].x * v3OtherCorner[j].x) +
				(normals[i].y * v3OtherCorner[j].y) +
				(normals[i].z * v3OtherCorner[j].z);

			float tempOtherCenter = (normals[i].x * a_pOther->m_v3Center.x) +
				(normals[i].y * a_pOther->m_v3Center.y) +
				(normals[i].z * a_pOther->m_v3Center.z);

			//float tempOther = glm::dot(v3OtherCorner[j], normals[i]);

			// check and store max and min of the projections
			if (tempThis > thisMaxProj) thisMaxProj = tempThis;
			else if (tempThis < thisMinProj) thisMinProj = tempThis;

			if (tempOther > otherMaxProj) otherMaxProj = tempOther;
			else if (tempOther < otherMinProj) otherMinProj = tempOther;
		}
		float thisCenterProj = (normals[i].x * m_v3Center.x) +
			(normals[i].y * m_v3Center.y) +
			(normals[i].z * m_v3Center.z);

		float otherCenterProj = (normals[i].x * a_pOther->m_v3Center.x) +
			(normals[i].y * a_pOther->m_v3Center.y) +
			(normals[i].z * a_pOther->m_v3Center.z);

		float thisHalf = thisMaxProj - thisCenterProj;
		float otherHalf = otherMinProj - otherCenterProj;
		//thisMaxProj < otherMinProj || otherMaxProj < thisMinProj
		if (thisHalf + otherHalf > thisCenterProj - otherCenterProj)
			return 1;
	}

#pragma endregion
*/
	
	//there is no axis test that separates these two objects
	return eSATResults::SAT_NONE;
}