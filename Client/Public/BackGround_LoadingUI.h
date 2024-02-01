#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround_LoadingUI final : public CGameObject
{
public:
	typedef struct tagBackGroundDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fSizeX, fSizeY;
	}BACKGROUND_DESC;

private:
	CBackGround_LoadingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround_LoadingUI(const CBackGround_LoadingUI& rhs);
	virtual ~CBackGround_LoadingUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override; //! ������ü�� �ʱ�ȭ�� ���� �Լ�.
	virtual HRESULT Initialize(void* pArg) override; //! �纻��ü�� �ʱ�ȭ�� ���� �Լ�. ( void*�� �Ű����ڷ� �޾� Ư���� �纻��ü ó���� �����ϴ�. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		m_ViewMatrix, m_ProjMatrix;

private:
	_float			m_fFrame = { 0.0f };

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

	CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBackGround_LoadingUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! ������ü ����
	virtual CGameObject*	Clone(void* pArg) override; //! �纻��ü ����
	virtual void			Free() override;
};

END

