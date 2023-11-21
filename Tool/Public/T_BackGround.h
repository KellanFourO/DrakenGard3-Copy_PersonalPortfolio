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
	virtual HRESULT Initialize_Prototype() override; //! ������ü�� �ʱ�ȭ�� ���� �Լ�.
	virtual HRESULT Initialize(void* pArg) override; //! �纻��ü�� �ʱ�ȭ�� ���� �Լ�. ( void*�� �Ű����ڷ� �޾� Ư���� �纻��ü ó���� �����ϴ�. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CT_BackGround*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! ������ü ����
	virtual CGameObject*	Clone(void* pArg) override; //! �纻��ü ����
	virtual void			Free() override;
};

END

