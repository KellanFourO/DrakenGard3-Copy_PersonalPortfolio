#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct  
	{
		_uint		iNumInstance;
		_float3		vCenter;
		_float		fRange;
		_float2		vSpeed;
		_float2		vScale;
		_float3		vRotation;
		_float4		vColor;
		_float2		vLifeTime;
		_float		fAge;
		_float4     vDir = { 1.f, 0.f, 0.f, 0.f };
		_float3		vInterval = { 1.f, 1.f, 1.f };

		_float2		vRandomRotation = {};
		
		_bool		bTimeScale = false;
		_bool		bRandom = true;
	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual	~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_VIBuffers() override; //! �ν��Ͻ��� ���ε��� �����ϱ����� override �Ѱ��� �� �� �ִ�
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr }; //! �ν��Ͻ��� ���� ���۷μ� �ν��Ͻ����۶�� �Ҹ���.
	_uint						m_iInstanceStride = { 0 }; //! �ν��Ͻ� ���� �ϳ��� ũ��� �÷� 16���� ������ ����� ũ���� �ߴ�. �ν��Ͻ� ����� ���� �������
	_uint						m_iNumInstance = { 0 } ; /* !�׸� ��ü 1���� 1���� ��������� �ʿ��ϴٰ� �ߴ�.�ν��Ͻ� ���� 1���� 1���� ��������� ������.*/ //TODO ��, �׸� ��ü ����
	_uint						m_iIndexCountPerInstance = { 0 }; /*!�׸� ��ü�� �ʿ��� �ε����� 6����� ������ ��� 6�̶�� �ϸ� �ȴ�.*/ //TODO �ε��� ������ ��ŭ �Ѱ��� ġ�ڴ�.

protected:
	random_device				m_RandomDevice; // ������ �̱�����
	mt19937_64					m_RandomNumber; 

protected:
	_float*						m_pSpeeds = { nullptr }; //! �� �ν��Ͻ��� ��ƼŬ���� ���������� �������� ������ ���ǵ���� ���� �迭
	_float*						m_pLifeTimes = { nullptr }; //! ���� ���������� ������Ÿ��
	INSTANCING_DESC				m_InstancingDesc;			//! ����ü �������
	_float						m_fAge = { 0.0f };		//! Ÿ�� ������ ���� ��ť�ַ��̼�
	_float3						m_vInterval = {};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END

