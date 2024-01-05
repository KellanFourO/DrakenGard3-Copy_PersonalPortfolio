#pragma once



#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"
#include "Transform.h"

#include "GameInstance.h"
#include <regex>
#include <codecvt>

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


CBone* CModel::Get_BonePtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
		{
			if (!strcmp(pBone->Get_Name(), pBoneName))
				return true;
			return false;
		});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

CBone* CModel::Get_BonePtr(const _int& iIndex)
{
	/* �ε����� �ش��ϴ� Bone ã�� */

	if (m_Bones.size() < iIndex || iIndex < 0)
		return nullptr;

	return m_Bones[iIndex];
}


CAnimation* CModel::Get_CurrentAnimation(const _int& iIndex)
{
	if (iIndex >= m_Animations.size() ||
		iIndex < 0)
		return nullptr;

	return m_Animations[iIndex];
}

_uint CModel::Get_CurrentAnimationKeyIndex() const
{
	/* TrackPosition������ �ִϸ��̼��� �����Ҷ� ��� */
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentChannelKeyIndex();
}

_int CModel::Get_IndexFromAnimName(const _char* In_szAnimName)
{
	for (_uint i(0); i < m_iNumAnimations; ++i)
	{
		if (strcmp(m_Animations[i]->Get_Name(), In_szAnimName) == 0)
		{
			return i;
		}
	}

	return -1;
}

void CModel::Set_Animation(_uint iAnimIndex, _uint iStartKeyIndex, _float fBlendTime)
{
	if (iAnimIndex >= m_Animations.size())
		return;

	if (m_iPrevAnimIndex != -1)
	{
		m_Animations[iAnimIndex]->Reset_Animation();
	}

	/* ������̴� �ִϸ��̼� = Pre, ���� ����� �ִϸ��̼� = Curr*/
	m_iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iAnimIndex;

	/* StartKey�� Ʈ�������� ���� = �߰� �ִϸ��̼����� ����ϴ°� */
	m_Animations[m_iCurrentAnimIndex]->Set_StartAnimationKey(iStartKeyIndex);

	/* �ִϸ��̼� ����� TrackPosition �������� */
	m_fStartBlendTime = m_Animations[m_iCurrentAnimIndex]->Get_TrackPosition();

	/* �߰��ִϸ��̼����� �ִ´ٸ� �տ� ���̽ð� �־ �ʱ���۽ð� �ٸ��� ���� */
	m_fMaxBlendTime = fBlendTime + m_fStartBlendTime;
	m_fCurrentBlendTime = m_fStartBlendTime;

	m_isBlend = true;

	/* ����Ű������ ���� */
	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		m_Bones[i]->Setup_PreKeyFrame();
	}
}

void CModel::Set_AnimationSpeed(_float fAnimationSpeed)
{
	for (size_t i = 0; i < m_Animations.size(); i++)
	{
		m_Animations[i]->Set_AnimationSpeed(fAnimationSpeed);
	}
}

void CModel::Root_Motion(CTransform* pTransform)
{
	

	if (false == m_isBlend)
	{
		//TODO ��ġ�� ���ŵǳ� �ٽ� ���� ��ġ�� ���ư��� ����.
			//! ���� ��Ʈ ���� ��ġ�� ������
		_float3 vCurrentRootPosition;
		XMStoreFloat3(&vCurrentRootPosition, m_pRootTranslateBone->Get_CombinedTransformationMatrix().r[3]);

		//! ���� ��Ʈ���� ��ġ�� ���� ��Ʈ���� ��ġ�� �̿��ؼ� ���갪�� ������
		_float3 vDeltaPosition;
		XMStoreFloat3(&vDeltaPosition, (XMLoadFloat3(&vCurrentRootPosition) - XMLoadFloat3(&m_vPrevRootPosition)));

		//!������ ���� ��ġ�� ��������
		_float3 vCurrentWorldPosition;
		XMStoreFloat3(&vCurrentWorldPosition, pTransform->Get_State(CTransform::STATE_POSITION));


		_float3 vTargetLook;
		XMStoreFloat3(&vTargetLook, pTransform->Get_State(CTransform::STATE_LOOK));

		//! ������ ��ü�� �ٶ󺸰��ִ�	�������� �̵����� ���� ���ְ� �̵������ٰ� ���� �ٶ󺸰��ִ� ������ ��������
		_float3 vMoveDirection;
		XMStoreFloat3(&vMoveDirection, (XMLoadFloat3(&vDeltaPosition) * XMLoadFloat3(&vTargetLook)));

		//! ������ ���� ���� �̵������� ����������

		XMStoreFloat3(&vCurrentWorldPosition, (XMLoadFloat3(&vCurrentWorldPosition) + XMLoadFloat3(&vMoveDirection)));


		//! ���� ���� ��Ŀ� �̵����� ������ ������ġ�� �ٲ�����
		_float4x4 vCurrentWorldMatrix;

		XMStoreFloat4x4(&vCurrentWorldMatrix, pTransform->Get_WorldMatrix());


		vCurrentWorldMatrix._41 = vCurrentWorldPosition.x;
		vCurrentWorldMatrix._42 = vCurrentWorldPosition.y;
		vCurrentWorldMatrix._43 = vCurrentWorldPosition.z;

		pTransform->Set_WorldFloat4x4(vCurrentWorldMatrix);

		//! ���� ��ġ�� ���� ��ġ�� �ȴ�.
		m_vPrevRootPosition = vCurrentRootPosition;

// 		_float3 vZeroPosition = { 0.f, 0.f, 0.f };
// 		m_pRootTranslateBone->Set_Position(vZeroPosition);
	}
		
		
	//}
	//else 
	//	return;
}

