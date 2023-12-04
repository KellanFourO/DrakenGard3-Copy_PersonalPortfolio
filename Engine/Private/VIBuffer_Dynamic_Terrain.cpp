#include "VIBuffer_Dynamic_Terrain.h"
#include "GameInstance.h"

CVIBuffer_Dynamic_Terrain::CVIBuffer_Dynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_Dynamic_Terrain::CVIBuffer_Dynamic_Terrain(const CVIBuffer_Dynamic_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

_bool CVIBuffer_Dynamic_Terrain::Get_Vertex(_uint _iIndex, VTXDYNAMIC* _pOut)
{
	if (m_VertexInfo.size() <= _iIndex)
		return false;

	*_pOut = m_VertexInfo[_iIndex];

	return true;
}

_bool CVIBuffer_Dynamic_Terrain::Get_Indices(_uint _iIndex, _uint3* _pOut)
{
	if (m_Indices.size() <= _iIndex)
		return false;

	*_pOut = m_Indices[_iIndex];

	return true;
}

_bool CVIBuffer_Dynamic_Terrain::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut)
{
	_matrix matWorld = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector vRayPos, vRayDir;

	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), matWorld);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), matWorld));

	_float fDist;

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_uint3 iIndices = m_Indices[i];
		_vector vPickedPos;

		_vector vPosX = XMLoadFloat3(&m_VertexInfo[iIndices.ix].vPosition);
		_vector vPosY = XMLoadFloat3(&m_VertexInfo[iIndices.iy].vPosition);
		_vector vPosZ = XMLoadFloat3(&m_VertexInfo[iIndices.iz].vPosition);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vPosX, vPosY, vPosZ, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			XMStoreFloat3(pOut, vPickedPos);

			return true;
		}
	}
	
	return false;
}

HRESULT CVIBuffer_Dynamic_Terrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Dynamic_Terrain::Initialize(void* pArg)
{
	VTXDYNAMIC* pInfo = (VTXDYNAMIC*)pArg;

	m_fInterval = pInfo->vPosition.z;
	m_iNumVerticesX = (_uint)pInfo->vPosition.x;
	m_iNumVerticesZ = (_uint)pInfo->vPosition.z;

	m_iStride = sizeof(VTXDYNAMIC);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	//!  버텍스 버퍼 시작
	VTXDYNAMIC* pVertices = new VTXDYNAMIC[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXDYNAMIC) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(_float(j * m_fInterval), 0.f, _float(i * m_fInterval));
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTangent = _float3(1.f, 0.f, 0.f);

			m_VertexInfo.push_back(pVertices[iIndex]);
		}
	}
	//!버텍스 버퍼 종료

	//! 인덱스 버퍼 시작
	m_iIndexStride = sizeof(FACEINDICES32);
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPrimitive * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_ulong  iNumIndices = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong iIndex = i * m_iNumVerticesX + j;

			_ulong iIndices[] =
			{
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumIndices] = { iIndices[0], iIndices[1], iIndices[2] };
			m_Indices.push_back(_uint3(pIndices[iNumIndices]._1, pIndices[iNumIndices]._2, pIndices[iNumIndices]._3));
			++iNumIndices;

			pIndices[iNumIndices] = { iIndices[0], iIndices[2], iIndices[3] };
			m_Indices.push_back(_uint3(pIndices[iNumIndices]._1, pIndices[iNumIndices]._2, pIndices[iNumIndices]._3));
			++iNumIndices;
		}
	}
	//! 인덱스 버퍼 종료


	//! 정점 버퍼 시작
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;
	//!정점 버퍼 종료

	//!인덱스 버퍼 시작
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;
	//! 인덱스 버퍼 종료

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

void CVIBuffer_Dynamic_Terrain::Tick(_vector _vMousePos, _float _fRadious, _float _fPower, _uint _iMode)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;

	//TODO MAP
	//! 하위 리소스에 포함된 데이터에 대한 포인터를 가져오고, GPU 액세스를 거부한다.
	
	//!인터페이스 포인터 : 바꾸려는 녀석의 주소(2D텍스처,버퍼 등등 ID3D11Resource 상속받는 객체 주소)
	//! 하위 리소스 인덱스 번호 : 만약, 밉맵이 여러개 일때의 어떤 밉레벨을 바꿀 건지.
	//! CPU 권한 : DISCARD 또는 NO_OVERWRTE
	//! DISCARD : 기존값 버리고 새로채우기
	//! NO_OVERWRITE :  기존값 유지하고, 추가적으로 값 채우기
	//! GPU가 사용중일 때 수행하는 작업 지정 : 0 고정
	//! 출력 값 : D3D11_MAPPED_SUBRESOURCE. MAP을 통해 얻어온 값을 출력해줄 구조체 포인터
	
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_iNumVerticesX = LONG(512);
	m_iNumVerticesZ = LONG(512);

	
}


CVIBuffer_Dynamic_Terrain* CVIBuffer_Dynamic_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Dynamic_Terrain* pInstance = new CVIBuffer_Dynamic_Terrain(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Dynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Dynamic_Terrain::Clone(void* pArg)
{
	CVIBuffer_Dynamic_Terrain* pInstance = new CVIBuffer_Dynamic_Terrain(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Dynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Dynamic_Terrain::Free()
{
}
