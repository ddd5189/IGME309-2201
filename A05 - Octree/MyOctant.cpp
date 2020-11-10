#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::octCount = 0;
uint MyOctant::maxLevel = 3;
uint MyOctant::idealCount = 5;

MyOctant::MyOctant(uint maxLevelP, uint idealCountP)
{
	// Creating the root Octant

	// Default Values
	Init();

	// Set the passed in parameters
	maxLevel = maxLevelP;
	idealCount = idealCountP;

	// Set this to the root
	octRoot = this;

	// List of min and max vectors of the bounding object
	std::vector<vector3> minMaxVectors;

	// Amount of objects in the scene
	uint objCount = entityMng->GetEntityCount();
	for (uint i = 0; i < objCount; i++)
	{
		// Get each entity and it's rigidBody and store its global min max in the list.
		MyRigidBody* rb = entityMng->GetEntity(i)->GetRigidBody();
		minMaxVectors.push_back(rb->GetMaxGlobal());
		minMaxVectors.push_back(rb->GetMinGlobal());
	}

	// Rigid Body using the list of min max vectors
	MyRigidBody* rb = new MyRigidBody(minMaxVectors);

	vector3 halfWidth = rb->GetHalfWidth();	// Half Width of the created rigid body to make the box
	float maxValue = 0.0f;					// Max Value for finding the size, center, max and min
	
	// Loop through each component
	for (int i = 0; i < 3; i++)
	{
		// Make sure maxValue is the largest component of the halfWidth
		if (maxValue < halfWidth[i])
			maxValue = halfWidth[i];
	}

	vector3 center = rb->GetCenterLocal();	// Get the center of the rigid body
	octSize = maxValue * 2.0f;				// Set the size of the octant
	octCenter = center;						// Set the Octants center
	octMax = center + (vector3(maxValue));	// Set the max of the Octant
	octMin = center - (vector3(maxValue));	// Set the min of the Octant

	octCount++;								// Increment the count, as now there is the root Octant

	// Clear and delete all the memory we no longer need
	minMaxVectors.clear();
	SafeDelete(rb);
	
	// Now construct the tree based on the max level passed in
	ConstructTree(maxLevel);
}

MyOctant::MyOctant(vector3 center, float size)
{
	// Creating octants that are children to the root

	// Default values
	Init();

	// Set the passed in paramaters
	octCenter = center;
	octSize = size;

	octMax = octCenter + (vector3(octSize) / 2.0f);	// Set the max of the Octant
	octMin = octCenter - (vector3(octSize) / 2.0f);	// Set the min of the Octant

	octCount++;		// Increment the count of how many Octants there are
}

MyOctant::MyOctant(MyOctant const& other)
{
	// Copy Constructor for the MyOctant Class
	octChildrenCount = other.octChildrenCount;
	octCenter = other.octCenter;
	octMax = other.octMax;
	octMin = other.octMin;
	octSize = other.octSize;
	octID = other.octID;
	octLevel = other.octLevel;
	octParent = other.octParent;
	octRoot = other.octRoot;
	childList = other.childList;
	// Loop through all the children 
	for (uint i = 0; i < 8; i++)
	{
		octChild[i] = other.octChild[i];
	}
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	// Copy Assignment Operator using swap
	// Make sure this is not the same as the other 
	if (this != &other) {
		Release();				// Release all the memory of this object
		Init();					// Reinitalize to the default values
		MyOctant temp(other);	// Temp object of other 
		Swap(temp);				// Swap all it's values
	}
	return *this;
}

MyOctant::~MyOctant(void) { Release(); }

float MyOctant::GetSize(void) { return octSize; }

vector3 MyOctant::GetCenterGlobal(void) { return octCenter; }

vector3 MyOctant::GetMinGlobal(void) { return octMin; }

vector3 MyOctant::GetMaxGlobal(void) { return octMax; }

MyOctant* MyOctant::GetParent(void) { return octParent; }

bool MyOctant::IsLeaf(void) { return octChildrenCount == 0; }

uint MyOctant::GetOctantCount(void) { return octCount; }

