#include "Mesh.h" 
#include "Camera.h"

#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x10
#define DIR_DOWN	0x20

#pragma once
class CGameObject
{
private:
	bool m_bActive;
public:
	void SetActive(bool bActive = false) {	m_bActive = bActive;	}

	// 월드 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(D3DXVECTOR3* pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX* pd3dxmtxView, D3DXVECTOR3* pd3dxvPickRayPosition, D3DXVECTOR3* pd3dxvPickRayDirection);
	// 월드 좌표계의 픽킹 광선을 생성한다.
	int PickObjectByRayIntersection(D3DXVECTOR3* pd3dxvPickPosition, D3DXMATRIX* pd3dxmtxView, MESHINTERSECTINFO* pd3dxIntersectinfo);
	 
public:
	//객체가 카메라의 절두체 내부에 있는 가를 판단한다. 
	bool IsVisible(CCamera *pCamera = NULL);

public:
	// 객체는 하나의 메쉬를 가지는 것이 아니라 여러 개의 메쉬를 가질 수 있다.
	CGameObject(int nMeshes = 0);	// nMeshes는 객체가 가지는 메쉬의 개수이다.
	virtual ~CGameObject();

private:
	int m_nReferences;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld; 
	  
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//객체의 위치를 설정한다.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	D3DXVECTOR3 GetPosition();

public:
	//로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//로컬 x-축, y-축, z-축 방향으로 회전한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	//객체의 위치, 로컬 x-축, y-축, z-축 방향 벡터를 반환한다.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	//객체를 렌더링하기 전에 호출되는 함수이다.
	virtual void OnPrepareRender() { }
public:
	// 객체가 가지는 메쉬들데 대한 포인터와 그 개수이다.
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
protected:
	//자전 속도와 회전축 벡터를 나타내는 멤버 변수를 선언한다.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;

public:
	//자전 속도와 회전축 벡터를 설정하는 함수이다.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }

};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes = 1);
	virtual ~CRevolvingObject();

	virtual void Animate(float fTimeElapsed);

private:
	// 공전 회전축과 회전 속력을 나타낸다.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// 공전 속력을 설정한다.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// 공전을 위한 회전축을 설정한다.
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }
};