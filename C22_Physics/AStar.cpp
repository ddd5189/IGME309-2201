#include "AStar.h"
#include "Floor.h"
using namespace Simplex;

Simplex::AStar* Simplex::AStar::m_pInstance = nullptr;

Simplex::AStar::~AStar(void)
{
	for (uint i = 0; i < m_mFloorList.size(); i++)
	{
		if (m_mFloorList[i] != nullptr) {
			SafeDelete(m_mFloorList[i]);
		}
	}
}

AStar* Simplex::AStar::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new AStar();
	}
	return m_pInstance;
}

void Simplex::AStar::ReleaseInstance(void)
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void Simplex::AStar::AddEntity(MyRigidBody* block)
{
	m_mFloorList.push_back(block);
	m_uEntityCount++;
}

