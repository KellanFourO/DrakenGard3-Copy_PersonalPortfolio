#pragma once
#include "Component.h"


BEGIN(Engine)
class CAnimation;
class CTransform;


class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE	{ TYPE_NONANIM, TYPE_ANIM, TYPE_END }; //! 애니메이션이 있는 모델과 없는 모델을 구분짓기 위한 열거체
	enum class ANIM_STATE { NONE, LOOP, REVERSE, STATE_END };

	struct ModelData
	{
		wstring strBoneDataPath;
		wstring strMeshDataPath;
		wstring strMaterialDataPath;
		wstring strAnimationDataPath;
		wstring strHitAnimationDataPath;
	};

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class	CBone* Get_BonePtr(const _char* pBoneName) const;
	class	CBone* Get_BonePtr(const _int& iIndex);

	class	CAnimation* Get_CurrentAnimation(const _int& iIndex);
	class	CAnimation* Get_CurrentAnimation() { return  m_Animations[m_iCurrentAnimIndex]; }

	_uint	Get_NumMeshes() const { return m_iNumMeshes; }
	_uint	Get_CurrentAnimationIndex() const { return m_iCurrentAnimIndex; }

	
	_int	Get_IndexFromAnimName(const _char* In_szAnimName);
	_uint	Get_CurrentAnimationKeyIndex() const;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex); //! virtual은 의미가없다. 자기 자신이 가지고잇는 메시들을 렌더링 시키기위한 함수이다.

public:
	void	Play_Animation(_float fTimeDelta, _float3& vRootOutPos);
	void	Set_Loop(_bool bLoop) { m_isLoop = bLoop; }
	void	Set_Animation(_uint iAnimIndex, _uint iStartKeyIndex = 0, _float fBlendTime = 0.1f);
	void	Set_AnimationSpeed(_float fAnimationSpeed);
	_bool	Get_FinishedAnim() { return m_isFinished; }

	void	Root_MotionStart() { m_bRootMotionStart = true;}
	void	Root_MotionChange() { m_isRootAnim = true; }
	void	Root_MotionEnd() { m_bRootMotionStart = false; }
	void	Root_Motion(CTransform* pTransform);
	void	Reset_RootMotion();

public:
	//! 셰이더에 던질 뼈행렬은 특정매시에게 영향을 주는 뼈행렬을 던질 거라고했다. 모델에서 던지는것이아닌 매쉬 클래스를 통해 현재 클래스를 거쳐서 던져주는 행위를 하는 것이다.
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);

public:
	_bool	MouseOnModel();
	_bool	Compute_MousePos(RAY _Ray, _matrix _WorldMatrix);

private:
	//!const aiScene*			m_pAIScene = { nullptr };
	//!Assimp::Importer		m_Importer;
private:
	_float4x4					m_PivotMatrix; 
	TYPE						m_eModelType = { TYPE_END };

	vector<class CMesh*>		m_Meshes;
	_uint						m_iNumMeshes = { 0 };

	vector<MATERIAL_DESC>		m_Materials; 
	_uint						m_iNumMaterials = { 0 };

	vector<class CBone*>		m_Bones;


	vector<CAnimation*>			m_Animations;
	_uint						m_iNumAnimations = { 0 }; 
	_uint						m_iPrevAnimIndex = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 }; 
	_float						m_fStartBlendTime;
	_float						m_fCurrentBlendTime = { 0.f };
	_float						m_fMaxBlendTime;
	_bool						m_isBlend = { true };
	_bool						m_isLoop = { true };
	_bool						m_isFinished = { false };


	_float3						m_vPrevRootPosition = { 0.f, 0.f, 0.f };
	_float3						m_vCurrentRootPosition = { 0.f, 0.f, 0.f };

	_bool						m_bRootMotionStart = { false };
	_bool						m_isRootAnim = false;

	CBone*				m_pRootTranslateBone = { nullptr };

	ModelData					m_tDataFilePath;
public:
	typedef vector<CBone*>	BONES;

private:
	HRESULT Read_BoneData(const wstring& strPath);
	HRESULT Read_MeshData(const wstring& strPath, _fmatrix PivotMatrix);
	HRESULT Read_MaterialData(wstring& strPath);
	HRESULT Read_AnimationData(const wstring& strPath);

	_uint	Get_BoneIndex(const char* szName);

	string	ModifyPath(const string& originalPath);
	string	ReplaceExtension(const string& originalPath, const string& newExtension);
	string	ConvertWstrToStrModel(const wstring& wstr);
	wstring ConvertStrToWstrModel(const string& str);
	_float3 QuaternionToEuler(const _float4& quaternion);
	
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, ModelData& tDataFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END


