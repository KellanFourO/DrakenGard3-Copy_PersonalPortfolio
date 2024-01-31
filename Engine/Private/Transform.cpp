#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	,m_WorldMatrix(rhs.m_WorldMatrix)
	,m_fRotationPerSec(rhs.m_fRotationPerSec)
	,m_fSpeedPerSec(rhs.m_fSpeedPerSec)
{
}

HRESULT CTransform::Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec)
{
	m_fSpeedPerSec = fSpeedPerSec;
	m_fRotationPerSec = fRotationPerSec;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	//_matrix			matTmp;
	/*
	matTmp = XMLoadFloat4x4(&m_WorldMatrix);*/


	/*_float3		vTmp = _float3(0.f, 0.f, 0.f);
	_float4		vTmp2 = _float4(0.f, 0.f, 0.f, 0.f);

	_vector		vTmp1 = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vTmp3 = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	XMVector3Normalize(vTmp1);

	_float		fDot = XMVectorGetX(XMVector3Dot(vTmp1, vTmp3));

	_float		fLength = XMVectorGetX(XMVector3Length(vTmp1));

	XMStoreFloat3(&vTmp, vTmp1);
	XMStoreFloat4(&vTmp2, vTmp1);*/

	return S_OK;
}

_float3 CTransform::Get_RotateDir(_float3& vBaseDir, _float fAngle)
{
	// 기준이 되는 방향벡터를 XMFLOAT3에서 XMVECTOR로 변환
	XMVECTOR baseDirection = XMLoadFloat3(&vBaseDir);

	// Yaw 회전 행렬 생성
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(fAngle));

	// 회전된 방향벡터 계산
	XMVECTOR rotatedDirection = XMVector3Transform(baseDirection, rotationMatrix);

	// XMVECTOR를 XMFLOAT3로 변환하여 반환
	XMFLOAT3 result;
	XMStoreFloat3(&result, rotatedDirection);

	return result;
}


void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	//TODO 월드행렬 각 행의 길이는 각 x,y,z의 스케일과 같다.
	//! 그래서 기존의 행을 가져와서 스케일과 곱연산을 해주면 스케일이 달라지게 되는 원리
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP,    XMVector3Normalize(Get_State(CTransform::STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK,  XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * fScaleZ);
}

void CTransform::Add_LookPos(_float3& _vAddPos) //! 바라보는 방향으로 위치값을 더해주자
{

	XMStoreFloat3(&_vAddPos, XMVector3TransformNormal(XMLoadFloat3(&_vAddPos), XMLoadFloat4x4(&m_WorldMatrix)));

	m_WorldMatrix._41 += _vAddPos.x;
	m_WorldMatrix._42 += _vAddPos.y;
	m_WorldMatrix._43 += _vAddPos.z;
}

_float3 CTransform::Get_MoveAxisPos(_float fDistance, STATE eState, _bool bAddType, _float3 vCamLook)
{
	
	Look_At_CamLook(vCamLook);

	_float3 vMove = {};

	switch (eState)
	{
	case Engine::CTransform::STATE_RIGHT:
	{
		vMove = bAddType ? _float3(fDistance, 0.f, 0.f) : _float3(-fDistance, 0.f, 0.f);
		break;
	}

	case Engine::CTransform::STATE_UP:
	{
		vMove = bAddType ? _float3(0.f, fDistance, 0.f) : _float3(0.f, -fDistance, 0.f);
		break;
	}

	case Engine::CTransform::STATE_LOOK:
	{
		vMove = bAddType ? _float3(0.f, 0.f, fDistance) : _float3(0.f, 0.f, -fDistance);
		break;
	}

	case Engine::CTransform::STATE_POSITION:
		return _float3(); // 이동하지 않음

	default:
		break;
	}

	// vMove를 vCamLook 기준으로 변환
	// vMove를 vCamLook 방향으로 회전시킨 후 거리를 적용합니다.
	XMVECTOR moveVector = XMLoadFloat3(&vMove);
	XMVECTOR lookVector = XMLoadFloat3(&vCamLook);

	XMVECTOR rotatedMove = XMVector3TransformCoord(moveVector, XMMatrixRotationRollPitchYawFromVector(lookVector));

	_float3 movedPosition;
	XMStoreFloat3(&movedPosition, rotatedMove * fDistance);

	return movedPosition;
}