void MyOctant::Swap(MyOctant& other)
{
	// Swap all variables to the other object
	std::swap(octChildrenCount, other.octChildrenCount);
	std::swap(octSize, other.octSize);
	std::swap(octID, other.octID);
	std::swap(octRoot, other.octRoot);
	std::swap(childList, other.childList);
	std::swap(octCenter, other.octCenter);
	std::swap(octMax, other.octMax);
	std::swap(octMin, other.octMin);
	std::swap(octLevel, other.octLevel);
	std::swap(octParent, other.octParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(octChild[i], other.octChild[i]);
	}
}

bool MyOctant::IsColliding(uint RBIndex)
{
	MyRigidBody* rb = entityMng->GetEntity(RBIndex)->GetRigidBody();	// Rigid Body of that Entity
	vector3 entMax = rb->GetMaxGlobal();								// Global Max of the Entity
	vector3 entMin = rb->GetMinGlobal();								// Global Min of the Entity
	uint objCount = entityMng->GetEntityCount();						// Amount of objects in the scene

	// AABB collision tests
	// Each test is checking if the object is within the Octants bounds
	// If it isn't on any axis then it won't be inside
	// Check for collision on the x-axis
	if (entMax.x < octMin.x) return false;
	if (entMin.x > octMax.x) return false;
	// Check for collision on the y-axis
	if (entMax.y < octMin.y) return false;
	if (entMin.y > octMax.y) return false;
	// Check for collision on the z-axis
	if (entMax.z < octMin.z) return false;
	if (entMin.z > octMax.z) return false;

	// If all fail then the object is within the Octant
	return true;
}

void MyOctant::Display(uint index, vector3 color)
{
	// Draw the Wire Cube based on the data of the proper octant
	if (octID == index) {
		// ToWorld mat4
		matrix4 toWorld = glm::translate(IDENTITY_M4, octCenter) * glm::scale(vector3(octSize));
		meshMng->AddWireCubeToRenderList(toWorld, color);
	}
	// If the current octID isn't the index passed in
	// loop through all the children and Display the correct Wire Cube
	else {
		for (uint i = 0; i < octChildrenCount; i++)
		{
			octChild[i]->Display(index);
		}
	}
}

void MyOctant::Display(vector3 color)
{
	// Loop through every Octant and display the wire cube
	if (octChildrenCount != 0) {
		for (uint i = 0; i < octChildrenCount; i++)
		{
			octChild[i]->Display(color);
		}
	}
	else {
		// ToWorld mat4
		matrix4 toWorld = glm::translate(IDENTITY_M4, octCenter) * glm::scale(vector3(octSize));
		meshMng->AddWireCubeToRenderList(toWorld, color);
	}
}

void MyOctant::DisplayLeafs(vector3 color)
{
	// I didn't find any use for this method
}

void MyOctant::ClearEntityList(void)
{
	// Clear the entity list of all octants
	// Loop only if the current octant has children
	if (octChildrenCount != 0) {
		for (uint i = 0; i < octChildrenCount; i++)
		{
			octChild[i]->ClearEntityList();
		}
	}
	else {
		entityList.clear();
	}
}

void MyOctant::Subdivide(void)
{
	// Don't subdivide if at max level
	if (octLevel >= maxLevel) return;

	// Don't subdivide if this Octant already has children
	if (octChildrenCount != 0) return;

	// Adding children so set octChildrenCount to 8
	octChildrenCount = 8;

	float halfSize = octSize / 4.0f;	// Half size of the new child Octant
	float fullSize = halfSize * 2.0f;	// Full size of the new child Octant
	vector3 center;						// Center vector of the new child Octant

	uint index = 0;						// Index to iterate through each of the 8 children

	// Move the center around and create the child at that center point

	// Key
	// 1: Front or Back (F/B)
	// 2: Top or Bottom	(T/B)
	// 3: Left or Right (L/R)

	// FTR 0
	center = octCenter;
	center.x += halfSize;
	center.y += halfSize;
	center.z += halfSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// FTL 1
	center.x -= fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// FBL 2
	center.y -= fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// FBR 3
	center.x += fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// BBR 4
	center.z -= fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// BBL 5
	center.x -= fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// BTL 6
	center.y += fullSize;
	octChild[index] = new MyOctant(center, fullSize);
	index++;

	// BTR 7
	center.x += fullSize;
	octChild[index] = new MyOctant(center, fullSize);

	// Now loop through all the children and set them to the next level in the tree 
	// set their root to to the root
	// and set their parent to this object
	for (uint i = 0; i < 8; i++)
	{
		octChild[i]->octLevel = 1 + octLevel;
		octChild[i]->octRoot = octRoot;
		octChild[i]->octParent = this;
		// Check if the child holds more than the idealCount
		// If it doesn't no need to subdivide again
		if (octChild[i]->ContainsMoreThan(idealCount))
		{
			// Then subdivide again
			octChild[i]->Subdivide();
		}
	}
}

