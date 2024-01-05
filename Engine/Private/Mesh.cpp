#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, _int iNumFace, vector<VTXANIMMESH>& Vertices, vector<FACEINDICES32>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<_float4x4>& vecOffsetMatrix, vector<class CBone*> Bones)
{
	strcpy_s(m_szName, strName.c_str());
	m_iMaterialIndex = iMaterialIndex; 

	m_BoneIndices = BoneIndices;
	m_OffsetMatrices = vecOffsetMatrix;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (_int)Vertices.size(); //! ������ ������ �о���� ������.
	
	//!m_iNumIndices = pAIMesh->mNumFaces * 3;y
	m_iNumBones = BoneIndices.size();
	m_iNumIndices = iNumFace * 3; //! mNumFaces�� �ﰢ�� ������. ��, �о���� �ﰢ�� ������ * 3
	m_iIndexStride = 4; //! ���� �ظ��ؼ� ������ 65535���� �Ѿ��. �׷��ϱ� �׳� 4�� Default

	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	if(FAILED(Ready_Vertices_Anim(Vertices, Bones)))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	//! ���� ������ ������ 65535�� ������ �Ѿ�Ŵ� ����Ʈ�� 4�� �Z����. int�� �Ҵ�����
	_uint* pIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	_int iIndiceSize = Indices.size();

	for (size_t i = 0; i < iIndiceSize; i++)
	{
		pIndices[iNumIndices++] = Indices[i]._1;
		pIndices[iNumIndices++] = Indices[i]._2;
		pIndices[iNumIndices++] = Indices[i]._3;

		m_MeshIndices.push_back(Indices[i]);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, _int iNumFace, vector<VTXMESH>& Vertices, vector<FACEINDICES32>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<_float4x4>& vecOffsetMatrix, _matrix PivotMatrix)
{
	m_iMaterialIndex = m_iMaterialIndex; //! AIMesh�� ����ִ� �ε��� �޾ƿ���
	m_OffsetMatrices = vecOffsetMatrix;
	m_BoneIndices = BoneIndices;
	strcpy_s(m_szName, strName.c_str()); //! mName ���� �����Ͱ� ĳ���� �迭�̴� �̸���������

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (_int)Vertices.size(); //! ������ ������ �о���� ������.


	m_iNumIndices = iNumFace * 3; //! mNumFaces�� �ﰢ�� ������. ��, �о���� �ﰢ�� ������ * 3
	m_iIndexStride = 4; //! ���� �ظ��ؼ� ������ 65535���� �Ѿ��. �׷��ϱ� �׳� 4�� Default

	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	if (FAILED(Ready_Vertices_NonAnim(Vertices, PivotMatrix)))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	//! ���� ������ ������ 65535�� ������ �Ѿ�Ŵ� ����Ʈ�� 4�� �Z����. int�� �Ҵ�����
	//! mNumFace�� �ﰢ�� ��������߾���. �ﰢ�� ������ŭ �������Ƽ� �ε��� ���� ä������

	//!�ε����� ��Ӵþ�� i�� ����ϸ� �ȵǴ� ��Ȳ�̴� ���� �ϳ� ������
	_uint iNumIndices = { 0 };
	_uint iNumIndices2 = { 0 };

	//! ���� ������ ������ 65535�� ������ �Ѿ�Ŵ� ����Ʈ�� 4�� �Z����. int�� �Ҵ�����
	_uint* pIndices = new _uint[m_iNumIndices];

	_int iIndiceSize = Indices.size();

	for (size_t i = 0; i < iIndiceSize; i++)
	{
		pIndices[iNumIndices++] = Indices[i]._1;
		pIndices[iNumIndices++] = Indices[i]._2;
		pIndices[iNumIndices++] = Indices[i]._3;

		m_MeshIndices.push_back(Indices[i]);
	}
	
	

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	//TODO ���� ������� ������ ���ڰ����� ���� �޾ƿ��°� ���� ��.
	_float4x4	BoneMatrices[256];
	
	//TODO �����¸�Ʈ������ ����? 
	//! �޽ñ������� ǥ���� ����̴�. ������ ���¸� ǥ���Ѵٱ⺸�ٴ� �޽ñ������� ���� ��ȯ�Ѵٶ�� �����ؾ��Ѵ�.
	//! ����, ���� ���� NPC���� �ְ� NPC���� ������ �ɾ�ٴϰų�, ������ ���� �ɸ� ��� �ִϸ��̼��� ���ؾ� �Ѵ�.
	//! �� �ִϸ��̼� ���� �ϳ��� �����ϴ� ���� NPC���� ���� �� �ִ�.
	//! ����, ����, ���, ���� NPC�� ��� ���� �ִϸ��̼��� �����ϴ� ��찡 �ִ�. 
	//! ���⼭ �߻��ϴ� ������ ��� ü���� �ٸ��� ������ �Ȱ��� �ִϸ��̼��� �����ϱ⿡�� �������� ��찡 �ִ� ��.
	//! ���� ���� ���������� �޽��� ����� �޶����� �����̴�. �� ������ �޽����� ����Ǳ� ���ؼ� �ش� ���� ���¸� �ش���� �̷� ��ĵ��� �� ���ؼ� ������Ѿߵ� ��� ���¸� �����ϴ� ����̴�.
	//! �޽����� ����Ǳ� ���� ������ �ݵ�� ������ ����� ���ؼ� ���������� �ش� �޽����� ��Ȯ�� ������ �� �ִ� ���̴�.
	//! ������ ��Ʈ������ �ٲ��� �����Ƿ� ���� �ε�� �ѹ��� �迭�� ���� �����ص� ���̴�. #�����¸�Ʈ��������

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		//!������ ��Ʈ���� ��������
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

_bool CMesh::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut)
{
	_matrix matWorld = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector vRayPos, vRayDir;

	//! ���콺 ���� ��ġ(������), ����
	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), matWorld);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), matWorld));

	_float fDist;

	_int	iMeshIndex = m_MeshIndices.size();

	for (_uint i = 0; i < iMeshIndex; ++i)
	{
		_vector vPickedPos;

		//�ﰢ��  ���� �ε��� 3��
		_vector vIndexXPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i]._1]);
		_vector vIndexYPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i]._2]);
		_vector vIndexZPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i]._3]);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vIndexXPos, vIndexYPos, vIndexZPos, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;

			vPickedPos = XMVector3TransformCoord(vPickedPos, _WorldMatrix);

			XMStoreFloat3(pOut, vPickedPos);

			return true;
		}
	}

	return false;
}


