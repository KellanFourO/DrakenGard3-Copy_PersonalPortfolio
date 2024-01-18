#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CBone;
class CModel;
class CShader;
class CNavigation;

class ENGINE_DLL CPartObject abstract : public CGameObject
{

public:
	typedef struct tagPartDesc
	{
		class CTransform*	m_pParentTransform	= { nullptr };
		class CNavigation*	m_pParentNavigation = { nullptr };
		class CBone*		m_pSocketBone		= { nullptr };
		
	}PART_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;


public:
	class CModel* Get_ModelCom() { return m_pModelCom; }
	void		 SetUp_Animation(_uint iAnimIndex);
	class CBone* Get_BonePtr(const _char* pBoneName);
	

protected:
	HRESULT		Ready_Components(_uint iLevelIndex, const wstring& strShaderTag, const wstring& strModelTag);
	HRESULT		Bind_ShaderResources();

protected:
	string				m_strName = "";
	_float4x4			m_WorldMatrix = {};
	_uint				m_eCurrentLevelIndex = 9999;

protected:
	class CNavigation*	m_pParentNavigationCom = { nullptr };
	class CTransform*	m_pParentTransformCom	= { nullptr };
	class CModel*		m_pModelCom				= { nullptr };
	class CShader*		m_pShaderCom			= { nullptr };
	class CBone*		m_pSocketBone			= { nullptr };

public:
	virtual void Free() override;
};

END

