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
	m_iMaterialIndex = pAIMesh->mMaterialIndex; //! AIMesh가 들고있는 인덱스 받아오자

	strcpy_s(m_szName, pAIMesh->mName.data); //! mName 안의 데이터가 캐릭터 배열이다 이름가져오자

	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices; //! 정점의 개수는 읽어들인 개수다.

	m_iNumIndices = pAIMesh->mNumFaces * 3; //! mNumFaces가 삼각형 개수다. 즉, 읽어들인 삼각형 개수의 * 3
	m_iIndexStride = 4; //! 모델은 왠만해선 정점이 65535개를 넘어간다. 그러니까 그냥 4로 Default

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
	//TODO 추후 본행렬의 개수는 인자값으로 같이 받아오는게 좋을 것.
	_float4x4	BoneMatrices[256];
	
	//TODO 오프셋매트리스는 뭐야? 
	//! 메시기준으로 표현된 행렬이다. 뼈만의 상태를 표현한다기보다는 메시기준으로 뼈를 변환한다라고 생각해야한다.
	//! 만약, 게임 내의 NPC들이 있고 NPC들은 마을을 걸어다니거나, 유저가 말을 걸면 어떠한 애니메이션을 취해야 한다.
	//! 그 애니메이션 정보 하나를 공유하는 여러 NPC들이 있을 수 있다.
	//! 남자, 여자, 어린이, 노인 NPC들 모두 같은 애니메이션을 수행하는 경우가 있다. 
	//! 여기서 발생하는 문제는 모두 체형이 다르기 때문에 똑같은 애니메이션을 수행하기에는 부적합한 경우가 있는 것.
	//! 같은 뼈를 쓰고있지만 메쉬의 사이즈가 달라지기 때문이다. 이 뼈들이 메쉬에게 적용되기 위해서 해당 뼈의 상태를 해당뼈는 이런 행렬들을 더 곱해서 적용시켜야돼 라는 상태를 설명하는 행렬이다.
	//! 메쉬에게 적용되기 위한 뼈들은 반드시 오프셋 행렬을 곱해서 적용시켜줘야 해당 메쉬에게 정확히 적용됤될 수 있는 것이다.
	//! 오프셋 매트릭스는 바뀌지 않으므로 최초 로드시 한번만 배열로 만들어서 저장해둘 것이다. #오프셋매트릭스셋팅

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		//!오프셋 매트리를 곱해주자
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

_bool CMesh::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix)
{
	_matrix matWorld = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector vRayPos, vRayDir;

	//! 마우스 광선 위치(시작점), 방향
	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), matWorld);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), matWorld));

	_float fDist;

	_int	iMeshIndex = m_MeshIndices.size();

	for (_uint i = 0; i < iMeshIndex; ++i)
	{
		_vector vPickedPos;

		//삼각형  정점 인덱스 3개
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
		m_MeshVertexs.push_back(pVertices[i].vPosition); //! 픽킹 정점 위치 저장해두기.

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

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices]; //!#제로메모리조건
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		m_MeshVertexs.push_back(pVertices[i].vPosition);

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

	m_iNumBones = pAIMesh->mNumBones;

	//! 메시에게 영향을 주는 뼈를 순회하면서 각각의 뼈가 어떤 정점들에게 영향을 주는지 파악한다.
	for (size_t i = 0; i < pAIMesh->mNumBones; i++)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i]; //! 뼈하나를 가져왔다.

		_float4x4	OffsetMatrix; //#오프셋매트릭스셋팅
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		//! 마찬가지로 전치해주자
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint iBoneIndex = { 0 }; //! 영향을 주는 뼈의 인덱스를 저장해놓을거다.

		//TODO CBone 클래스는 AINode로 이루어져있고, 현재 클래스인 Mesh 클래스는 AIBone으로 이루어져있다.  AINode와 AIBone은 방식은 다르지만 뼈의 이름은 같다.
		//! 위 특성을 이용해서 모델클래스에서 Bone클래스를 저장하고있는 Bones 벡터를 가져와서 같은 이름을 가진 뼈를 찾아서 인덱스를 셋팅하자.
		
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

		//!위에서 찾아서 true를 만났다면 해당 index를 보관해주자
		m_BoneIndices.push_back(iBoneIndex);

		//! 위에가져온 뼈는 몇개의 정점에게 영향을 주는가?
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			//!pAIBone->mWeight[j].mVertexId == 이 뼈가 영향을 주는 j번째 정점의 인덱스
			//! #제로메모리조건
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

		//!위에서 찾아서 true를 만났다면 해당 index를 보관해주자
		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity()); //! 애니메이션 정보가 애초에 없기때문에 오프셋매트릭스도 존재하지않는다. 그냥 항등으로 넣어주자
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
