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
	//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

D3DXVECTOR3 CGameObject::GetUp()
{
	//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

D3DXVECTOR3 CGameObject::GetRight()
{
	//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

void CGameObject::MoveStrafe(float fDistance)
{
	//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveUp(float fDistance)
{
	//���� ��ü�� ���� y-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveForward(float fDistance)
{
	//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	//���� ��ü�� �־��� ������ ȸ���Ѵ�.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	//���� ��ü�� �־��� ȸ������ �߽����� ȸ���Ѵ�.
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
//		/*��ü�� �޽��� �ٿ�� �ڽ�(�� ��ǥ��)�� ��ü�� ���� ��ȯ ��ķ� ��ȯ�ϰ� ���ο� �ٿ�� �ڽ��� ����Ѵ�.*/
//		bcBoundingCube.Update(&m_d3dxmtxWorld);
//	}
//	//�ٿ�� �ڽ�(���� ��ǥ��)�� ī�޶��� ����ü�� ���ԵǴ� ���� �˻��Ѵ�. 
//	if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
//	return(bIsVisible);
//}

void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//	pd3dxvPickPosition: ī�޶� ��ǥ���� �� (ȭ�� ��ǥ�迡�� ���콺�� Ŭ���� ���� ����ȯ�� ��)
	//	pd3dxmtxWorld : ���� ��ȯ ���, pd3dxmtxView: ī�޶� ��ȯ ���
	//	pd3dxvPickRayPosition: ��ŷ ������ ������. pd3dxvPickRayDirection: ��ŷ ���� ����

	/* 1)��ü�� ���� ��ȯ ����� �־����� ��ü�� ���� ��Ȳ ��İ� ī�޶� ��ȯ ����� ���ϰ�, ������� ���Ѵ�.
	('�̰�'�� ī�޶� ��Ȳ ����� ����İ� ��ü�� ���� ��ȯ ����� ������� ���� ����.)

	��ü�� ���� ��ȯ ����� �־����� ������ ī�޶� ��ȯ ����� ������� ���Ѵ�. 
	��ü�� ���� ��ȯ ����� �־����� �� ��ǥ���� ��ŷ ������ ���ϰ�, �׷��� ������ ���� ��ǥ���� ��ŷ ������ ���Ѵ�. */

	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;

	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	
	/* ī�޶� ��ǥ���� ���� (0, 0, 0)�� ������ ����  ����ķ� ��ȯ�Ѵ�. 
	��ȯ�� ����� 'ī�޶� ��ǥ���� ����'�� �����ϴ� �� ��ǥ���� �� �Ǵ� ���� ��ǥ���� ���̴�.*/
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);

	/* ī�޶� ��ǥ���� ���� ������ ���� ����ķ� ��ȯ�Ѵ�. 
	��ȯ�� ����� '���콺�� Ŭ���� ��'�� �����Ǵ� �� ��ǥ���� �� �Ǵ� ���� ��ǥ���� ���̴�. */
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);

	// ��ŷ ������ ���� ���͸� ���Ѵ�.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}


int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO* pd3dxIntersectInfo)
{
	// pd3dxvPickPosition : ī�޶� ��ǥ���� ��(ȭ�� ��ǥ�迡�� ���콺�� Ŭ���� ���� ����ȯ�� ��)
	// pd3dxmtxView : ī�޶� ��ȯ ���
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;

	// Ȱ��ȭ�� ��ü�� ���Ͽ� �޽��� ������ ��ŷ ������ ���ϰ� ��ü�� �޽��� �浹 �˻縦 �Ѵ�.
	if (m_bActive && m_pMesh)
	{
		// ��ü�� �� ��ǥ���� ��ŷ ������ ���Ѵ�.
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		/* �� ��ǥ���� ��ŷ ������ �޽��� �浹�� �˻��Ѵ�. ��ŷ ������ �޽��� �ﰢ������ ���� �� �浹�� �� �ִ�.
		�˻��� ����� �浹�� Ƚ���̴�.*/
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
	//������ ��Ÿ���� ���� ȸ�� ����� �����ʿ� ���Ѵ�.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, &m_d3dxvRevolutionAxis, (float)D3DXToRadian(m_fRevolutionSpeed * fTimeElapsed));
	m_d3dxmtxWorld = m_d3dxmtxWorld * mtxRotate;
}
#pragma endregion