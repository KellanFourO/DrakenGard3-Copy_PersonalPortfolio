#include "VIBuffer_Ground.h"
#include "GameInstance.h"

CVIBuffer_Ground::CVIBuffer_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Ground::CVIBuffer_Ground(const CVIBuffer_Ground& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Ground::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	return S_OK;
}

HRESULT CVIBuffer_Ground::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Ground::Get_Vertex(_uint _iIndex, VTXGROUND* _pOut)
{
	if (m_VertexInfo.size() <= _iIndex)
		return false;

	*_pOut = m_VertexInfo[_iIndex];

	return true;
}

_bool CVIBuffer_Ground::Get_Indices(_uint _iIndex, _uint3* _pOut)
{
	if (m_Indices.size() <= _iIndex)
		return false;

	*_pOut = m_Indices[_iIndex];

	return true;
}

void CVIBuffer_Ground::Update(_vector _vMousePos, _float _fRadious, _float _fPower, _uint _iMode)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_int2	iPickIndex  = { _int(_vMousePos.m128_f32[0] / m_fInterval), _int(_vMousePos.m128_f32[2] / m_fInterval) };
	_int	iRoundIndx  = (_int)(_fRadious / m_fInterval);

	_int2	iBeginIndex, iEndIndex;
	iBeginIndex.x = (0 > iPickIndex.x - iRoundIndx) ? (0) : (iPickIndex.x - iRoundIndx);
	iBeginIndex.y = (0 > iPickIndex.y - iRoundIndx) ? (0) : (iPickIndex.y - iRoundIndx);
	 														
	iEndIndex.x   = ((_int)m_iNumVerticesX < iPickIndex.x + iRoundIndx) ? (m_iNumVerticesX) : (iPickIndex.x + iRoundIndx);
	iEndIndex.y   = ((_int)m_iNumVerticesZ < iPickIndex.y + iRoundIndx) ? (m_iNumVerticesZ) : (iPickIndex.y + iRoundIndx);

	for (_uint iZ(iBeginIndex.y); iZ < (_uint)iEndIndex.y; ++iZ)
	{
		for (_uint iX(iBeginIndex.x); iX < (_uint)iEndIndex.x; ++iX)
		{
			_ulong	iIndex = iZ * m_iNumVerticesX + iX;

			_float3 vPos    = ((VTXGROUND*)SubResource.pData)[iIndex].vPosition;
			_float  fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - _vMousePos));
		
			switch (_iMode)
			{
				case 0: 
				{
					if (_fRadious < fLength)
						continue;

					((VTXGROUND*)SubResource.pData)[iIndex].vPosition.y += _fPower;
					m_VertexInfo[iIndex].vPosition = ((VTXGROUND*)SubResource.pData)[iIndex].vPosition;
				}
				break;

				case 1:
				{
					if (_fRadious < fLength)
						continue;

					_float fLerpPower = _fPower * (1.f - pow((fLength / _fRadious), 2.f));

					((VTXGROUND*)SubResource.pData)[iIndex].vPosition.y += fLerpPower;
					m_VertexInfo[iIndex].vPosition = ((VTXGROUND*)SubResource.pData)[iIndex].vPosition;
				}
				break;

				case 2:
				{
					((VTXGROUND*)SubResource.pData)[iIndex].vPosition.y = _fPower;
					m_VertexInfo[iIndex].vPosition = ((VTXGROUND*)SubResource.pData)[iIndex].vPosition;
				}
				break;
			}		
		}
	}

	for (_uint iZ(iBeginIndex.y); iZ < (_uint)iEndIndex.y; ++iZ)
	{
		for (_uint iX(iBeginIndex.x); iX < (_uint)iEndIndex.x; ++iX)
		{
			_ulong	iIndex = iZ * m_iNumVerticesX + iX;

			_long  iAdjacency[] =
			{
				_long(iIndex + m_iNumVerticesX),	// 위
				_long(iIndex + 1),					// 오른쪽
				_long(iIndex - m_iNumVerticesX),	// 아래
				_long(iIndex - 1)					// 왼쪽
			};
			
			if (0 == iX)
				iAdjacency[3] = -1;

			if (m_iNumVerticesX - 1 == iX)
				iAdjacency[1] = -1;

			if (0 == iZ)
				iAdjacency[2] = -1;

			if (m_iNumVerticesZ - 1 == iZ)
				iAdjacency[0] = -1;

			_float3 vNorm = m_VertexInfo[iIndex].vNormal;

			// 노말 벡터 계산
			_vector vComputeNorm = XMVectorSet(0.f, 0.f, 0.f, 0.f);

			for (_uint i = 0; i < 4; ++i)
			{
				_uint iNext = (3 == i) ? (0) : (i + 1);

				if (0 > iAdjacency[i] || 0 > iAdjacency[iNext])
					continue;

				_vector vLine_no1 = XMLoadFloat3(&m_VertexInfo[iAdjacency[i]].vPosition)     - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition);
				_vector vLine_no2 = XMLoadFloat3(&m_VertexInfo[iAdjacency[iNext]].vPosition) - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition);
				_vector vLingNorm = XMVector3Normalize(XMVector3Cross(vLine_no1, vLine_no2));

				vComputeNorm = XMVector3Normalize(vComputeNorm + vLingNorm);
			}

			XMStoreFloat3(&vNorm, vComputeNorm);

			m_VertexInfo[iIndex].vNormal = vNorm;
			((VTXGROUND*)SubResource.pData)[iIndex].vNormal = vNorm;

			if (0 > iAdjacency[1])
				continue;

			// 탄젠트 벡터 계산
			_float3 vTempTangent;
			XMStoreFloat3(&vTempTangent, XMVector3Normalize(XMLoadFloat3(&m_VertexInfo[iAdjacency[1]].vPosition) - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition)));
			
			m_VertexInfo[iIndex].vTangent = vTempTangent;
			((VTXGROUND*)SubResource.pData)[iIndex].vTangent = vTempTangent;
		}
	}

	m_pContext->Unmap(m_pVB, 0);
}

