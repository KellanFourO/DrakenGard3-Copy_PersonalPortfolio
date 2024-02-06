#pragma once
#include "Client_Defines.h"
#include "MyUI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_Blood final : public CMyUI
{

private:
	CUI_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);
	CUI_Blood(const CUI_Blood& rhs);
	virtual ~CUI_Blood() = default;

public:
	virtual HRESULT			Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT			Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	virtual HRESULT			Ready_Components() override;
	virtual HRESULT			Bind_ShaderResources() override;


private:
	void					Dead_Action(_float fTimeDelta, _float fLifeTime);

private:
	LEVEL					m_eCurrentLevel = LEVEL_END;
	CTexture*				m_pMaskTexture = { nullptr };
	CTexture*				m_pNoiseTexture = { nullptr };
	CTexture*				m_pDissoveTexture = { nullptr };
	
	_int					m_iRandomNumber = {};

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

	_float						m_fOriginY = {};
	_float						m_fMaxDown = { 150.f };
	_float						m_fDownSpeed = { 5.f };


	_float						m_fAge = 0.f;
	_int						m_iPassIndex = 12;
	_float						m_fDissoveWeight = 0.f;

public:
	static CUI_Blood*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel); //! 원형객체 생성
	virtual CGameObject*	Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

END