void CModel::Reset_RootMotion()
{
	m_vPrevRootPosition = {};
	m_bRootMotionStart = false;
	m_isRootAnim = false;

	
	
}


HRESULT CModel::Initialize_Prototype(TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix)
{
	
	m_eModelType = eType;
	m_tDataFilePath = tDataFilePath;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Read_BoneData(m_tDataFilePath.strBoneDataPath))) //! ���� ���� �翬�� ��Ʈ����ϰŰ�, �ֻ��� �θ��̱⿡ �θ��ε����� ������ -1�� ä������
		return E_FAIL;

	if(FAILED(Read_MeshData(m_tDataFilePath.strMeshDataPath, PivotMatrix)))
		return E_FAIL;

	//#������_Ready_Materials
	if(FAILED(Read_MaterialData(m_tDataFilePath.strMaterialDataPath)))
		return E_FAIL;

	if(eType == CModel::TYPE_ANIM)
	if (FAILED(Read_AnimationData(m_tDataFilePath.strAnimationDataPath)))
		return E_FAIL;

	if (!m_tDataFilePath.strHitAnimationDataPath.empty()) //! HitAnimationPath�� ������ �ƴϾ��ٸ�
	{
		if (FAILED(Read_AnimationData(m_tDataFilePath.strHitAnimationDataPath)))
		{
			return E_FAIL;
		}
	}
	

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

void CModel::Play_Animation(_float fTimeDelta, _float3& vRootOutPos)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return;

	if (m_isBlend)
	{
		/* �ִϸ��̼� ������ ����� */
		if (m_fMaxBlendTime < DBL_EPSILON || m_fCurrentBlendTime >= m_fMaxBlendTime)
		{
			/* DBL_EPSILON : �ε� �Ҽ��� ���꿡�� ǥ�� ������ ���� ���� ���� �� (���� 0 �ٻ�) || �ִϸ��̼��� �ѹ� ������ */
			m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_isLoop, fTimeDelta, m_Bones);
			m_isBlend = false;
			m_Animations[m_iCurrentAnimIndex]->Set_TrackPosition(m_fCurrentBlendTime);
		}

		else
		{
			/* �������� ���� */
			_float fRatio = (m_fCurrentBlendTime - m_fStartBlendTime) / (m_fMaxBlendTime - m_fStartBlendTime);

			m_Animations[m_iCurrentAnimIndex]->Blend_TransformationMatrix(m_fMaxBlendTime, fRatio, m_Bones);

			m_fCurrentBlendTime += fTimeDelta;
		}
	}
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(m_isLoop, fTimeDelta, m_Bones);
	}


	_float3 vPos = {};

	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PivotMatrix), vPos);

	if (true == m_bRootMotionStart && false == m_isBlend && false == Get_CurrentAnimation()->Get_Finished())
	{
		_float3 vCalcPos;
		XMStoreFloat3(&vCalcPos, (XMLoadFloat3(&vPos) - XMLoadFloat3(&m_vPrevRootPosition)));
		

		vRootOutPos = vCalcPos;

	}
		m_vPrevRootPosition = vPos;
	

	
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


