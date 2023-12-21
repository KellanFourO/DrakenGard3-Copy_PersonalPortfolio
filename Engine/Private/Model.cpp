#pragma once



#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"

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
	//! 깊은복제로 뺄 것이다. , m_Bones(rhs.m_Bones)
	//! 마찬가지 , m_Animations(rhs.m_Animations)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	//! 애니메이션도 공유되버리면서 m_fTrackposition의 값이 누적되버리면서 속도가 점점 빨라지는 것이다. 깊은 복제 빼버리자.
	for (auto& pProtoAnimation : rhs.m_Animations)
		m_Animations.push_back(pProtoAnimation->Clone());

	//! 뼈를 얕은복사시켜서 공유시키면 모두 같은 애니메이션이 돌아가버린다. 
	//! 플레이 애니메이션은 현재 뼈대를 시간에 맞게 셋팅한다고했다.
	//! 뼈대가 각 자신에 맞게 셋팅이 되긴하는데 공유되버려서 결국 마지막에 돌아간 애니메이션의 뼈정보로 셋팅되버리는 현상.
	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& MaterialDesc : m_Materials) //!  매터리얼디스크립션 구조체를 담고있는 벡터 순회
	{
		for(auto& pTexture : MaterialDesc.pMtrlTextures) 
			Safe_AddRef(pTexture); //!  구조체안의 18개 까지 가질수있는 배열의 텍스처 레퍼런스 카운트 카운트 올려주자
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


HRESULT CModel::Initialize_Prototype(TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix)
{
	//TODO m_Importer 객체가 가지고있는 ReadFile 함수를 호출한다
	
	//! ReadFile 함수는 모델의 정보를 담고있는 AiScene*을 리턴해준다.
	//! 추후 설명 : aiProcess_PreTransformVertices | aiProcess_GlobalScale
	
	//! aiProcess_PreTransformVertices == 정점을 미리 변환시키겠다. 이거 사용하면 애니메이션 정보날아간다. 반드시 애님모델이 아닌경우에만 사용하자
	//! 원래는 최초 로컬 좌표상태에서 뼈가 움직일때마다 스워드 메시에게 행렬을 곱해주는 게맞다. 위 옵션을 주면 애니정보가 날아가서 처리하기가 까다로워진다.
	//! 애님모델인 경우에는 위 옵션을 주지않고 우리가 직접 작업해주자. 열거체로 구분해줄것이다.
	
	//! aiProcess_GlobalScale == 기존에 로드했던 모델이 작은 상태에서 스케일을 많이 키우다보면 재질이 이상해진다. 
	//! 그래서 디자이너 분들이 애초에 100배 스케일링 해놓은 상태로 넘겨준다. 이미 큰 스케일을 작게 만드는게 좀 더 낫기 때문인가보다.
	//! GlobalScale을 주게된다면 로드할때부터 0.01 스케일링을 해주는데, 혹시 100배 스케일링이 안되있는 상태의 모델도 있을수 있기 때문에 추천하지 않는다.

	//! aiProcess_ConvertToLeftHanded == 왼손좌표계를 사용할께
	//! aiProcessPreset_TargetRealtime_Fast == 퀄리티는 조금 떨어지더라도 가장 빠른 방식을 선택해서 읽어들일게
	
	m_eModelType = eType;
	m_tDataFilePath = tDataFilePath;

	//!  엵애니메이션이 없는모델이면 PreTransformVertices 더 해주자

	//#PivotMatrix
	//! 피봇매트리스는 디자이너들이 보통 우리의 환경에 맞게 완벽하게 셋팅해주지않는다. 그래서 180도 돌아가있는 상태로 로드시키면 룩과 다르게 모델이 반대를 바라보는 참사가 일어난다.
	//! 로드 이후는 이미 정점이 끝까지 변환된것과 마찬가지이니 로드할때부터 180도 돌려놓고 시작하기 위해 주는 행렬
	
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Read_BoneData(m_tDataFilePath.strBoneDataPath))) //! 최초 노드는 당연히 루트노드일거고, 최상위 부모이기에 부모인덱스는 없으니 -1로 채워주자
		return E_FAIL;

	if(FAILED(Read_MeshData(m_tDataFilePath.strMeshDataPath, PivotMatrix)))
		return E_FAIL;

	//#모델재질_Ready_Materials
	if(FAILED(Read_MaterialData(m_tDataFilePath.strMaterialDataPath)))
		return E_FAIL;

	if(eType == CModel::TYPE_ANIM)
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
	//TODO 메시별로 그린다라는 행위를 하고있다.
	//! SHIFT + ALT + G #몬스터모델렌더 
	//!실제 몬스터라는 모델의 렌더함수를 살펴보면
	//! 셰이더 비긴 함수를 수행한 후에 메시를 순회하면서 렌더를 돌리고 있었다.
	//!  이제는 여러개의 메쉬가 매터리얼 구조체를 참조해서 텍스처로 재질정보를 받아와서 그려야하고, 여래개의 메쉬는 디퓨즈 텍스처등 공유될수있다.
	//! 즉, 메쉬의 개수에 비해 매터리얼 구조체의 개수는 더 작거나 같을수 있다.
	//! 반대로 하나의 구조체로 여러개의 메쉬를 다 표현하기 힘들 수 도 있으니 여러개의 구조체가 있을수도 있따는 얘기이다.
	//! 왜 메쉬 기준일까? == 메시별로 렌더하고있는데, 특정메쉬를 그리기 전에 셰이더텍스처를 교체한다라는 이야기는 가능하니까 일반적인 것.
	 
	 //TODO 이제 모델은 렌더링할때 메쉬가 어떤텍스처, 어떤메터리얼 구조체를 이용하는가에 대한 정보가 명확히 필요하다.
	 //! 그래서 매터리얼 인덱스를 멤버변수로 두자.

	 //! 메시가 어떤 재질을 들고있을지 줘야한다. 메시를 순회하면서 텍스처를 던지는 행위는 이제 클라이언트에 던지는 것으로 바꾸자.
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

	//! 이전 키프레임과 현재 키프레임의 인덱스가 달랐다면

	if (m_bChangeAnim) 
	{
		//! 현재 애니메이션에게 이전 애니메이션을 넘겨주자
		m_Animations[m_iCurrentAnimIndex]->Blend_TransformationMatrix(isLoop, fTimeDelta, m_Bones, m_pPrevAnimation, this);
	}
	else
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);
	
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

	return m_Materials[iMaterialIndex].pMtrlTextures[eTextureType]->Bind_ShaderResource(pShader,pConstantName); //! 우리는 어처피 1장이다. textureIndex는 따로 줄 필요없다.
}

