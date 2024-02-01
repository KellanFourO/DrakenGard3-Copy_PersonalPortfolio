#include "..\Public\VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_RandomNumber(rhs.m_RandomNumber)
	, m_pSpeeds(rhs.m_pSpeeds)
	, m_pLifeTimes(rhs.m_pLifeTimes)
	, m_InstancingDesc(rhs.m_InstancingDesc)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	m_InstancingDesc = *(INSTANCING_DESC*)pArg;

	

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance; //! �ν��Ͻ� ���� �� �� ũ��� �ν��Ͻ� ���� ������ * �ν��Ͻ� ����
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; //! ��������� ��� ���� ����Ǿ���ϱ⿡ �������۷� �������ش�.
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! ���ؽ� ���۴�
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //! CPU���� ���� ������ ����ؼ� �ν��Ͻ� ���۰� ���� ��������� ���� ������ �� �ֵ��� ����
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	_vector vDir = XMLoadFloat4(&m_InstancingDesc.vDir);

	if (true == m_InstancingDesc.bRandom)
	{
		//! ������ ���⺤��
		_float	fLength = { 0.0f };

		uniform_real_distribution<float>	RandomRange(0.1f, m_InstancingDesc.fRange);
		uniform_real_distribution<float>	RandomRotation(XMConvertToRadians(m_InstancingDesc.vRandomRotation.x), XMConvertToRadians(m_InstancingDesc.vRandomRotation.y));
		uniform_real_distribution<float>	RandomScale(m_InstancingDesc.vScale.x, m_InstancingDesc.vScale.y);
		uniform_real_distribution<float>	RandomSpeed(m_InstancingDesc.vSpeed.x, m_InstancingDesc.vSpeed.y); //! float2�� �̿��Ͽ� �������� ������ ũ��� �ӵ��� �ּ�, �ִ밪�� �޾Ƽ� �̾�����
		uniform_real_distribution<float>	RandomLifeTime(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);

		for (size_t i = 0; i < m_iNumInstance; i++) //! �ν��Ͻ� ������ŭ ������� ���������
		{
			m_pSpeeds[i] = RandomSpeed(m_RandomNumber);
			m_pLifeTimes[i] = RandomLifeTime(m_RandomNumber);

			_float	fScale = RandomScale(m_RandomNumber);

			pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);

			
			vDir = XMVector3Normalize(vDir) * RandomRange(m_RandomNumber);

			_vector		vRotation = XMQuaternionRotationRollPitchYaw(RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber), RandomRotation(m_RandomNumber));

			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

			XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMVector3TransformNormal(vDir, RotationMatrix));
			pVertices[i].vPosition.w = 1.f;
			pVertices[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);

		}
	}
	else
	{
		//! ������ ���⺤��
		_float	fLength = { 0.0f };

		for (size_t i = 0; i < m_iNumInstance; i++) //! �ν��Ͻ� ������ŭ ������� ���������
		{
			m_pSpeeds[i] = m_InstancingDesc.vSpeed.y;//RandomSpeed(m_RandomNumber);
			m_pLifeTimes[i] = m_InstancingDesc.vLifeTime.y;//RandomLifeTime(m_RandomNumber);

			_float	fScale = m_InstancingDesc.vScale.y; //RandomScale(m_RandomNumber);

			pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pVertices[i].vLook = _float4(0.f, 0.f, 1.0f, 0.f);

			vDir = XMVector3Normalize(vDir) * m_InstancingDesc.fRange;//RandomRange(m_RandomNumber);

			_vector		vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_InstancingDesc.vRotation.x), XMConvertToRadians(m_InstancingDesc.vRotation.y), XMConvertToRadians(m_InstancingDesc.vRotation.z));

			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

			//_vector vInterval = XMLoadFloat3(&m_InstancingDesc.vInterval);
			

			XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMLoadFloat3(&m_vInterval) + XMVector3TransformNormal(vDir, RotationMatrix));
			XMStoreFloat3(&m_vInterval, XMLoadFloat3(&m_vInterval) + XMLoadFloat3(&m_InstancingDesc.vInterval));
			

			pVertices[i].vPosition.w = 1.f;
			pVertices[i].vColor = m_InstancingDesc.vColor;//(1.f, 1.f, 1.f, 1.f);

		}
	}
	

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB ||
		nullptr == m_pVBInstance)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = { //! ���� ���ۿ� �ν��Ͻ� ����. �ΰ��� ���ؽ����۸� ���ε��ϴ°��� �� �� �ִ�.
		m_pVB,
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};


	/* � ���ؽ� ���۵��� �̿��ҰŴ�. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* � �ε��� ���۸� �̿��ҰŴ�. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* ������ ������� �̾ �׸��Ŵ�. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

void CVIBuffer_Instancing::Update(_float fTimeDelta)
{
	m_fAge += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float		fAlpha = max(m_pLifeTimes[i] - m_fAge, 0.f);

		pVertices[i].vColor.w = fAlpha;

		_vector		vDir = XMVector3Normalize(XMLoadFloat4(&pVertices[i].vPosition) - XMLoadFloat3(&m_InstancingDesc.vCenter));
		vDir = XMVectorSetW(vDir, 0.f);

		if (true == m_InstancingDesc.bTimeScale)
		{
			_float	fScale = m_InstancingDesc.vScale.y += 0.00005f; //RandomScale(m_RandomNumber);

			pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			
		}

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition) + vDir * m_pSpeeds[i] * fTimeDelta);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instancing::Render()
{
	//! ��ο��ε����ν��Ͻ� �Լ��� ����ϴ� ���� �� �� �ִ�.
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0); //! ���ڰ��� �ε������� ����� �Ѱ��� ĥ ������, �ν��Ͻ� ������ �����,  ���ؽ�, �ε���, �ν��Ͻ� ���� ���ۿ������� ������

	return S_OK;
}



void CVIBuffer_Instancing::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pLifeTimes);
	}


	Safe_Release(m_pVBInstance);
}