MyOctant* MyOctant::GetChild(uint child)
{
	// Make sure it's less than 8
	if (child < 7) return this->octChild[child];
	else return nullptr;
}

bool MyOctant::ContainsMoreThan(uint entities)
{
	uint idealEntities = entities;					// Ideal amount of entities in any given Octnat
	uint entitiesInCount = 0;						// Amount of entities found inside the Octant
	uint objCount = entityMng->GetEntityCount();	// Amount of all entities in the scene
	for (uint i = 0; i < objCount; i++)
	{
		// If the entity is in the Octant, increase the entityCount
		// When entityCount reaches more than the idealCount return true
		if (IsColliding(i)) entitiesInCount++;
		if (entitiesInCount > idealEntities) return true;
	}
	return false;
}

void MyOctant::KillBranches(void)
{
	// Loop through all the children and recursivly delete working from the bottom up
	for (uint i = 0; i < octChildrenCount; i++)
	{
		octChild[i]->KillBranches();
		SafeDelete(octChild[i]);
	}
	// Once all the children are deleted set the count back to 0
	octChildrenCount = 0;
}

void MyOctant::ConstructTree(uint maxLevelP)
{
	// Make sure the octCount is 1 (Helps with drawing each individual octants wire cube)
	octCount = 1;

	// Subdivide if the root holds more than the ideal count
	if (ContainsMoreThan(idealCount)) {
		Subdivide();
	}

	// Assign the proper Octant ID to the entities in that Octant 
	AssignIDtoEntity();

	// Construct the list of objects
	ConstructList();
}

void MyOctant::AssignIDtoEntity(void)
{
	// Recursively call AssignIDtoEntity until it reaches a leaf
	if (octChildrenCount != 0) {
		for (uint i = 0; i < octChildrenCount; i++)
		{
			octChild[i]->AssignIDtoEntity();
		}
	}
	// Once it reaches a leaf
	else {
		uint objCount = entityMng->GetEntityCount();
		for (uint i = 0; i < objCount; i++)
		{
			// Check what entities are in the Octant and add them to the entity list
			// and add a unique dimension to the entity
			if (IsColliding(i)) {
				entityList.push_back(i);
				entityMng->AddDimension(i, octID);
			}
		}
	}
}

// Private Methods
void MyOctant::Init(void)
{
	// set all values to default
	octID = octCount;
	octSize = 0.0f;
	octLevel = 0;
	octChildrenCount = 0;
	octCenter = ZERO_V3;
	octMax = ZERO_V3;
	octMin = ZERO_V3;
	octRoot = nullptr;
	octParent = nullptr;
	for (uint i = 0; i < 8; i++)
	{
		octChild[i] = nullptr;
	}

	childList.clear();
	ClearEntityList();

	meshMng = MeshManager::GetInstance();
	entityMng = MyEntityManager::GetInstance();
}

void MyOctant::Release(void)
{
	// Clear the entity and child list 
	ClearEntityList();
	childList.clear();

	// Kill all the branches
	KillBranches();
}

void MyOctant::ConstructList(void)
{
	// Loop through if it has children, 
	// otherwise check if it has any objects in it and add it to he child list
	if (octChildrenCount != 0) {
		// Add all the children to an the childList
		for (uint i = 0; i < octChildrenCount; i++)
		{
			octChild[i]->ConstructList();
		}
	}
	else {
		// Add all the children list to the root
		if (entityList.size() != 0) {
			octRoot->childList.push_back(this);
		}
	}
}
