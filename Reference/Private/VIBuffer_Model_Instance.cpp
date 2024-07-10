#include "VIBuffer_Model_Instance.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Bone.h"

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Render()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint		iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(6, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Render_Mesh(_uint iMeshContainerIndex)
{
	if (iMeshContainerIndex >= m_iNumMeshes)
		return E_FAIL;


	/*if (sizeof(m_pVisibleInstanceDescs[m_iCurrentVisibleIndex]) / sizeof(INSTANCE_MESH_DESC))
		return E_FAIL;*/

	ID3D11Buffer* pVertexBuffers[] = {
		m_Meshes[iMeshContainerIndex]->Get_VertexBuffer(),
		m_pVBInstance
	};

	_uint		iStrides[] = {
		m_Meshes[iMeshContainerIndex]->Get_Stride(),
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_Meshes[iMeshContainerIndex]->Get_IndexBuffer(), m_Meshes[iMeshContainerIndex]->Get_IndexFormat(), 0);
	m_pContext->IASetPrimitiveTopology(m_Meshes[iMeshContainerIndex]->Get_Topology());

	m_pContext->DrawIndexedInstanced(m_Meshes[iMeshContainerIndex]->Get_NumIndices(), m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Model_Instance::Init_Model(wstring& In_ModelTag, _fmatrix PivotMatrix)
{
	//Prototype_Component_Model_Effect_Hanabira
	wstring strSliceTag = SliceModelTag(In_ModelTag);

	Reset_Model();
	m_tModelData = *m_pGameInstance->Get_ModelData_InKey(strSliceTag);
	
	m_strModelTag = In_ModelTag;
	m_PivotMatrix;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	Create_Materials();
	Create_Bones();
	Create_Meshs();

	
}

void CVIBuffer_Model_Instance::Init_Instance(_uint In_iNumInstance)
{
	m_iInstanceStride = sizeof(VTXMODELINSTANCE);
	m_iNumInstance = In_iNumInstance;
	m_iNumVertexBuffers = 2;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMODELINSTANCE* pModelInstance = new VTXMODELINSTANCE[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pModelInstance[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pModelInstance[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pModelInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pModelInstance[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pModelInstance;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance);

	Safe_Delete_Array(pModelInstance);
}

HRESULT CVIBuffer_Model_Instance::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

HRESULT CVIBuffer_Model_Instance::Bind_SRV(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eActorType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pMtrlTextures[eActorType])
		return m_Materials[iMaterialIndex].pMtrlTextures[eActorType]->Bind_ShaderResource(pShader, pConstantName); //! 우리는 어처피 1장이다. textureIndex는 따로 줄 필요없다.
}

void CVIBuffer_Model_Instance::Update(vector<INSTANCE_MESH_DESC>& In_ParticleDescs)
{
	if (0 == In_ParticleDescs.size() || 0 == m_iNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));



	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_matrix RotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&In_ParticleDescs[i].vRotation));

		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vRight, RotationMatrix.r[0] * In_ParticleDescs[i].vScale.x);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vUp, RotationMatrix.r[1] * In_ParticleDescs[i].vScale.y);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vLook, RotationMatrix.r[2] * In_ParticleDescs[i].vScale.z);
		XMStoreFloat4(&((VTXMODELINSTANCE*)SubResource.pData)[i].vTranslation, XMVectorSetW(XMLoadFloat3(&In_ParticleDescs[i].vPos), 1.f));
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	
}

HRESULT CVIBuffer_Model_Instance::Create_Materials()
{
	HANDLE hFile = CreateFile(m_tModelData.strMaterialDataPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* 모든 매태리얼 순회 */
	size_t iNumMaterials;
	ReadFile(hFile, &iNumMaterials, sizeof(size_t), &dwByte, nullptr);
	m_Materials.reserve(iNumMaterials);
	m_iNumMaterials = iNumMaterials;

	for (size_t i = 0; i < iNumMaterials; i++)
	{
		MATERIAL_DESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIAL_DESC));

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			string path;
			size_t strLength;
			if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
				return E_FAIL;

			string strPathName(strLength, '\0');
			if (!ReadFile(hFile, &strPathName[0], strLength, &dwByte, nullptr))
				return E_FAIL;

			if (strPathName != "")
			{
				strPathName.resize(strLength);
				string strNewExtension = "dds";
				string strNewFileName = ReplaceExtension(strPathName, strNewExtension);


				path = ModifyPath(ConvertWstrToStrInstance(m_tModelData.strMaterialDataPath)) + strNewFileName;
				MaterialDesc.pMtrlTextures[aiTextureType(j)] = CTexture::Create(m_pDevice, m_pContext, ConvertStrToWstrInstance(path));
			}
			else { continue; }
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}



