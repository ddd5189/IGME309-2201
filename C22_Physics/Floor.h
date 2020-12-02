/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __FLOOR_H_
#define __FLOOR_H_

#include "Definitions.h"

#include "MyRigidBody.h"

namespace Simplex
{

class Floor
{

private:

	int ID;
	vector3 position, min, max; 

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
	///*
	//Usage: destructor
	//Arguments: ---
	//Output: ---
	//*/
	//~AStar(void);
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

	Floor(uint ID, MyRigidBody* rb);

	Floor(Floor const& a_pOther);

	~Floor(void);


};

};


#endif //__FLOOR_H_