_float4 CTransform::Get_RandomPositionAroundCenter(_fvector& vCenter, _float fRadius)
{
	// C++11 이상을 지원하는 경우 <random> 헤더를 사용하여 랜덤 위치 생성
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	// 랜덤한 각도 및 거리 생성
	float angle = dis(gen) * 2.0f * 3.14159265358979323846f; // 0에서 2π까지의 랜덤한 각도
	float distance = dis(gen) * fRadius; // 0에서 반경까지의 랜덤한 거리

	// 랜덤한 위치 계산
	float x = vCenter.m128_f32[0] + distance * cos(angle);
	float y = vCenter.m128_f32[1] + distance * sin(angle);
	float z = vCenter.m128_f32[2]; // 중점 기준이므로 z는 그대로 유지f

	// 계산된 위치 반환
	return { x, y, z, 1.f };
}

void CTransform::Go_Player_Straight(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

	//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	Look_At_CamLook(vCamLook);
	_vector vLook = Get_State(STATE_LOOK);
	_vector vPosition = Get_State(STATE_POSITION);



	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//TODO 네비게이션 매쉬 검사
	//! 내가 가려고 하는 위치가 갈수있는 위치인지 검사 하고 갈 수 있는  위치라면 Set_State 해주는 것.
	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
	
	}

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Player_Left(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Player_Right(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	Look_At_CamLook(vCamLook);

	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Player_Backward(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

		//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Player_Up(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Player_Down(_float fTimeDelta, _float3 vCamLook, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);


	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

	//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	_vector vLook = Get_State(STATE_LOOK);
	_vector vPosition = Get_State(STATE_POSITION);

	

	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//TODO 네비게이션 매쉬 검사
	//! 내가 가려고 하는 위치가 갈수있는 위치인지 검사 하고 갈 수 있는  위치라면 Set_State 해주는 것.
	if (nullptr != pNavigation)
	{
		if(false == pNavigation->isMove(vPosition))
			return;
		
	}

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::KeepEye(_float fTimeDelta, _bool bRight, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	if(false == bRight)
		vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	else
		vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, class CNavigation* pNavigation)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

		//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
	
	}

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(CTransform::STATE_UP);


	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Set_State(STATE_POSITION, vPosition);
}



void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	//TODO 임의의축으로 회전하게 하는 함수이다.
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp	   = Get_State(STATE_UP);
	_vector vLook  = Get_State(STATE_LOOK);

	//! 코드를 보면 기존에 방향벡터들을 가져와서 회전시킨것. 스케일 값에 변화는 주지않고 회전만 시켰으니 스케일곱을 수행하지 않는다.

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis,m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix)); 
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}






void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	//TODO 특정 각도로 바라보게 하는 함수이다.
	//! 기존 각도에 누적을 시키면 안된다. 스케일값을 고려한 항등행렬을 만들어서 특정각도를 주자.
	
	_float3 vScale = Get_Scaled();

	_vector vRight	= XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis,fRadian);
	
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::RotationOfCameraDir(_fvector vCamLook, _float fRadian)
{
	//TODO 특정 각도로 바라보게 하는 함수이다.
	//! 기존 각도에 누적을 시키면 안된다. 스케일값을 고려한 항등행렬을 만들어서 특정각도를 주자.


	_float3 vScale = Get_Scaled();
	_matrix	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);

	_vector		vCamBase = XMVectorSetY(vCamLook, 0.f); //vCamLook;
	//_vector		vLook = vCamBase;

	_vector		vLook = XMVector3Normalize(XMVector3TransformNormal(vCamBase, RotationMatrix)) * vScale.z;

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;


	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));



}

void CTransform::RotationAroundPoint(const _float3& vCenter, const _float3& vAxis, _float fAngle)
{
	
	_matrix rotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&vAxis), fAngle);

	// 특정 위치(center)를 기준으로 회전
	_vector positionVector = Get_State(CTransform::STATE_POSITION);
	_vector centerVector = XMLoadFloat3(&vCenter);

	positionVector = XMVector3TransformCoord(positionVector - centerVector, rotationMatrix);
	positionVector += centerVector;

	
	Set_State(CTransform::STATE_POSITION, positionVector);
}

