#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Dynamic_Terrain;
END

BEGIN(Client)

/* Dynamic Terrain*/


class CDynamic_Terrain final : public CGameObject
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END }; // ����, ����ũ
	
	enum EDIT_MODE
	{
		HEIGHT_FLAT,
		HEIGHT_LERP,
		HEIGHT_SET,
		FILLTER,
		NON,
		EDIT_END
	};

	typedef struct tagVertex_Dynamic_Info
	{
		float		fX;
		float		fY;
		float		fZ;

		vector<VTXDYNAMIC> vecVertexInfo;
	}DINFO;
private:
	CDynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_Terrain(const CDynamic_Terrain& rhs);
	virtual ~CDynamic_Terrain() = default;

public:
	void	Delete_Component(const wstring& strComTag);
	_vector Get_WorldMatrix() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel); //! ������ü�� �ʱ�ȭ�� ���� �Լ�.
	virtual HRESULT Initialize(void* pArg) override; //! �纻��ü�� �ʱ�ȭ�� ���� �Լ�. ( void*�� �Ű����ڷ� �޾� Ư���� �纻��ü ó���� �����ϴ�. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	virtual void Write_Json(json& Out_Json, const wstring& strFileName);
	virtual void Load_FromJson(const json& In_Json, const wstring& strFileName);
	

public:
	void	CreateHeightMap();
	void	Picking_Terrain(EDIT_MODE eMode);
	_float3	GetMousePos();

	_bool	MouseOnTerrain();

	void	SetPower(_float fPower) { m_fPower = fPower; }
	void	SetRadious(_float fRadious) { m_fDrawRadious = fRadious; }

private:
	CShader*				   m_pShaderCom = { nullptr };
	CTexture*				   m_pTextureCom[TYPE_END] = { nullptr }; // ������ ����ũ�� �Ѱ��� ������� �迭�� ���� 0: ���� 2��, 1: ����ũ 1��
	CVIBuffer_Dynamic_Terrain* m_pVIBufferCom = { nullptr };

private:
	_float						m_fDrawRadious = { 1.f };
	_float						m_fPower = { 2.f };
	_float3						m_fPickingPos = { 0.f, 0.f, 0.f };

private:
	EDIT_MODE					m_eEditMode = { EDIT_MODE::NON };
	LEVEL						m_eCurrentLevelID = { LEVEL_END };
	DINFO						m_tDynamicInfo;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void	ReceiveInfo(DINFO pInfo);

public:
	static CDynamic_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel); //! ������ü ����
	virtual CGameObject* Clone(void* pArg) override; //! �纻��ü ����
	virtual void			Free() override;
};

END