_bool CModel::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix)
{
	if (m_Meshes.empty()) // 메쉬가 비었는지 체크
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

				path = ModifyPath(ConvertWstrToStr(strPath)) + strNewFileName;
				MaterialDesc.pMtrlTextures[aiTextureType(j)] = CTexture::Create(m_pDevice, m_pContext, ConvertStrToWstr(path));
			}
			else{ continue;}
		}
			
		m_Materials.push_back(MaterialDesc);
	}
	
	return S_OK;
}

HRESULT CModel::Read_AnimationData(const wstring& strPath)
{
	/* 모든 애니메이션 순회 */
	HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;

	/* 모든 메시 순회 */
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

		/* 모든 채널 순회 */
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


			/* 모든 채널 순회 */
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

string CModel::ConvertWstrToStr(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), &str[0], size_needed, nullptr, nullptr);
	return str;
}

wstring CModel::ConvertStrToWstr(const string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], size_needed);
	return wstr;
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

	/* 원형객체를 초기화한다.  */
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
		for (auto& pTexture : MaterialDesc.pMtrlTextures) //! 정적배열은 Clear해줄 필요 없다.
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}

	m_Meshes.clear();

	if(false == m_isCloned)
		m_Importer.FreeScene(); //! 원형객체일때만 임포터에 프리신 호출해서 정리해주자

	
}

