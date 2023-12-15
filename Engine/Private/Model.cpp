#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	//! ���������� �� ���̴�. , m_Bones(rhs.m_Bones)
	//! �������� , m_Animations(rhs.m_Animations)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	//! �ִϸ��̼ǵ� �����ǹ����鼭 m_fTrackposition�� ���� �����ǹ����鼭 �ӵ��� ���� �������� ���̴�. ���� ���� ��������.
	for (auto& pProtoAnimation : rhs.m_Animations)
		m_Animations.push_back(pProtoAnimation->Clone());

	//! ���� ����������Ѽ� ������Ű�� ��� ���� �ִϸ��̼��� ���ư�������. 
	//! �÷��� �ִϸ��̼��� ���� ���븦 �ð��� �°� �����Ѵٰ��ߴ�.
	//! ���밡 �� �ڽſ� �°� ������ �Ǳ��ϴµ� �����ǹ����� �ᱹ �������� ���ư� �ִϸ��̼��� �������� ���õǹ����� ����.
	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& MaterialDesc : m_Materials) //!  ���͸����ũ���� ����ü�� ����ִ� ���� ��ȸ
	{
		for(auto& pTexture : MaterialDesc.pMtrlTextures) 
			Safe_AddRef(pTexture); //!  ����ü���� 18�� ���� �������ִ� �迭�� �ؽ�ó ���۷��� ī��Ʈ ī��Ʈ �÷�����
	}
	
	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix)
{
	//TODO m_Importer ��ü�� �������ִ� ReadFile �Լ��� ȣ���Ѵ�
	
	//! ReadFile �Լ��� ���� ������ ����ִ� AiScene*�� �������ش�.
	//! ���� ���� : aiProcess_PreTransformVertices | aiProcess_GlobalScale
	
	//! aiProcess_PreTransformVertices == ������ �̸� ��ȯ��Ű�ڴ�. �̰� ����ϸ� �ִϸ��̼� �������ư���. �ݵ�� �ִԸ��� �ƴѰ�쿡�� �������
	//! ������ ���� ���� ��ǥ���¿��� ���� �����϶����� ������ �޽ÿ��� ����� �����ִ� �Ը´�. �� �ɼ��� �ָ� �ִ������� ���ư��� ó���ϱⰡ ��ٷο�����.
	//! �ִԸ��� ��쿡�� �� �ɼ��� �����ʰ� �츮�� ���� �۾�������. ����ü�� �������ٰ��̴�.
	
	//! aiProcess_GlobalScale == ������ �ε��ߴ� ���� ���� ���¿��� �������� ���� Ű��ٺ��� ������ �̻�������. 
	//! �׷��� �����̳� �е��� ���ʿ� 100�� �����ϸ� �س��� ���·� �Ѱ��ش�. �̹� ū �������� �۰� ����°� �� �� ���� �����ΰ�����.
	//! GlobalScale�� �ְԵȴٸ� �ε��Ҷ����� 0.01 �����ϸ��� ���ִµ�, Ȥ�� 100�� �����ϸ��� �ȵ��ִ� ������ �𵨵� ������ �ֱ� ������ ��õ���� �ʴ´�.

	//! aiProcess_ConvertToLeftHanded == �޼���ǥ�踦 ����Ҳ�
	//! aiProcessPreset_TargetRealtime_Fast == ����Ƽ�� ���� ���������� ���� ���� ����� �����ؼ� �о���ϰ�
	
	m_eModelType = eType;
	m_tDataFilePath = tDataFilePath;

	//!  ���ִϸ��̼��� ���¸��̸� PreTransformVertices �� ������

	//#PivotMatrix
	//! �Ǻ���Ʈ������ �����̳ʵ��� ���� �츮�� ȯ�濡 �°� �Ϻ��ϰ� �����������ʴ´�. �׷��� 180�� ���ư��ִ� ���·� �ε��Ű�� ��� �ٸ��� ���� �ݴ븦 �ٶ󺸴� ���簡 �Ͼ��.
	//! �ε� ���Ĵ� �̹� ������ ������ ��ȯ�ȰͰ� ���������̴� �ε��Ҷ����� 180�� �������� �����ϱ� ���� �ִ� ���
	
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Read_BoneData(m_tDataFilePath.strBoneDataPath))) //! ���� ���� �翬�� ��Ʈ����ϰŰ�, �ֻ��� �θ��̱⿡ �θ��ε����� ������ -1�� ä������
		return E_FAIL;

	if(FAILED(Read_MeshData(m_tDataFilePath.strMeshDataPath, PivotMatrix)))
		return E_FAIL;

	//#������_Ready_Materials
	if(FAILED(Read_MaterialData(m_tDataFilePath.strMaterialDataPath)))
		return E_FAIL;

	if (FAILED(Read_AnimationData(m_tDataFilePath.strAnimationDataPath)))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	//TODO �޽ú��� �׸��ٶ�� ������ �ϰ��ִ�.
	//! SHIFT + ALT + G #���͸𵨷��� 
	//!���� ���Ͷ�� ���� �����Լ��� ���캸��
	//! ���̴� ��� �Լ��� ������ �Ŀ� �޽ø� ��ȸ�ϸ鼭 ������ ������ �־���.
	//!  ������ �������� �޽��� ���͸��� ����ü�� �����ؼ� �ؽ�ó�� ���������� �޾ƿͼ� �׷����ϰ�, �������� �޽��� ��ǻ�� �ؽ�ó�� �����ɼ��ִ�.
	//! ��, �޽��� ������ ���� ���͸��� ����ü�� ������ �� �۰ų� ������ �ִ�.
	//! �ݴ�� �ϳ��� ����ü�� �������� �޽��� �� ǥ���ϱ� ���� �� �� ������ �������� ����ü�� �������� �ֵ��� ����̴�.
	//! �� �޽� �����ϱ�? == �޽ú��� �����ϰ��ִµ�, Ư���޽��� �׸��� ���� ���̴��ؽ�ó�� ��ü�Ѵٶ�� �̾߱�� �����ϴϱ� �Ϲ����� ��.
	 
	 //TODO ���� ���� �������Ҷ� �޽��� ��ؽ�ó, ����͸��� ����ü�� �̿��ϴ°��� ���� ������ ��Ȯ�� �ʿ��ϴ�.
	 //! �׷��� ���͸��� �ε����� ��������� ����.

	 //! �޽ð� � ������ ��������� ����Ѵ�. �޽ø� ��ȸ�ϸ鼭 �ؽ�ó�� ������ ������ ���� Ŭ���̾�Ʈ�� ������ ������ �ٲ���.
	//for (auto& pMesh : m_Meshes)
	//{
	//	if (nullptr != pMesh)
	//	{
	//		pMesh->Bind_VIBuffers();
	//		pMesh->Render();
	//	}
	//}

	if(iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Bind_VIBuffers();
	m_Meshes[iMeshIndex]->Render();


	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
	if(m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	//! ���� �ִϸ��̼��� ����ϰ� �ִ� ������ TransformationMatrix�� �������ش�.
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);
	
	//! ȭ���� �������� ���·� �׷��������ؼ��� �ݵ��, ������ CombindTransformationMatrix�� ���ŵ� ���Ŀ��� �Ѵ�.
	//! ��� ������ �� �����ϸ� �θ�κ��� �ڽı��� ��ȸ�Ͽ� CombindTransformationMatrix�� ����������.
	
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
	}
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader,pConstantName, m_Bones);
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if(iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader,pConstantName); //! �츮�� ��ó�� 1���̴�. textureIndex�� ���� �� �ʿ����.
}

