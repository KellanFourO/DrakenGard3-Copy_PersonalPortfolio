#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Tool)

class CT_BackGround final : public CGameObject
{
private:
	CT_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CT_BackGround(const CT_BackGround& rhs);
	virtual ~CT_BackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CT_BackGround*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! 원형객체 생성
	virtual CGameObject*	Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

END