_bool CVIBuffer_Ground::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut)
{
	_matrix		WorldMatrix = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector		vRayPos, vRayDir;

	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_uint3		iIndices = m_Indices[i];
		_vector		vPickedPos;

		_vector	vVec0 = XMLoadFloat3(&m_VertexInfo[iIndices.ix].vPosition);
		_vector	vVec1 = XMLoadFloat3(&m_VertexInfo[iIndices.iy].vPosition);
		_vector	vVec2 = XMLoadFloat3(&m_VertexInfo[iIndices.iz].vPosition);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			XMStoreFloat3(pOut, vPickedPos);

			return true;
		}
	}

	return false;
}

_bool CVIBuffer_Ground::Compute_MouseRatio(RAY _Ray, _matrix _WorldMatrix, _float2* pOut)
{
	_matrix		WorldMatrix = XMMatrixInverse(nullptr, _WorldMatrix);

	_vector			vRayPos, vRayDir;

	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;
	_float2		vMaxRange	= { m_iNumVerticesX * m_fInterval, m_iNumVerticesZ * m_fInterval };

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_uint3		iIndices = m_Indices[i];
		_vector		vPickedPos;

		_vector	vVec0 = XMLoadFloat3(&m_VertexInfo[iIndices.ix].vPosition);
		_vector	vVec1 = XMLoadFloat3(&m_VertexInfo[iIndices.iy].vPosition);
		_vector	vVec2 = XMLoadFloat3(&m_VertexInfo[iIndices.iz].vPosition);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;

			pOut->x = vPickedPos.m128_f32[0] / vMaxRange.x;
			pOut->y = vPickedPos.m128_f32[2] / vMaxRange.y;

			return true;
		}
	}

	return false;
}

_bool CVIBuffer_Ground::Compute_IsInTerrain(_uint _iNumVertexX, _uint _iNumVertexZ, _float _fInterval, _fvector _vTargetPos, _fmatrix _WorldMatrix)
{
	_float vRangeX = _iNumVertexX * _fInterval;
	_float vRangeZ = _iNumVertexZ * _fInterval;

	_matrix WorldMatrixInv(XMMatrixInverse(nullptr, _WorldMatrix));
	_float3 vLocalPos;
	XMStoreFloat3(&vLocalPos, XMVector3TransformCoord(_vTargetPos, WorldMatrixInv));

	if ((0.f < vLocalPos.x && vRangeX > vLocalPos.x) &&
		(0.f < vLocalPos.z && vRangeZ > vLocalPos.z))
	{
		return true;
	}

	return false;
}


CVIBuffer_Ground* CVIBuffer_Ground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath)
{
	CVIBuffer_Ground* pInstance = new CVIBuffer_Ground(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Ground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Ground::Clone(void* pArg)
{
	CVIBuffer_Ground* pInstance = new CVIBuffer_Ground(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Ground");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Ground::Free()
{
}
