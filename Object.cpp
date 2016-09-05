#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

CGameObject::CGameObject()
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_pMesh = NULL; 

	m_nReferences = 0;

	m_bActive = true;
}
CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release(); 
}
void CGameObject::AddRef()
{
	m_nReferences++;
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

D3DXVECTOR3 CGameObject::GetPosition()
{
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}
void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
} 

void CGameObject::Animate(float fTimeElapsed)
{
}
void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	OnPrepareRender();
	CShader::UpdateShaderVariables(pd3dDeviceContext, &m_d3dxmtxWorld);
	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}
D3DXVECTOR3 CGameObject::GetLookAt()
{
	//게임 객체를 로컬 z-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

D3DXVECTOR3 CGameObject::GetUp()
{
	//게임 객체를 로컬 y-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CGameObject::GetRight()
{
	//게임 객체를 로컬 x-축 벡터를 반환한다.
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

void CGameObject::MoveStrafe(float fDistance)
{
	//게임 객체를 로컬 x-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveUp(float fDistance)
{
	//게임 객체를 로컬 y-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveForward(float fDistance)
{
	//게임 객체를 로컬 z-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	//게임 객체를 주어진 각도로 회전한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	//게임 객체를 주어진 회전축을 중심으로 회전한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

//bool CGameObject::IsVisible(CCamera *pCamera)
//{
//	bool bIsVisible = true;
//	AABB bcBoundingCube;
//	if (m_pMesh)
//	{
//		bcBoundingCube = m_pMesh->GetBoundingCube();
//		/*객체의 메쉬의 바운딩 박스(모델 좌표계)를 객체의 월드 변환 행렬로 변환하고 새로운 바운딩 박스를 계산한다.*/
//		bcBoundingCube.Update(&m_d3dxmtxWorld);
//	}
//	//바운딩 박스(월드 좌표계)가 카메라의 절두체에 포함되는 가를 검사한다. 
//	if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
//	return(bIsVisible);
//}

void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//	pd3dxvPickPosition: 카메라 좌표계의 점 (화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	//	pd3dxmtxWorld : 월드 변환 행렬, pd3dxmtxView: 카메라 변환 행렬
	//	pd3dxvPickRayPosition: 픽킹 광선의 시작점. pd3dxvPickRayDirection: 피킹 광선 벡터

	/* 1)객체의 월드 변환 행렬이 주어지면 객체의 월드 변황 행렬과 카메라 변환 행렬을 곱하고, 역행렬을 구한다.
	('이것'은 카메라 변황 행렬의 역행렬과 객체의 월드 변환 행렬의 역행렬의 곱과 같다.)

	객체의 월드 변환 행렬이 주어지지 않으면 카메라 변환 행렬의 역행렬을 구한다. 
	객체의 월드 변환 행렬이 주어지면 모델 좌표계의 픽킹 광선을 구하고, 그렇지 않으면 월드 좌표계의 픽킹 광선을 구한다. */

	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;

	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	
	/* 카메라 좌표계의 원점 (0, 0, 0)을 위에서 구한  역행렬로 변환한다. 
	변환의 결과는 '카메라 좌표계의 원점'에 대응하는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);

	/* 카메라 좌표계의 점을 위에서 구한 역행렬로 변환한다. 
	변환의 결과는 '마우스를 클릭한 점'에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다. */
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);

	// 픽킹 광선의 방향 벡터를 구한다.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}


int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO* pd3dxIntersectInfo)
{
	// pd3dxvPickPosition : 카메라 좌표계의 점(화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	// pd3dxmtxView : 카메라 변환 행렬
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;

	// 활성화된 객체에 대하여 메쉬가 있으면 픽킹 광선을 구하고 객체의 메쉬와 충돌 검사를 한다.
	if (m_bActive && m_pMesh)
	{
		// 객체의 모델 좌표계의 픽킹 광선을 구한다.
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		/* 모델 좌표계의 픽킹 광선과 메쉬의 충돌을 검사한다. 픽킹 광선과 메쉬의 삼각형들은 여러 번 충돌할 수 있다.
		검사의 결과는 충돌된 횟수이다.*/
		nIntersected = m_pMesh->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);
	}
	return (nIntersected);
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	bool bIsVisible = (m_bActive) ? true : false;
	if (m_bActive)
	{
		AABB bcBoundingCube;
		if (m_pMesh) bcBoundingCube = m_pMesh->GetBoundingCube();
		bcBoundingCube.Update(&m_d3dxmtxWorld);
		if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
	}
	return(bIsVisible);
}

#pragma region __CRotatingObject__
CRotatingObject::CRotatingObject()
{
	m_d3dxvRotationAxis = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_d3dxvRotationAxis, m_fRotationSpeed * fTimeElapsed);

}

void CRotatingObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}
#pragma endregion


#pragma region __CRevolvingObject__
CRevolvingObject::CRevolvingObject()
{
	m_d3dxvRevolutionAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 1.0f;
}

CRevolvingObject::~CRevolvingObject()
{
}

void CRevolvingObject::Animate(float fTimeElapsed)
{
	//공전을 나타내기 위해 회전 행렬을 오른쪽에 곱한다.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRevolutionAxis, (float)D3DXToRadian(m_fRevolutionSpeed * fTimeElapsed));
	m_d3dxmtxWorld = m_d3dxmtxWorld * mtxRotate;
}
#pragma endregion