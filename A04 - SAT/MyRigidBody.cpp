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
vector3 MyRigidBody::GetCenterGlobal(void) { return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
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
		if (SAT(a_pOther) != eSATResults::SAT_NONE)
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
#pragma region Find and store corners for each OBB
	// Number of corners (8) 
	const uint numCorners = 8;
	//	Corners of This box
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

	//	Convert the corners to world space
	for (uint i = 0; i < numCorners; ++i)
	{
		v3ThisCorner[i] = vector3(m_m4ToWorld * vector4(v3ThisCorner[i], 1.0f));
	}

	vector3 otherMax = a_pOther->GetMaxLocal(); // Max of the Other box in local coordinate system
	vector3 otherMin = a_pOther->GetMinLocal(); // Min of the Other box	in local coordinate system
	// Other box's model matrix
	matrix4 otherModelMatrix = a_pOther->GetModelMatrix();

	// Corners of the Other box
	vector3 v3OtherCorner[numCorners];
	// Multiply each by the model matrix so it's in the proper orientation
	//Back square
	v3OtherCorner[0] = otherMin;
	v3OtherCorner[1] = vector3(otherMax.x, otherMin.y, otherMin.z);
	v3OtherCorner[2] = vector3(otherMin.x, otherMax.y, otherMin.z);
	v3OtherCorner[3] = vector3(otherMax.x, otherMax.y, otherMin.z);
	//Front square			 
	v3OtherCorner[4] = vector3(otherMin.x, otherMin.y, otherMax.z);
	v3OtherCorner[5] = vector3(otherMax.x, otherMin.y, otherMax.z);
	v3OtherCorner[6] = vector3(otherMin.x, otherMax.y, otherMax.z);
	v3OtherCorner[7] = otherMax;

	// Convert the corners to world space 
	for (uint i = 0; i < numCorners; i++)
	{
		v3OtherCorner[i] = vector3(otherModelMatrix * vector4(v3OtherCorner[i], 1.0f));
	}
#pragma endregion

#pragma region Axes Calculations
	std::vector<vector3> thisAxes;	// Axes of This objects OBB 
	std::vector<vector3> otherAxes;	// Axes of Other objects OBB

	const uint index0 = 0;	// Index of the min corner (0)
	const uint index1 = 1;	// Index of the corner to the right of the min corner on the x-axis (1)
	const uint index2 = 2;	// Index of the corner above the min corner on the y-axis (2)
	const uint index4 = 4;	// Index of the corner in front of the min corner on the z-axis (4)

	//----------------------------This Object-------------------------------------------------------------------
	vector3 thisX = v3ThisCorner[index1] - v3ThisCorner[index0];	// X axis of This OBB
	vector3 thisXAxis = glm::normalize(thisX);						// Normalized X axis vector of This OBB
	thisAxes.push_back(thisXAxis);

	vector3 thisY = v3ThisCorner[index2] - v3ThisCorner[index0];	// Y axis of This OBB
	vector3 thisYAxis = glm::normalize(thisY);						// Normalized Y axis vector of This OBB
	thisAxes.push_back(thisYAxis);

	vector3 thisZ = v3ThisCorner[index4] - v3ThisCorner[index0];	// Z axis of This OBB
	vector3 thisZAxis = glm::normalize(thisZ);						// Normalized Z axis vector of This OBB
	thisAxes.push_back(thisZAxis);

	//----------------------------Other Object------------------------------------------------------------------
	vector3 otherX = v3OtherCorner[index1] - v3OtherCorner[index0];	// X axis of the Other OBB
	vector3 otherXAxis = glm::normalize(otherX);					// Normalized X axis vector of Other OBB
	otherAxes.push_back(otherXAxis);

	vector3 otherY = v3OtherCorner[index2] - v3OtherCorner[index0];	// Y axis of the Other OBB
	vector3 otherYAxis = glm::normalize(otherY);					// Normalized Y axis vector of Other OBB
	otherAxes.push_back(otherYAxis);

	vector3 otherZ = v3OtherCorner[index4] - v3OtherCorner[index0];	// Z axis of the Other OBB
	vector3 otherZAxis = glm::normalize(otherZ);					// Normalized Z axis vector of Other OBB
	otherAxes.push_back(otherZAxis);
#pragma endregion

#pragma region SAT tests
	// Following code was adapted from Chapter 4 of Real-Time Collision Detection by Christer Ericson (pgs. 101-105)

	float thisProjection;		// Projection of This object on the current separting plane
	float otherProjection;		// Projection of the Other object on the current separting plane
	const uint mat3Value = 3;	// For looping through the matrices (3)
	const uint axesValue = 3;	// For looping through the axes (3)
	const uint xAxis = 0;		// Index representing x-axis (0)
	const uint yAxis = 1;		// Index representing y-axis (1)
	const uint zAxis = 2;		// Index representing z-axis (2)
	matrix3 R;					// Rotational matrix that brings the Other object into This objects coordinate system
	matrix3 AbsR;				// The absolute value of the R matrix
	vector3 thisHalfWidth = m_v3HalfWidth;				// Halfwidth of This OOB
	vector3 otherHalfWidth = a_pOther->m_v3HalfWidth;	// Halfwidth of the Other OOB

	// Calculate the rotation matrix (R)
	for (uint i = 0; i < mat3Value; i++)
	{
		for (uint j = 0; j < mat3Value; j++)
		{
			R[i][j] = glm::dot(thisAxes[i], otherAxes[j]);
		}
	}

	// The distance between the center of each object
	vector3 distanceCenter = a_pOther->GetCenterGlobal() - GetCenterGlobal();
	// Bring distanceCenter into This coordinate frame
	distanceCenter = vector3(
		glm::dot(distanceCenter, thisAxes[xAxis]),
		glm::dot(distanceCenter, thisAxes[yAxis]),
		glm::dot(distanceCenter, thisAxes[zAxis]));

	// Compute common subexpressions. Add in epislon term to conteract arithmitac errors when two
	// edges are parrallel and their cross product is (near) null 
	for (uint i = 0; i < mat3Value; i++)
		for (uint j = 0; j < mat3Value; j++)
			AbsR[i][j] = glm::abs(R[i][j]) + DBL_EPSILON;

	// *Note A = This B = Other
	// test axes L=AX, L=AY, L=AZ
	for (uint i = 0; i < axesValue; i++)
	{
		thisProjection = thisHalfWidth[i];
		otherProjection = otherHalfWidth.x * AbsR[i][0] + otherHalfWidth.y * AbsR[i][1] + otherHalfWidth.z * AbsR[i][2];
		if (glm::abs(distanceCenter[i]) > thisProjection + otherProjection) {
			switch (i)
			{
			case 0: return eSATResults::SAT_AX; // if gap on X axis
			case 1: return eSATResults::SAT_AY; // if gap on Y axis
			case 2: return eSATResults::SAT_AZ; // if gap on Z axis
			default:return 1;
			}
		}
	}

	// test axes L=BX, L=BY, L=BZ
	for (uint i = 0; i < axesValue; i++)
	{
		thisProjection = thisHalfWidth.x * AbsR[0][i] + thisHalfWidth.y * AbsR[1][i] + thisHalfWidth.z * AbsR[2][i];
		otherProjection = otherHalfWidth[i];
		if (glm::abs(distanceCenter.x * R[0][i] +
			distanceCenter.y * R[1][i] +
			distanceCenter.z * R[2][i]) > thisProjection + otherProjection) {
			switch (i)
			{
			case 0: return eSATResults::SAT_BX; // if gap on X axis
			case 1: return eSATResults::SAT_BY; // if gap on Y axis
			case 2: return eSATResults::SAT_BZ; // if gap on Z axis
			default:return 1;
			}
		}
	}

	// **Note x = cross product of each axis
	// test axis AX x BX
	thisProjection = thisHalfWidth.y * AbsR[2][0] + thisHalfWidth.z * AbsR[1][0];
	otherProjection = otherHalfWidth.y * AbsR[0][2] + otherHalfWidth.z * AbsR[0][1];
	if (glm::abs(distanceCenter.z * R[1][0] - distanceCenter.y * R[2][0]) > thisProjection + otherProjection)
		return eSATResults::SAT_AXxBX;

	// test axis AX x BY
	thisProjection = thisHalfWidth.y * AbsR[2][1] + thisHalfWidth.z * AbsR[1][1];
	otherProjection = otherHalfWidth.x * AbsR[0][2] + otherHalfWidth.z * AbsR[0][0];
	if (glm::abs(distanceCenter.z * R[1][1] - distanceCenter.y * R[2][1]) > thisProjection + otherProjection)
		return eSATResults::SAT_AXxBY;

	// test axis AX x BZ
	thisProjection = thisHalfWidth.y * AbsR[2][2] + thisHalfWidth.z * AbsR[1][2];
	otherProjection = otherHalfWidth.x * AbsR[0][1] + otherHalfWidth.y * AbsR[0][0];
	if (glm::abs(distanceCenter.z * R[1][2] - distanceCenter.y * R[2][2]) > thisProjection + otherProjection)
		return eSATResults::SAT_AXxBZ;

	// test axis AY x BX
	thisProjection = thisHalfWidth.x * AbsR[2][0] + thisHalfWidth.z * AbsR[0][0];
	otherProjection = otherHalfWidth.y * AbsR[1][2] + otherHalfWidth.z * AbsR[1][1];
	if (glm::abs(distanceCenter.x * R[2][0] - distanceCenter.z * R[0][0]) > thisProjection + otherProjection)
		return eSATResults::SAT_AYxBX;

	// test axis AY x BY
	thisProjection = thisHalfWidth.x * AbsR[2][1] + thisHalfWidth.z * AbsR[0][1];
	otherProjection = otherHalfWidth.x * AbsR[1][2] + otherHalfWidth.z * AbsR[1][0];
	if (glm::abs(distanceCenter.x * R[2][1] - distanceCenter.z * R[0][1]) > thisProjection + otherProjection)
		return eSATResults::SAT_AYxBY;

	// test axis AY x BZ
	thisProjection = thisHalfWidth.x * AbsR[2][2] + thisHalfWidth.z * AbsR[0][2];
	otherProjection = otherHalfWidth.x * AbsR[1][1] + otherHalfWidth.y * AbsR[1][0];
	if (glm::abs(distanceCenter.x * R[2][2] - distanceCenter.z * R[0][2]) > thisProjection + otherProjection)
		return eSATResults::SAT_AYxBZ;

	// test axis AZ x BX
	thisProjection = thisHalfWidth.x * AbsR[1][0] + thisHalfWidth.y * AbsR[0][0];
	otherProjection = otherHalfWidth.y * AbsR[2][2] + otherHalfWidth.z * AbsR[2][1];
	if (glm::abs(distanceCenter.y * R[0][0] - distanceCenter.x * R[1][0]) > thisProjection + otherProjection)
		return eSATResults::SAT_AZxBX;

	// test axis AZ x BY
	thisProjection = thisHalfWidth.x * AbsR[1][1] + thisHalfWidth.y * AbsR[0][1];
	otherProjection = otherHalfWidth.x * AbsR[2][2] + otherHalfWidth.z * AbsR[2][0];
	if (glm::abs(distanceCenter.y * R[0][1] - distanceCenter.x * R[1][1]) > thisProjection + otherProjection)
		return eSATResults::SAT_AZxBY;

	// test axis AZ x BZ
	thisProjection = thisHalfWidth.x * AbsR[1][2] + thisHalfWidth.y * AbsR[0][2];
	otherProjection = otherHalfWidth.x * AbsR[2][1] + otherHalfWidth.y * AbsR[2][0];
	if (glm::abs(distanceCenter.y * R[0][2] - distanceCenter.x * R[1][2]) > thisProjection + otherProjection)
		return eSATResults::SAT_AZxBZ;

	//there is no axis test that separates these two objects
	return eSATResults::SAT_NONE;
#pragma endregion
}