_bool CTransform::TurnToTarget(const _fvector& vTargetPosition, _float fTimeDelta)
{
	_vector vCurrentPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	// 목표 위치와 현재 위치를 벡터로 표현
	_vector vToTarget = vTargetPosition - vCurrentPosition;

	// 목표 방향 벡터를 정규화
	vToTarget = XMVector3Normalize(vToTarget);

	// 현재 방향 벡터와 목표 방향 벡터 간의 각도를 계산
	_float fDot = XMVectorGetX(XMVector3Dot(vLook, vToTarget));

	// 각도가 일정 값 이내라면 이미 목표 지점을 향해 바라보고 있는 것으로 판단
	if (fDot > 0.99f)
	{
		return true;
	}

	// 현재 방향과 목표 방향 벡터 간의 축을 계산wA
	_vector vRotationAxis = XMVector3Normalize(XMVector3Cross(vLook, vToTarget));

	// 회전 각도를 계산 (acos 함수를 이용하여 두 벡터 간의 각도를 계산)
	_float fRotationAngle = acosf(fDot) * fTimeDelta;

	
	// 회전을 수행
	Turn(vRotationAxis, fRotationAngle);

	return false; // 아직 목표 지점을 향해 바라보고 있지 않음
}


void CTransform::Set_Point_Gravity_Velocity(_float3 vTargetPosition, _float3 vPrevPosition, _float fMaxHeight, _float fMaxTime, _Out_ _float3* vOutVelocity, _Out_ _float* fOutGravity)
{
	_float fEndHeight = vTargetPosition.y - vPrevPosition.y;
	_float fCalcHeight = fMaxHeight - vPrevPosition.y;

	_float fEndTime = 0.f;
	_float fCalcGravity = 2 * fCalcHeight / (fMaxTime * fMaxTime);

	_float fA, fB, fC;

	vOutVelocity->y = sqrtf(2 * fCalcGravity * fCalcHeight);

	fA = fCalcGravity;
	fB = -2 * vOutVelocity->y;
	fC = 2 * fEndHeight;

	_float fDelta;
	fDelta = fB * fB - 4 * fA * fC;

	if (fDelta >= 0) {
		fEndTime = (-fB + sqrtf(fDelta)) / (2 * fA);
	}
	else {
		// 음수인 경우에 대한 처리
		fEndTime = fabs((-fB + sqrtf(-fDelta)) / (2 * fA));  // 음수를 고려하여 sqrtf(-delta)
	}

	//fEndTime = (-fB + sqrtf(fB * fB - 4 * fA * fC)) / (2 * fA);

	vOutVelocity->x = -(vPrevPosition.x - vTargetPosition.x) / fEndTime;
	vOutVelocity->z = -(vPrevPosition.z - vTargetPosition.z) / fEndTime;

	*fOutGravity = fCalcGravity;

}

void CTransform::Point_Parabola(_float3 vTargetPos, _float3 vPrevPos, _float3 vVelocity, _float fTimeAcc, _float fGravity)
{
	_float4 vTargetPosW = {vTargetPos.x, vTargetPos.y, vTargetPos.z, 1.f};

	_vector vPos = Get_State(CTransform::STATE_POSITION);

	vPos.m128_f32[0] = vPrevPos.x + vVelocity.x * fTimeAcc;
	vPos.m128_f32[1] = vPrevPos.y + (vVelocity.y * fTimeAcc) - (0.5f * fGravity * fTimeAcc * fTimeAcc);
	vPos.m128_f32[2] = vPrevPos.z + vVelocity.z * fTimeAcc;

	//Look_At(XMLoadFloat4(&vTargetPosW));\
	wDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDrwRDDD
	Set_State(CTransform::STATE_POSITION, vPos);
}


