#include "Floor.h"

Simplex::Floor::Floor(uint ID, MyRigidBody* rb)
{
	this->ID = ID;
	this->position = rb->GetCenterGlobal();
	this->min = rb->GetMinGlobal();
	this->max = rb->GetMaxGlobal();
}

Simplex::Floor::Floor(Floor const& a_pOther)
{
}


Simplex::Floor::~Floor(void)
{

}
