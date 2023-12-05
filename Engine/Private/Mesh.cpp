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
	m_iMaterialIndex = pAIMesh->mMaterialIndex; //! AIMesh가 들고있는 인덱스 받아오자

	strcpy_s(m_szName, pAIMesh->mName.data); //! mName 안의 데이터가 캐릭터 배열이다 이름가져오자

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices; //! 정점의 개수는 읽어들인 개수다.

	m_iNumIndices = pAIMesh->mNumFaces * 3; //! mNumFaces가 삼각형 개수다. 즉, 읽어들인 삼각형 개수의 * 3
	m_iIndexStride = 4; //! 모델은 왠만해선 정점이 65535개를 넘어간다. 그러니까 그냥 4로 Default
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

	//! 모델의 정점의 개수가 65535는 무조건 넘어갈거니 디폴트로 4로 줫엇다. int로 할당하자
	_uint* pIndices = new _uint[m_iNumIndices];

	//! mNumFace가 삼각형 개수라고했엇다. 삼각형 개수만큼 루프돌아서 인덱스 정보 채워주자
	
	//!인덱스는 계속늘어나고 i를 사용하면 안되는 상황이다 변수 하나 더쓰자
	_uint iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		//!mFaces안의 삼각형(폴리곤) 정보가 정의되어있고 그중에 mIndices는 삼각형 정보를 담고잇는 배열이다 사용하자
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

		//TODO 텍스쿠드 주석추가 #모델_텍스쿠드_AiMesh
		//! AiMesh안에 텍스쿠드는 배열로 선언되어있고 16진수 0x8만큼을 할당해놓았다.
		//! 원래 한 정점이 가질수 있는 컬러와 텍스쿠드는 총 8개까지 선언될 수 있다.
		//! 즉 , 위 사항을 고려해서 총 8개의 컬러,텍스쿠드를 가져올 수 있도록 미리 벡터의 배열을 8개만큼 선언 해놓은 것.
		//! 우리는 텍스쿠드를 하나만 사용하고 있기에 0번째에서 i를 넣어주는 상황인 것이다.
		//! 하지만 만약 모델의 색깔이 이상하게 들어온다면 uv좌표가 잘못된 상태가 아닌지 의심해봐야 한다.
		//! 상황에 따라 재질을 표현하는 텍스처가 꼭 디퓨즈만 있는 것이 아니다. 엠비언트나 노말등도 있을 수 있다
		//! 재질을 표현하는 텍스처.png 또는 확장자를 열어보았더니 같은 모양 이미지가 색깔이 다르게 되어있따면 같은 메시
		//! 아니라면 다양한 UV좌표를 사용한다고 생각하고 작업해야한다.

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_SubResourceData.pSysMem = pVertices;

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
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

		//TODO 텍스쿠드 주석추가 #모델_텍스쿠드_AiMesh
		//! AiMesh안에 텍스쿠드는 배열로 선언되어있고 16진수 0x8만큼을 할당해놓았다.
		//! 원래 한 정점이 가질수 있는 컬러와 텍스쿠드는 총 8개까지 선언될 수 있다.
		//! 즉 , 위 사항을 고려해서 총 8개의 컬러,텍스쿠드를 가져올 수 있도록 미리 벡터의 배열을 8개만큼 선언 해놓은 것.
		//! 우리는 텍스쿠드를 하나만 사용하고 있기에 0번째에서 i를 넣어주는 상황인 것이다.
		//! 하지만 만약 모델의 색깔이 이상하게 들어온다면 uv좌표가 잘못된 상태가 아닌지 의심해봐야 한다.
		//! 상황에 따라 재질을 표현하는 텍스처가 꼭 디퓨즈만 있는 것이 아니다. 엠비언트나 노말등도 있을 수 있다
		//! 재질을 표현하는 텍스처.png 또는 확장자를 열어보았더니 같은 모양 이미지가 색깔이 다르게 되어있따면 같은 메시
		//! 아니라면 다양한 UV좌표를 사용한다고 생각하고 작업해야한다.

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	//! 메시에게 영향을 주는 뼈를 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.
	for (size_t i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i]; //! 뼈하나를 가져왔다.

		//! 위에가져온 뼈는 몇개의 정점에게 영향을 주는가?
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			//!pAIBone->mWeight[j].mVertexId == 이 뼈가 영향을 주는 j번째 정점의 인덱스
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x) //! 해당 정점의 가중치값이 셋팅이 안됐다면
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i; 
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight; //! 가중치 채워주자
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

	/* pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다. */
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
