#include "AppClass.h"
void Application::InitVariables(void)
{
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	//m_pMesh->GenerateSphere(1.0f, 5, C_WHITE);

	for (int i = 0; i < m_iCubeCount; i++)
	{
		MyMesh* m_pMesh = new MyMesh();
		m_pMesh->GenerateCube(1.0f, C_BLACK);
		m_pCubeList.push_back(m_pMesh);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	// 1
	m_pCubeList[0]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + m_fValue, 7.0f, 0.0f)));
	m_pCubeList[1]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + m_fValue, 7.0f, 0.0f)));

	// 2
	m_pCubeList[2]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + m_fValue, 6.0f, 0.0f)));
	m_pCubeList[3]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(2.0f + m_fValue, 6.0f, 0.0f)));

	// 3
	m_pCubeList[4]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[5]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[6]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[7]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 0.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[8]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 1.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[9]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 2.0f + m_fValue, 5.0f, 0.0f)));
	m_pCubeList[10]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(3.0f + m_fValue, 5.0f, 0.0f)));

	// 4
	m_pCubeList[11]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-4.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[12]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[13]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[14]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 0.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[15]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 1.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[16]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 3.0f + m_fValue, 4.0f, 0.0f)));
	m_pCubeList[17]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 4.0f + m_fValue, 4.0f, 0.0f)));

	// 5
	for (float i = 0; i < 11; i++)
	{
		m_pCubeList[18+i]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(i-5 + m_fValue, 3.0f, 0.0f)));
	}

	// 6
	m_pCubeList[28]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-5.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[29]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[30]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[31]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[32]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 0.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[33]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 1.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[34]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 2.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[35]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 3.0f + m_fValue, 2.0f, 0.0f)));
	m_pCubeList[36]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 5.0f + m_fValue, 2.0f, 0.0f)));

	// 7
	m_pCubeList[37]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-5.0f + m_fValue, 1.0f, 0.0f)));
	m_pCubeList[38]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-3.0f + m_fValue, 1.0f, 0.0f)));
	m_pCubeList[39]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 3.0f + m_fValue, 1.0f, 0.0f)));
	m_pCubeList[40]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 5.0f + m_fValue, 1.0f, 0.0f)));

	//8
	m_pCubeList[41]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-2.0f + m_fValue, 0.0f, 0.0f)));
	m_pCubeList[42]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3(-1.0f + m_fValue, 0.0f, 0.0f)));
	m_pCubeList[43]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 1.0f + m_fValue, 0.0f, 0.0f)));
	m_pCubeList[44]->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3( 2.0f + m_fValue, 0.0f, 0.0f)));

	m_fValue += 0.01f;

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < m_iCubeCount; i++)
	{
		if (m_pCubeList[i] != nullptr) {
			delete m_pCubeList[i];
			m_pCubeList[i] = nullptr;
		}
	}

	//release GUI
	ShutdownGUI();
}