void CTransform::Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare)
{
	//TODO 타겟 위치에 도달할때 까지 추적

	_vector vPosition	= Get_State(STATE_POSITION);
	_vector	vDir		= vTargetPos - vPosition;

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if(fDistance >= fSpare)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;


	Look_At(vTargetPos);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Target_Speed(_fvector vTargetPos, _float fTimeDelta, _float fSpeed, _float fSpare)
{
	//TODO 타겟 위치에 도달할때 까지 추적

	_vector vPosition = Get_State(STATE_POSITION);
	_vector	vDir = vTargetPos - vPosition;

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= fSpare)
		vPosition += XMVector3Normalize(vDir) * fSpeed * fTimeDelta;


	Look_At(vTargetPos);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Target_Navi(_fvector vTargetPos, _float fTimeDelta, CNavigation* pNavigation, _float fSpare)
{
	//TODO 타겟 위치에 도달할때 까지 추적

	_vector vPosition = Get_State(STATE_POSITION);
	_vector	vDir = vTargetPos - vPosition;

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance >= fSpare)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavigation)
	{
		if (false == pNavigation->isMove(vPosition))
			return;
		
	}

	Look_At(vTargetPos);
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Look_At(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled(); //! 기존 스케일값
	
	_vector		vPosition = Get_State(STATE_POSITION);
	
	_vector		vLook = XMVector3Normalize(vTargetPos - vPosition) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook,vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At_CamLook(_float3 vCamLook)
{
	_float3 vMyScale = Get_Scaled();

	_vector vMyLook = Get_State(CTransform::STATE_LOOK);
	

	_vector vNewLook = XMVector3Normalize(XMVectorLerp(vMyLook, XMLoadFloat3(&vCamLook), 0.1f) * vMyScale.z);
	vNewLook.m128_f32[1] = vMyLook.m128_f32[1];

	_vector	vMyRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLook)) * vMyScale.x;
	_vector	vMyUp = XMVector3Normalize(XMVector3Cross(vNewLook, vMyRight)) * vMyScale.y;

	Set_State(STATE_RIGHT, vMyRight);
	Set_State(STATE_UP, vMyUp);
	Set_State(STATE_LOOK, vNewLook);
}

void CTransform::Look_At_Dir(_vector _vLook)
{
	
	_vector vScale = XMLoadFloat3(&Get_Scaled());

	_vector vLook = XMVector3Normalize(_vLook);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Look_At_Dir(_float3 _vLook)
{
	_vector vScale = XMLoadFloat3(&Get_Scaled());

	_vector vLook = XMVector3Normalize(XMLoadFloat3(&_vLook));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Look_At_OnLand(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f))) * vScale.z;

	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

_bool CTransform::HasArrived(const DirectX::XMFLOAT3& _vCurrentPos, const DirectX::XMFLOAT3& _vTargetPos, _float fArrivalThreshold)
{
	DirectX::XMVECTOR vCurrentPos = DirectX::XMLoadFloat3(&_vCurrentPos);
	DirectX::XMVECTOR vTargetPos = DirectX::XMLoadFloat3(&_vTargetPos);

	// 두 지점 간의 거리를 계산
	DirectX::XMVECTOR vDistance = DirectX::XMVector3Length(DirectX::XMVectorSubtract(vTargetPos, vCurrentPos));

	// 거리가 arrivalThreshold 이하이면 도착으로 간주
	return DirectX::XMVectorGetX(vDistance) <= fArrivalThreshold;
}

void CTransform::Translate(const _float3& vTranslation, class CNavigation* pNavigation, _bool bNotAgent)
{
	/* Check NavMeshAgent */
	if (nullptr != pNavigation)
	{
		_vector vPos = Get_State(CTransform::STATE_POSITION);
		
		

		vPos = XMLoadFloat3(&vTranslation) + vPos;
		
		
		
		//Vec3 vTemp = vTranslation + Vec4(m_WorldMatrix.m[3]).xyz();

		if (false == pNavigation->isMove(vPos))
			return;

		

		_float3 vRealPos;
		XMStoreFloat3(&vRealPos, vPos);

		m_vTranslatePos = vRealPos;
		
		
		_float fY = pNavigation->Compute_Height(vRealPos, &m_isGround);
		
		if(m_isGround == true)
			vPos.m128_f32[1] = fY;
		else
			_int i =0;

		Set_State(CTransform::STATE_POSITION, vPos);
	}

		//vPos = vPos + XMLoadFloat3(&vTranslation);
		//
		//Set_State(CTransform::STATE_POSITION, vPos);
	
}



void CTransform::Write_Json(json& Out_Json)
{
	Out_Json.emplace("Transform", m_WorldMatrix.m);
	Out_Json.emplace("SpeedPerSec", m_fSpeedPerSec);
	Out_Json.emplace("RotationPerSec", m_fRotationPerSec);
}

void CTransform::Load_FromJson(const json& In_Json)
{
	if (In_Json.find("Transform") == In_Json.end())
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_WorldMatrix.m[i][j] = In_Json["Transform"][i][j];
		}
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSpeedPerSec, _float fRotationPerSec)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(fSpeedPerSec,fRotationPerSec)))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	int i = 0;
	__super::Free();
}
