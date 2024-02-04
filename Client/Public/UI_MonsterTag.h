#pragma once
#include "Client_Defines.h"
#include "MyUI.h"



BEGIN(Client)

class CUI_MonsterTag final : public CMyUI
{
public:
	enum MONSTERTYPE { EN00, EN01, EN70, BOSS, TYPE_END};

	typedef struct tagMonsterHPDesc : public CMyUI::UI_DESC
	{
		CTransform*		pOwnerTransform;
		STATUS_DESC*	pOwnerStatus;
		MONSTERTYPE		eMonsterType = TYPE_END;
		_float			fCrntHPUV = 1.0f;
		_float			fPrevHPUV = 1.0f;
		_float			fScreenPosXOffset = 1.f;
	}MONSTER_TAG;

private:
	CUI_MonsterTag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);
	CUI_MonsterTag(const CUI_MonsterTag& rhs);
	virtual ~CUI_MonsterTag() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! ������ü�� �ʱ�ȭ�� ���� �Լ�.
	virtual HRESULT			Initialize(void* pArg) override; //! �纻��ü�� �ʱ�ȭ�� ���� �Լ�. ( void*�� �Ű����ڷ� �޾� Ư���� �纻��ü ó���� �����ϴ�. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;

private:
	void					Compute_OwnerCamDistance();
	_bool					In_Frustum();

private:
	LEVEL					m_eCurrentLevel = LEVEL_END;

private:
	MONSTER_TAG				m_tInfo = {};
	_float					m_fCurrentHp;
	_float					m_fPrevHp;
	/*_float					m_fHPBarTimeDuration = 100.f;*/

	const _float			m_fDefaultScale = 0.6f;
	_float					m_fScaleOffsetX = 1.f;

	
	_float					m_fScaleOffsetY = 1.f;

	_float					m_fPosXOffset = 0.f;
	_float					m_fPosYOffset = 0.f;
	_float					m_fOwnerCamDistance;

public:
	static CUI_MonsterTag*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel); //! ������ü ����
	virtual CGameObject*	Clone(void* pArg) override; //! �纻��ü ����
	virtual void			Free() override;
};

END


