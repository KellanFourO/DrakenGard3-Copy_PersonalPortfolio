#pragma once
#include "Component.h"

BEGIN(Engine)

class CCell;
//TODO �׺�Ž�
//! �� �޽��ΰ�?   =  �ﰢ������ �����̴ϱ� �޽���� �θ���
//! �̱��� �Ǵ� ������Ʈ�� ����µ�, �̱������� �̿��K�ҽ� ���߿� ��Ʈ���ϱⰡ ���ŷο�����. ������Ʈ�� �ؼ� �׺�Ž��� �̿��� ��ü���� ���� ������ �ִ� ���·� ��Ʈ���ϴ°� ����.
//! ����� ������ΰ�? �׺�޽��� �ﰢ������ ���� ����� ó���ؼ� ���� �ﰢ���� �����Ѵ�.
//! �ﰢ���� Cell Ŭ������ �߰��ؼ� ���� Ŭ������ �������ִ� ���·� ������ ���̴�.
//! Ŭ������ ���� ���� ������ Cell Ŭ�������� ������ ��.

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int	iCurrentIndex;
	}NAVI_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;


public:
	void			SaveData(wstring strSavePath);

public:
	virtual HRESULT Initialize_Prototype(const wstring& strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render();
#endif
	
public:
	vector<class CCell*>	Get_Cells() { return m_Cells; }
	_int	Get_CellSize() { return m_Cells.size(); };
	void	Update(_fmatrix WorldMatrix);
	_bool	isMove(_fvector vPosition);
	void	AddCell(class CCell* pCell);
	HRESULT	Delete_Cell(const _uint iIndex);


public:
	void	InRangeCellChange(class CCell* pCell, _int ePoint, _float3 vChangePos);
	

	_float Compute_Height(_float3 vPosition);

private:
	vector<class CCell*>	m_Cells;
	_int					m_iCurrentIndex = { -1 };
	
	static _float4x4		m_WorldMatrix;

	

#ifdef _DEBUG
private:
	class CShader*			m_pShader = { nullptr };
#endif

private:
	HRESULT		Make_Neighbors(); //! �̿� ���

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavgationFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