_bool CModel::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut)
{
	if (m_Meshes.empty()) // �޽��� ������� üũ
		return false;

	for (auto& iter : m_Meshes)
	{
		if (iter->Compute_MousePos(_Ray, _WorldMatrix, pOut))
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

HRESULT CModel::Read_MeshData(const wstring& strPath, _fmatrix PivotMatrix)
{
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* ��� �޽� ��ȸ */
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

		m_eModelType = bAnim ? TYPE_ANIM : TYPE_NONANIM;

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
			pMesh = (bAnim) ? CMesh::Create(m_pDevice, m_pContext, m_eModelType, strName, iNumFace, AnimVertices, Indiecs, iMaterialIndex, BoneIndices, vecOffsetMatrix, m_Bones) :
				CMesh::Create(m_pDevice, m_pContext, m_eModelType, strName, iNumFace, StaticVertices, Indiecs, iMaterialIndex, BoneIndices, vecOffsetMatrix, PivotMatrix);
		
			if (nullptr == pMesh)
				return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}
	return S_OK;

}

HRESULT CModel::Read_MaterialData(wstring& strPath)
{
	
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* ��� ���¸��� ��ȸ */
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


				path = ModifyPath(ConvertWstrToStrModel(strPath)) + strNewFileName;
				MaterialDesc.pMtrlTextures[aiTextureType(j)] = CTexture::Create(m_pDevice, m_pContext, ConvertStrToWstrModel(path));
			}
			else{ continue;}
		}
			
		m_Materials.push_back(MaterialDesc);
	}
	
	return S_OK;
}

 
HRESULT CModel::Read_AnimationData(const wstring& strPath)
{
	/* ��� �ִϸ��̼� ��ȸ */
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* ��� �޽� ��ȸ */
	size_t iNumAnims;

	ReadFile(hFile, &iNumAnims, sizeof(size_t), &dwByte, nullptr);

	m_iNumAnimations = iNumAnims;

	for (size_t i = 0; i < iNumAnims; i++)
	{
		// Read string length
		size_t strLength;
		if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
			return E_FAIL;

		// Read string content
		string strName(strLength, '\0');
		if (!ReadFile(hFile, &strName[0], strLength, &dwByte, nullptr))
			return E_FAIL;

		_float fDuration;

		ReadFile(hFile, &fDuration, sizeof(_float), &dwByte, nullptr);

		_float fTickPerSecond;

		ReadFile(hFile, &fTickPerSecond, sizeof(_float), &dwByte, nullptr);

		/* ��� ä�� ��ȸ */
		size_t iNumChannels;

		ReadFile(hFile, &iNumChannels, sizeof(size_t), &dwByte, nullptr);
		vector<CChannel*> Channels;
		Channels.reserve(iNumChannels);
		for (size_t j = 0; j < iNumChannels; j++)
		{
			// Read string length
			size_t strLength;
			if (!ReadFile(hFile, &strLength, sizeof(size_t), &dwByte, nullptr))
				return E_FAIL;

			// Read string content
			string strName(strLength, '\0');
			if (!ReadFile(hFile, &strName[0], strLength, &dwByte, nullptr))
				return E_FAIL;


			/* ��� ä�� ��ȸ */
			size_t iNumKeyframes;

			ReadFile(hFile, &iNumKeyframes, sizeof(size_t), &dwByte, nullptr);
			vector<KEYFRAME> Keyframes;
			Keyframes.reserve(iNumKeyframes);
			for (size_t k = 0; k < iNumKeyframes; k++)
			{
				KEYFRAME keyframe;

				ReadFile(hFile, &keyframe.fTrackPosition, sizeof(_float), &dwByte, nullptr);
				ReadFile(hFile, &keyframe.vScale, sizeof(_float3), &dwByte, nullptr);
				ReadFile(hFile, &keyframe.vRotation, sizeof(_float4), &dwByte, nullptr);
				ReadFile(hFile, &keyframe.vPosition, sizeof(_float3), &dwByte, nullptr);
				
				Keyframes.push_back(keyframe);
			}

			CChannel* pChannel = CChannel::Create(strName, Keyframes, Get_BoneIndex(strName.c_str()));
			if (nullptr == pChannel)
				return E_FAIL;

			Channels.push_back(pChannel);
		}

		CAnimation* pAnimation = CAnimation::Create(fDuration, fTickPerSecond, Channels, strName);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

_uint CModel::Get_BoneIndex(const char* szName)
{
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		if (!strcmp(m_Bones[i]->Get_Name(), szName))
			return i;
	}

	return 0;
}


string CModel::ModifyPath(const string& originalPath)
{
	filesystem::path pathObj = originalPath;

	filesystem::path directoryPath = pathObj.parent_path();
	
	filesystem::path modifiedPath = directoryPath;

	return modifiedPath.string() + "/";
}

string CModel::ReplaceExtension(const string& originalPath, const string& newExtension)
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

string CModel::ConvertWstrToStrModel(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
	string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &str[0], size_needed, nullptr, nullptr);
	return str;
}

wstring CModel::ConvertStrToWstrModel(const string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
	return wstr;
}

_float3 CModel::QuaternionToEuler(const _float4& quaternion)
{
	XMFLOAT3 euler;

	// Roll (X-axis rotation)
	float sinr_cosp = +2.0f * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
	float cosr_cosp = +1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (Y-axis rotation)
	float sinp = +2.0f * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(XM_PIDIV2, sinp); // use 90 degrees if out of range
	else
		euler.y = std::asin(sinp);

	// Yaw (Z-axis rotation)
	float siny_cosp = +2.0f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
	float cosy_cosp = +1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	return euler;
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

	//if(false == m_isCloned)
	//	m_Importer.FreeScene(); //! ������ü�϶��� �����Ϳ� ������ ȣ���ؼ� ����������

	
}

