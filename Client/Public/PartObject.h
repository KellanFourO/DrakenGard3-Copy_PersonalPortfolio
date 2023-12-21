#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CBone;
class CModel;
class CShader;
END

BEGIN(Client)

class CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		class CTransform*	m_pParentTransform	= { nullptr };
		class CBone*		m_pSocketBone		= { nullptr };
	}PART_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;


public:
	void		 SetUp_Animation(_uint iAnimIndex);
	class CBone* Get_BonePtr(const _char* pBoneName);

protected:
	HRESULT		Ready_Components(LEVEL eLevel, const wstring& strShaderTag, const wstring& strModelTag);
	HRESULT		Bind_ShaderResources();

protected:
	string				m_strName = "";
	_float4x4			m_WorldMatrix = {};
	LEVEL				m_eCurrentLevelID		= { LEVEL_END };

protected:
	class CTransform*	m_pParentTransformCom	= { nullptr };
	class CModel*		m_pModelCom				= { nullptr };
	class CShader*		m_pShaderCom			= { nullptr };
	class CBone*		m_pSocketBone			= { nullptr };

public:
	virtual void Free() override;
};

END

