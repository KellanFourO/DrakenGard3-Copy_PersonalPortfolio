#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct  
	{
	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual	~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_VIBuffers() override; //! 인스턴싱의 바인딩은 따로하기위해 override 한것을 볼 수 있다
	virtual HRESULT Render();

protected:
	ID3D11Buffer*	m_pVBInstance = { nullptr }; //! 인스턴싱을 위한 버퍼로서 인스턴스버퍼라고 불린다.
	_uint			m_iInstanceStride = { 0 }; //! 인스턴스 버퍼 하나의 크기는 플롯 16개를 가지는 행렬의 크기라고 했다. 인스턴스 사이즈를 따로 잡아주자
	_uint			m_iNumInstance = { 0 } ; /* !그릴 객체 1개당 1개의 상태행렬이 필요하다고 했다.인스턴스 버퍼 1개당 1개의 상태행렬을 가진다.*/ //TODO 즉, 그릴 객체 개수
	_uint			m_iIndexCountPerInstance = { 0 }; /*!그릴 객체의 필요한 인덱스가 6개라고 가정할 경우 6이라고 하면 된다.*/ //TODO 인덱스 사이즈 만큼 한개로 치겠다.

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