HRESULT CVIBuffer_Model_Instance::Create_Meshs()
{
	//m_iNumMeshs = m_tModelData.strMeshDataPath.->iNumMeshs;
	//
	//for (_uint i = 0; i < m_iNumMeshContainers; ++i)
	//{
	//	weak_ptr<CMeshContainer> pMeshContainer = Get_Owner().lock()->Add_Component<CMeshContainer>();
	//	pMeshContainer.lock()->Init_Mesh(m_pModelData->Mesh_Datas[i]);
	//	m_MeshContainers.push_back(pMeshContainer);
	//}

	HANDLE hFile = CreateFile(m_tModelData.strMeshDataPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* 모든 메시 순회 */
	size_t iNumMeshes;

	ReadFile(hFile, &iNumMeshes, sizeof(size_t), &dwByte, nullptr);

	m_iNumMeshes = iNumMeshes;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		
		// Read string length
		size_t strLength;
		if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
			return E_FAIL;

		// Read string content
		string strName(strLength, '\0');
		if (!ReadFile(hFile, &strName[0], strLength, &dwByte, nullptr))
			return E_FAIL;


		_bool					bAnim;

		ReadFile(hFile, &bAnim, sizeof(_bool), &dwByte, nullptr);

		CModel::TYPE eModelType;
		eModelType = bAnim ? CModel::TYPE::TYPE_ANIM : CModel::TYPE::TYPE_NONANIM;

		//bAnim = false;

		vector<VTXMESH>			StaticVertices;
		vector<VTXANIMMESH>		AnimVertices;
		vector<FACEINDICES32>	Indiecs;
		_uint					iMaterialIndex;
		_int					iNumFace;
		vector<_int>			BoneIndices;

		/* Vertices */
		if (bAnim)
		{
			size_t iNumVertices;

			ReadFile(hFile, &iNumVertices, sizeof(size_t), &dwByte, nullptr);
			AnimVertices.reserve(iNumVertices);

			for (size_t j = 0; j < iNumVertices; j++)
			{
				VTXANIMMESH vertex;

				if (!ReadFile(hFile, &vertex.vPosition, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;
				if (!ReadFile(hFile, &vertex.vNormal, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;
				if (!ReadFile(hFile, &vertex.vTexcoord, sizeof(_float2), &dwByte, nullptr))
					return E_FAIL;
				if (!ReadFile(hFile, &vertex.vTangent, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;
				if (!ReadFile(hFile, &vertex.vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr))
					return E_FAIL;
				if (!ReadFile(hFile, &vertex.vBlendWeights, sizeof(_float4), &dwByte, nullptr))
					return E_FAIL;

				AnimVertices.push_back(vertex);
			}
		}
		else
		{
			size_t iNumVertices;
			ReadFile(hFile, &iNumVertices, sizeof(size_t), &dwByte, nullptr);

			StaticVertices.reserve(iNumVertices);

			for (size_t j = 0; j < iNumVertices; j++)
			{
				VTXMESH vertex;

				if (!ReadFile(hFile, &vertex.vPosition, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;

				if (!ReadFile(hFile, &vertex.vNormal, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;

				if (!ReadFile(hFile, &vertex.vTexcoord, sizeof(_float2), &dwByte, nullptr))
					return E_FAIL;

				if (!ReadFile(hFile, &vertex.vTangent, sizeof(_float3), &dwByte, nullptr))
					return E_FAIL;

				StaticVertices.push_back(vertex);
			}
		}

		/* Indices */
		size_t iNumIndices;
		ReadFile(hFile, &iNumIndices, sizeof(size_t), &dwByte, nullptr);
		Indiecs.reserve(iNumIndices);

		for (size_t j = 0; j < iNumIndices; j++)
		{
			FACEINDICES32 index;

			if (!ReadFile(hFile, &index, sizeof(FACEINDICES32), &dwByte, nullptr))
				return E_FAIL;

			Indiecs.push_back(index);
		}


		/* Material Index */
		if (!ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		if (!ReadFile(hFile, &iNumFace, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		/* Bone Indices*/
		size_t iNumBoneIndices;
		ReadFile(hFile, &iNumBoneIndices, sizeof(size_t), &dwByte, nullptr);
		BoneIndices.reserve(iNumBoneIndices);

		for (size_t j = 0; j < iNumBoneIndices; j++)
		{
			_int Boneindex;

			if (!ReadFile(hFile, &Boneindex, sizeof(_int), &dwByte, nullptr))
				return E_FAIL;

			BoneIndices.push_back(Boneindex);
		}

		vector<_float4x4> vecOffsetMatrix;
		/* Bone Indices*/
		size_t iOffsetMatrixSize;
		ReadFile(hFile, &iOffsetMatrixSize, sizeof(size_t), &dwByte, nullptr);
		vecOffsetMatrix.reserve(iOffsetMatrixSize);

		for (size_t j = 0; j < iOffsetMatrixSize; j++)
		{
			_float4x4 OffSetMatrix;

			if (!ReadFile(hFile, &OffSetMatrix, sizeof(_float4x4), &dwByte, nullptr))
				return E_FAIL;

			vecOffsetMatrix.push_back(OffSetMatrix);
		}



		CMesh* pMesh = nullptr;
		{
			pMesh = (bAnim) ? CMesh::Create(m_pDevice, m_pContext, eModelType, strName, iNumFace, AnimVertices, Indiecs, iMaterialIndex, BoneIndices, vecOffsetMatrix, m_Bones) :
				CMesh::Create(m_pDevice, m_pContext, eModelType, strName, iNumFace, StaticVertices, Indiecs, iMaterialIndex, BoneIndices, vecOffsetMatrix, XMLoadFloat4x4(&m_PivotMatrix));

			if (nullptr == pMesh)
				return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Create_Bones()
{
	HANDLE hFile = CreateFile(m_tModelData.strBoneDataPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	size_t BonesSize;
	ReadFile(hFile, &BonesSize, sizeof(size_t), &dwByte, nullptr);

	for (size_t i = 0; i < BonesSize; ++i)
	{
		_float4x4 matTransformation = {};
		_float4x4 matOffset = {};
		_int iBoneIndex = 0;
		_int iParentIndex = 0;
		_uint iDepth = 0;

		// Read string length
		size_t strLength;
		if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
			return E_FAIL;

		// Read string content
		string strName(strLength, '\0');
		if (!ReadFile(hFile, &strName[0], strLength, &dwByte, nullptr))
			return E_FAIL;

		// Ensure null-termination
		strName.resize(strLength);

		if (!ReadFile(hFile, &matTransformation, sizeof(_float4x4), &dwByte, nullptr))
			return E_FAIL;

		if (!ReadFile(hFile, &matOffset, sizeof(_float4x4), &dwByte, nullptr))
			return E_FAIL;

		if (!ReadFile(hFile, &iBoneIndex, sizeof(_int), &dwByte, nullptr))
			return E_FAIL;

		if (!ReadFile(hFile, &iParentIndex, sizeof(_int), &dwByte, nullptr))
			return E_FAIL;

		if (!ReadFile(hFile, &iDepth, sizeof(_uint), &dwByte, nullptr))
			return E_FAIL;

		CBone* pBone = CBone::Create(strName, matTransformation, matOffset, iBoneIndex, iParentIndex, iDepth);

		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);

	}

	CloseHandle(hFile);

	return S_OK;
	
}

void CVIBuffer_Model_Instance::Reset_Model()
{
	for (auto& iter : m_Meshes)
		Safe_Release(iter);


	
	m_iNumMeshes = 0;
	m_iNumMaterials = 0;
	m_Meshes.clear();
	m_Materials.clear();
}

wstring& CVIBuffer_Model_Instance::SliceModelTag(wstring& strModelTag)
{
	size_t pos = strModelTag.rfind(L'_');

	// 만약 '_'를 찾았다면
	if (pos != std::wstring::npos && pos < strModelTag.length() - 1) {
		// '_' 이후의 문자열을 리턴
		return strModelTag.substr(pos + 1);
	}
	else {
		// 찾을 수 없는 경우 빈 문자열 리턴
		return wstring();
	}
}

string CVIBuffer_Model_Instance::ModifyPath(const string& originalPath)
{
	filesystem::path pathObj = originalPath;

	filesystem::path directoryPath = pathObj.parent_path();

	filesystem::path modifiedPath = directoryPath;

	return modifiedPath.string() + "/";
}

string CVIBuffer_Model_Instance::ReplaceExtension(const string& originalPath, const string& newExtension)
{
	size_t dotPos = originalPath.find_last_of('.');

	if (dotPos != std::string::npos)
	{
		std::string modifiedPath = originalPath;
		modifiedPath.replace(dotPos + 1, std::string::npos, newExtension);
		return modifiedPath;
	}

	// If no dot is found, return the original path
	return originalPath;
}

string CVIBuffer_Model_Instance::ConvertWstrToStrInstance(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
	string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &str[0], size_needed, nullptr, nullptr);
	return str;
}

wstring CVIBuffer_Model_Instance::ConvertStrToWstrInstance(const string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
	return wstr;
}

CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Model_Instance::Clone(void* pArg)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Model_Instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Model_Instance::Free()
{
	__super::Free();
}
