/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __ASTARCLASS_H_
#define __ASTARCLASS_H_

#include "Definitions.h"
#include "MyEntityManager.h"
#include "MyEntity.h"
#include "MyRigidBody.h"

namespace Simplex
{

class AStar
{
	MyEntityManager* m_pEntityMngr = nullptr; //Entity Manager
private:

	typedef MyEntity* PEntity; //MyEntity Pointer
	uint m_uEntityCount = 0; //number of elements in the list
	std::vector<MyRigidBody*> m_mFloorList; //list of entities
	static AStar* m_pInstance; // Singleton pointer

	//	/*
	//Usage: constructor
	//Arguments: ---
	//Output: class object instance
	//*/
	//AStar(void);
	///*
	//Usage: copy constructor
	//Arguments: class object to copy
	//Output: class object instance
	//*/
	//AStar(AStar const& a_pOther);
	///*
	//Usage: copy assignment operator
	//Arguments: class object to copy
	//Output: ---
	//*/
	//AStar& operator=(AStar const& a_pOther);
	/*
	Usage: destructor
	Arguments: ---
	Output: ---
	*/
	~AStar(void);
	///*
	//Usage: releases the allocated member pointers
	//Arguments: ---
	//Output: ---
	//*/
	//void Release(void);
	///*
	//Usage: initializes the singleton
	//Arguments: ---
	//Output: ---
	//*/
	//void Init(void);
public:
	static AStar* GetInstance();
	/*
	Usage: Releases the content of the singleton
	Arguments: ---
	Output: ---
	*/
	static void ReleaseInstance(void);
	/*
	USAGE: Gets the index (from the list of entities) of the entity specified by UniqueID
	ARGUMENTS: String a_sUniqueID -> queried index
	OUTPUT: index from the list of entities, -1 if not found
	*/
	void AddEntity(MyRigidBody* block);
	/*
	USAGE: Deletes the MyEntity Specified by unique ID and removes it from the list
	ARGUMENTS: uint a_uIndex -> index of the queried entry, if < 0 asks for the last one added
	OUTPUT: ---
	*/
};

};


#endif //__ASTARCLASS_H_