_bool CModel::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix)
{
	if (m_Meshes.empty()) // �޽��� ������� üũ
		return false;

	for (auto& iter : m_Meshes)
	{
		if (iter->Compute_MousePos(_Ray, _WorldMatrix))
			return true;
	}

	return false;
}


HRESULT CModel::Read_BoneData(const wstring& strPath)
{
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	while (true)
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

		if(dwByte == 0)
			break;
	}

	CloseHandle(hFile);

	return S_OK;
	
}

HRESULT CModel::Read_MeshData(const wstring& strPath, _fmatrix PivotMatrix)
{
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* ��� �޽� ��ȸ */
	size_t iNumMeshes;

	ReadFile(hFile, &iNumMeshes, sizeof(size_t), &dwByte, nullptr);

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

		m_eModelType = bAnim ? TYPE_ANIM : TYPE_NONANIM;

		vector<VTXMESH>			StaticVertices;
		vector<VTXANIMMESH>		AnimVertices;
		vector<_int>			Indiecs;
		_uint					iMaterialIndex;
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
			_int index;

			if (!ReadFile(hFile, &index, sizeof(_int), &dwByte, nullptr))
				return E_FAIL;

			Indiecs.push_back(index);
		}
			

		/* Material Index */
		if (!ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr))
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
			

		/* Create Mesh */
		CMesh* pMesh = nullptr;
		{
			pMesh = (bAnim) ? CMesh::Create(m_pDevice, m_pContext, m_eModelType, strName, AnimVertices, Indiecs, iMaterialIndex, BoneIndices, m_Bones) :
				CMesh::Create(m_pDevice, m_pContext, m_eModelType, strName, StaticVertices, Indiecs, iMaterialIndex, BoneIndices, PivotMatrix);

			if (nullptr == pMesh)
				return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}
	return S_OK;