HRESULT CMesh::Ready_Vertices_NonAnim(vector<VTXMESH>& Vertices, _fmatrix PivotMatrix)
{
	m_iStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &Vertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		m_MeshVertexs.push_back(pVertices[i].vPosition); //! ��ŷ ���� ��ġ �����صα�.

		memcpy(&pVertices[i].vNormal, &Vertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		//TODO �ؽ���� �ּ��߰� #��_�ؽ����_AiMesh
		//! AiMesh�ȿ� �ؽ����� �迭�� ����Ǿ��ְ� 16���� 0x8��ŭ�� �Ҵ��س��Ҵ�.
		//! ���� �� ������ ������ �ִ� �÷��� �ؽ����� �� 8������ ����� �� �ִ�.
		//! �� , �� ������ ����ؼ� �� 8���� �÷�,�ؽ���带 ������ �� �ֵ��� �̸� ������ �迭�� 8����ŭ ���� �س��� ��.
		//! �츮�� �ؽ���带 �ϳ��� ����ϰ� �ֱ⿡ 0��°���� i�� �־��ִ� ��Ȳ�� ���̴�.
		//! ������ ���� ���� ������ �̻��ϰ� ���´ٸ� uv��ǥ�� �߸��� ���°� �ƴ��� �ǽ��غ��� �Ѵ�.
		//! ��Ȳ�� ���� ������ ǥ���ϴ� �ؽ�ó�� �� ��ǻ� �ִ� ���� �ƴϴ�. �����Ʈ�� �븻� ���� �� �ִ�
		//! ������ ǥ���ϴ� �ؽ�ó.png �Ǵ� Ȯ���ڸ� ����Ҵ��� ���� ��� �̹����� ������ �ٸ��� �Ǿ��ֵ��� ���� �޽�
		//! �ƴ϶�� �پ��� UV��ǥ�� ����Ѵٰ� �����ϰ� �۾��ؾ��Ѵ�.
		
		memcpy(&pVertices[i].vTexcoord, &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Vertices[i].vTangent, sizeof(_float3));
		//memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		//memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	
	return S_OK;
}

HRESULT CMesh::Ready_Vertices_Anim(vector<VTXANIMMESH>& Vertices, vector<class CBone*> Bones)
{
	m_iStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices]; //!#���θ޸�����
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &Vertices[i].vPosition, sizeof(_float3));
		m_MeshVertexs.push_back(pVertices[i].vPosition);

		memcpy(&pVertices[i].vNormal, &Vertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &Vertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndices, &Vertices[i].vBlendIndices, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeights, &Vertices[i].vBlendWeights, sizeof(_float4));
	}

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
		
	Safe_Delete_Array(pVertices);

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(m_szName, pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if (iter == Bones.end())
			return E_FAIL;

		//!������ ã�Ƽ� true�� �����ٸ� �ش� index�� ����������
		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity()); //! �ִϸ��̼� ������ ���ʿ� ���⶧���� �����¸�Ʈ������ ���������ʴ´�. �׳� �׵����� �־�����
		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, _int iNumFace, vector<VTXANIMMESH>& Vertices, vector<FACEINDICES32>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<_float4x4>& vecOffsetMatrix, vector<class CBone*> Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDevice, pContext, eModelType, strName, iNumFace, Vertices, Indices, iMaterialIndex, BoneIndices, vecOffsetMatrix, Bones)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, string strName, _int iNumFace, vector<VTXMESH>& Vertices, vector<FACEINDICES32>& Indices, _uint iMaterialIndex, vector<_int>& BoneIndices, vector<_float4x4>& vecOffsetMatrix, _matrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDevice, pContext, eModelType, strName, iNumFace, Vertices, Indices, iMaterialIndex, BoneIndices, vecOffsetMatrix, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();
}

