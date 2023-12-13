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

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones)
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
	HRESULT		hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertices_Anim(pAIMesh, Bones);

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

		m_MeshIndices.push_back( { pAIMesh->mFaces[i].mIndices[0],
								   pAIMesh->mFaces[i].mIndices[1],
								   pAIMesh->mFaces[i].mIndices[2] } );
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

_bool CMesh::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix)
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
		_vector vIndexXPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i].ix]);
		_vector vIndexYPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i].iy]);
		_vector vIndexZPos = XMLoadFloat3(&m_MeshVertexs[m_MeshIndices[i].iz]);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vIndexXPos, vIndexYPos, vIndexZPos, fDist))
		{
			//vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			//XMStoreFloat3(pOut, vPickedPos);

			return true;
		}
	}

	return false;
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
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		m_MeshVertexs.push_back(pVertices[i].vPosition); //! ��ŷ ���� ��ġ �����صα�.

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

HRESULT CMesh::Ready_Vertices_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones)
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
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		m_MeshVertexs.push_back(pVertices[i].vPosition);

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

	m_iNumBones = pAIMesh->mNumBones;

	//! �޽ÿ��� ������ �ִ� ���� ��ȸ�ϸ鼭 ������ ���� � �����鿡�� ������ �ִ��� �ľ��Ѵ�.
	for (size_t i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i]; //! ���ϳ��� �����Դ�.

		_float4x4	OffsetMatrix; //#�����¸�Ʈ��������
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		//! ���������� ��ġ������
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint iBoneIndex = { 0 }; //! ������ �ִ� ���� �ε����� �����س����Ŵ�.

		//TODO CBone Ŭ������ AINode�� �̷�����ְ�, ���� Ŭ������ Mesh Ŭ������ AIBone���� �̷�����ִ�.  AINode�� AIBone�� ����� �ٸ����� ���� �̸��� ����.
		//! �� Ư���� �̿��ؼ� ��Ŭ�������� BoneŬ������ �����ϰ��ִ� Bones ���͸� �����ͼ� ���� �̸��� ���� ���� ã�Ƽ� �ε����� ��������.
		
		auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
			{
				if (false == strcmp(pAIBone->mName.data, pBone->Get_Name()))
				{
					return true;
				}

				++iBoneIndex;

				return false;
			});

		if(iter == Bones.end())
			return E_FAIL;

		//!������ ã�Ƽ� true�� �����ٸ� �ش� index�� ����������
		m_BoneIndices.push_back(iBoneIndex);

		//! ���������� ���� ��� �������� ������ �ִ°�?
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			//!pAIBone->mWeight[j].mVertexId == �� ���� ������ �ִ� j��° ������ �ε���
			//! #���θ޸�����
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

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, _fmatrix PivotMatrix, const vector<class CBone*>& Bones)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, PivotMatrix, Bones)))
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