// 	while (true)
// 	{
// 		// Read string length
// 		size_t strLength;
// 		if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
// 			return E_FAIL;
// 
// 		// Read string content
// 		string strName(strLength, '\0');
// 		if (!ReadFile(hFile, &strName[0], strLength, &dwByte, nullptr))
// 			return E_FAIL;
// 
// 		// Ensure null-termination
// 		strName.resize(strLength);
// 
// 		vector<VTXMESH>			NonAnimVertices;
// 		vector<VTXANIMMESH>		AnimVertices;
// 		vector<_int>			Indices;
// 		_uint					iMaterialIndex;
// 		vector<_int>			BoneIndices;
// 		_bool					isAnim;
// 		vector<asBone*>			Bones;
// 
// 		if (!ReadFile(hFile, &isAnim, sizeof(_bool), &dwByte, nullptr))
// 			return E_FAIL;
// 
// 		if (isAnim)
// 		{
// 			m_eModelType = CModel::TYPE_ANIM;
// 
// 			size_t vecSize;
// 			if(!ReadFile(hFile, &vecSize, sizeof(size_t), &dwByte, nullptr))
// 				return E_FAIL;
// 
// 			AnimVertices.reserve(vecSize);
// 
// 			for (size_t i = 0; i < vecSize; ++i)
// 			{
// 				VTXANIMMESH vertex;
// 
// 				if (!ReadFile(hFile, &vertex.vPosition, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vNormal, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vTexcoord, sizeof(_float2), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vTangent, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vBlendWeights, sizeof(_float4), &dwByte, nullptr))
// 					return E_FAIL;
// 
// 				AnimVertices.push_back(vertex);
// 			}
// 		}
// 		else
// 		{
// 			m_eModelType = CModel::TYPE_NONANIM;
// 
// 			size_t vecSize;
// 			if (!ReadFile(hFile, &vecSize, sizeof(size_t), &dwByte, nullptr))
// 				return E_FAIL;
// 
// 			NonAnimVertices.reserve(vecSize);
// 
// 			for (size_t i = 0; i < vecSize; ++i)
// 			{
// 				VTXMESH vertex;
// 
// 				if (!ReadFile(hFile, &vertex.vPosition, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vNormal, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vTexcoord, sizeof(_float2), &dwByte, nullptr))
// 					return E_FAIL;
// 				if (!ReadFile(hFile, &vertex.vTangent, sizeof(_float3), &dwByte, nullptr))
// 					return E_FAIL;
// 
// 				NonAnimVertices.push_back(vertex);
// 			}
// 		}
// 
// 		size_t vecIndiceSize;
// 		if (!ReadFile(hFile, &vecIndiceSize, sizeof(size_t), &dwByte, nullptr))
// 			return E_FAIL;
// 		
// 		Indices.reserve(vecIndiceSize);
// 
// 		for (size_t i = 0; i < vecIndiceSize; ++i)
// 		{
// 			_int index;
// 
// 			if (!ReadFile(hFile, &index, sizeof(_int), &dwByte, nullptr))
// 				return E_FAIL;
// 
// 			Indices.push_back(index);
// 		}
// 
// 		if (!ReadFile(hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr))
// 			return E_FAIL;
// 
// 		size_t vecBoneIndiceSize;
// 		if (!ReadFile(hFile, &vecBoneIndiceSize, sizeof(size_t), &dwByte, nullptr))
// 			return E_FAIL;
// 
// 		Indices.reserve(vecBoneIndiceSize);
// 
// 		for (size_t i = 0; i < vecBoneIndiceSize; ++i)
// 		{
// 			_int Boneindex;
// 
// 			if (!ReadFile(hFile, &Boneindex, sizeof(_int), &dwByte, nullptr))
// 				return E_FAIL;
// 
// 			BoneIndices.push_back(Boneindex);
// 		}
// 
// 		CMesh* pMesh = nullptr;
// 
// 		if (isAnim)
// 		{
// 			pMesh = CMesh::Create(m_pDevice,m_pContext,m_eModelType,strName, AnimVertices, Indices, iMaterialIndex, BoneIndices, m_Bones);
// 
// 			if(nullptr == pMesh)
// 				return E_FAIL;
// 		}
// 		else
// 		{
// 			pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, strName, NonAnimVertices, Indices, iMaterialIndex, BoneIndices, PivotMatrix);
// 
// 			if (nullptr == pMesh)
// 				return E_FAIL;
// 		}
// 
// 		m_Meshes.push_back(pMesh);
// 
// 		if (dwByte == 0)
// 			break;
// 	}
// 
// 	CloseHandle(hFile);
// 
// 	return S_OK;
	
}

HRESULT CModel::Read_MaterialData(const wstring& strPath)
{
	return S_OK;
}

HRESULT CModel::Read_AnimationData(const wstring& strPath)
{
	return S_OK;
}



CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, tDataFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for(auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}

	m_Bones.clear();

	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pMtrlTextures) //! �����迭�� Clear���� �ʿ� ����.
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}

	m_Meshes.clear();

	if(false == m_isCloned)
		m_Importer.FreeScene(); //! ������ü�϶��� �����Ϳ� ������ ȣ���ؼ� ����������

	
}

