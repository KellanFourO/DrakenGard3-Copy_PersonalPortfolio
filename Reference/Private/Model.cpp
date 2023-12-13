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

HRESULT CModel::Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix)
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

	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	//!  엵애니메이션이 없는모델이면 PreTransformVertices 더 해주자
	if(TYPE_NONANIM == eType)
		iFlag |= aiProcess_PreTransformVertices;
	
	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);

	if(nullptr == m_pAIScene)
		return E_FAIL;

	//#PivotMatrix
	//! 피봇매트리스는 디자이너들이 보통 우리의 환경에 맞게 완벽하게 셋팅해주지않는다. 그래서 180도 돌아가있는 상태로 로드시키면 룩과 다르게 모델이 반대를 바라보는 참사가 일어난다.
	//! 로드 이후는 이미 정점이 끝까지 변환된것과 마찬가지이니 로드할때부터 180도 돌려놓고 시작하기 위해 주는 행렬
	
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1))) //! 최초 노드는 당연히 루트노드일거고, 최상위 부모이기에 부모인덱스는 없으니 -1로 채워주자
		return E_FAIL;

	if(FAILED(Ready_Meshes(PivotMatrix)))
		return E_FAIL;

	//#모델재질_Ready_Materials
	if(FAILED(Ready_Materials(strModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
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

	//! 현재 애니메이션이 사용하고 있는 뼈들의 TransformationMatrix를 갱신해준다.
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, fTimeDelta, m_Bones);
	
	//! 화면의 최종적인 상태로 그려내기위해서는 반드시, 뼈들의 CombindTransformationMatrix가 갱신된 이후여야 한다.
	//! 모든 뼈들을 다 갱신하며 부모로부터 자식까지 순회하여 CombindTransformationMatrix를 갱신해주자.
	
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

HRESULT CModel::Ready_Meshes(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes; //! 읽어들인 메쉬개수로 멤버변수를 채워주자

	m_Meshes.reserve(m_iNumMeshes); //! 메쉬 개수를 알게됐으니 벡터를 예약해주자

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*	pMesh = CMesh::Create(m_pDevice,m_pContext, m_eModelType, m_pAIScene->mMeshes[i], PivotMatrix, m_Bones);

		if(nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Materials(const string& strModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials; //! AIScene으로 부터 읽어온 매터리얼의 개수를 받자.

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MATERIAL_DESC	MaterialDesc = {}; //! 제로 메모리

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			//! 한 메시안에 디퓨즈 매터리얼이 여러개 있을수도있다. 원래는 3중루프를 돌려야하나 보통은 1개만 사용한다. 만약 뭔가 이상하다면 pAIMaterial->GetTextureCount 함수를 호출해서 확인해보자
			/* 
			for (size_t k = 0; k < pAIMaterial->GetTextureCount(aiTextureType(j)); k++)
			{
				pAIMaterial->GetTexture(aiTextureType(j), k, );
			}; */

			//TODO 어심프가 읽은 텍스처의 경로는 애초에 추출할때 사용했던 텍스처의 경로로 인식한다. 파일명과 확장자만 떼서 fbx 모델파일이 있는 경로에 붙여주고 fbx 모델이있는곳에 관련 이미지 파일들을 넣어주자
			_char	szDrive[MAX_PATH ] = "";
			_char	szDirectory[MAX_PATH] = "";

			_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			aiString		strPath;

			//! GetTexture 함수의 리턴값을 보면 aiReturn인데 실패하면 0 아니면 0이 아닌값을 리턴해서 HRESULT 형식과 같아서 체크 가능하다.
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath))) //!  어심프가 읽을때 받아온 텍스처의 로드의 경로를 strPath 변수로 받자
				continue;

			_char	szFileName[MAX_PATH] = "";
			_char	szEXT[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char szTemp[MAX_PATH] = "";
			strcpy_s(szTemp, szDrive); //! 아까 가져온 드라이브 경로받자.
			strcat_s(szTemp, szDirectory); //! 폴더 경로 붙여주자
			strcat_s(szTemp, szFileName); //! 파일명 붙여주자
			strcat_s(szTemp, szEXT); //! 파일명 붙여주자

			//_char szTest[MAX_PATH] = ".dds";
			//
			//strcat_s(szTemp, szTest); //! 확장자 붙여주자

			_tchar szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTemp, strlen(szTemp), szFullPath, MAX_PATH); //! Texture클래스는 읹인자값으로 wstring을 받고있따다. 바꿔주자

			MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice,m_pContext,szFullPath, 1);
			if(nullptr == MaterialDesc.pMtrlTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAInode, _int iParentIndex)
{
	CBone*	pBone = CBone::Create(pAInode, iParentIndex);

	if(nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);


	//! 루프문 밖에서 부모인덱스를 미리 셋팅해놔야 참사를 막을수있다. dfs 탐색을 사용하고 우리의 부모인덱스는 옆노드가 아닌 상위노드의인덱스이기 때문에 루프문 바깥에서 해줘야한다.
	_int	iParentIdx = m_Bones.size() - 1;

	for (size_t i = 0; i < pAInode->mNumChildren; i++)
	{
		Ready_Bones(pAInode->mChildren[i], iParentIdx);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;
	
		m_Animations.push_back(pAnimation);
	}
	
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, PivotMatrix)))
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
