#ifndef _MYOCTANTCLASS_H_
#define _MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex 
{

class MyOctant
{
	static uint octCount;			// Number of Octants
	static uint maxLevel;			// Max level Oct Tree can go to
	static uint idealCount;			// Ideal count of entities in any given octant

	uint octID = 0;					// ID of Octant
	uint octLevel = 0;				// Level of Octant
	uint octChildrenCount = 0;		// Number of Children in Octant (0 or 8)

	float octSize = 0.0f;			// Size of the Octant

	MeshManager* meshMng = nullptr;			// Mesh Manager
	MyEntityManager* entityMng = nullptr;	// Entity Manager

	vector3 octCenter = vector3(0.0f);		// Center of Octant
	vector3 octMax = vector3(0.0f);			// Max of Octant
	vector3 octMin = vector3(0.0f);			// Min of Octant

	MyOctant* octParent = nullptr;			// Parent of Octant
	MyOctant* octChild[8];					// List of children of Octant

	std::vector<uint> entityList;			// List of entities in the Octant

	MyOctant* octRoot = nullptr;			// Root Octant
	std::vector<MyOctant*> childList;		// List of Octants that contain objects (only applies to root)

public:

	MyOctant(uint maxLevelP = 2, uint idealCountP = 5);

	MyOctant(vector3 center, float size);

	MyOctant(MyOctant const& other);

	MyOctant& operator=(MyOctant const& other);

	~MyOctant(void);

	void Swap(MyOctant& other);

	float GetSize(void);

	vector3 GetCenterGlobal(void);

	vector3 GetMinGlobal(void);

	vector3 GetMaxGlobal(void);

	bool IsColliding(uint RBIndex);

	void Display(uint index, vector3 color = C_YELLOW);

	void Display(vector3 color = C_YELLOW);

	void DisplayLeafs(vector3 color = C_YELLOW);

	void ClearEntityList(void);

	void Subdivide(void);

	MyOctant* GetChild(uint child);

	MyOctant* GetParent(void);

	bool IsLeaf(void);

	bool ContainsMoreThan(uint entities);

	void KillBranches(void);

	void ConstructTree(uint maxLevelP = 3);

	void AssignIDtoEntity(void);

	uint GetOctantCount(void);

private:

	void Release(void);

	void Init(void);

	void ConstructList(void);
};

}

#endif 

