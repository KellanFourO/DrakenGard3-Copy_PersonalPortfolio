#include "Mesh.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice,pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex; //! AIMesh�� ����ִ� �ε��� �޾ƿ���

	strcpy_s(m_szName, pAIMesh->mName.data); //! mName ���� �����Ͱ� ĳ���� �迭�̴� �̸���������

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices; //! ������ ������ �о���� ������.

	m_iNumIndices = pAIMesh->mNumFaces * 3; //! mNumFaces�� �ﰢ�� ������. ��, �о���� �ﰢ�� ������ * 3
	m_iIndexStride = 4; //! ���� �ظ��ؼ� ������ 65535���� �Ѿ��. �׷��ϱ� �׳� 4�� Default
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh,PivotMatrix) : Ready_Vertices_Anim(pAIMesh);

	if(FAILED(hr))
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

	//! mNumFace�� �ﰢ�� ��������߾���. �ﰢ�� ������ŭ �������Ƽ� �ε��� ���� ä������
	
	//!�ε����� ��Ӵþ�� i�� ����ϸ� �ȵǴ� ��Ȳ�̴� ���� �ϳ� ������
	_uint iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		//!mFaces���� �ﰢ��(������) ������ ���ǵǾ��ְ� ���߿� mIndices�� �ﰢ�� ������ ����մ� �迭�̴� �������
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

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

HRESULT CMesh::Ready_Vertices_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
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

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
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

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	
	return S_OK;
}

HRESULT CMesh::Ready_Vertices_Anim(const aiMesh* pAIMesh)
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

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));

		//TODO �ؽ���� �ּ��߰� #��_�ؽ����_AiMesh
		//! AiMesh�ȿ� �ؽ����� �迭�� ����Ǿ��ְ� 16���� 0x8��ŭ�� �Ҵ��س��Ҵ�.
		//! ���� �� ������ ������ �ִ� �÷��� �ؽ����� �� 8������ ����� �� �ִ�.
		//! �� , �� ������ ����ؼ� �� 8���� �÷�,�ؽ���带 ������ �� �ֵ��� �̸� ������ �迭�� 8����ŭ ���� �س��� ��.
		//! �츮�� �ؽ���带 �ϳ��� ����ϰ� �ֱ⿡ 0��°���� i�� �־��ִ� ��Ȳ�� ���̴�.
		//! ������ ���� ���� ������ �̻��ϰ� ���´ٸ� uv��ǥ�� �߸��� ���°� �ƴ��� �ǽ��غ��� �Ѵ�.
		//! ��Ȳ�� ���� ������ ǥ���ϴ� �ؽ�ó�� �� ��ǻ� �ִ� ���� �ƴϴ�. �����Ʈ�� �븻� ���� �� �ִ�
		//! ������ ǥ���ϴ� �ؽ�ó.png �Ǵ� Ȯ���ڸ� ����Ҵ��� ���� ��� �̹����� ������ �ٸ��� �Ǿ��ֵ��� ���� �޽�
		//! �ƴ϶�� �پ��� UV��ǥ�� ����Ѵٰ� �����ϰ� �۾��ؾ��Ѵ�.

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	//! �޽ÿ��� ������ �ִ� ���� ��ȸ�ϸ鼭 ������ ���� � �����鿡�� ������ �ִ��� �ľ��Ѵ�.
	for (size_t i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i]; //! ���ϳ��� �����Դ�.

		//! ���������� ���� ��� �������� ������ �ִ°�?
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			//!pAIBone->mWeight[j].mVertexId == �� ���� ������ �ִ� j��° ������ �ε���
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x) //! �ش� ������ ����ġ���� ������ �ȵƴٸ�
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i; 
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight; //! ����ġ ä������
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y) 
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight; 
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices�� �Ҵ��Ͽ� ä������ �������� ������ ID3D11Buffer�� �Ҵ��� ������ �����Ͽ� ä���ִ´�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix)))
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
