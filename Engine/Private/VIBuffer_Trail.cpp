#include "VIBuffer_Trail.h"
#include "Engine_Defines.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Transform.h"
#include "Bone.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
    : CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
    if (!pArg)
        return E_FAIL;

    m_tTrailDesc = *(TRAIL_DESC*)pArg;
    
    ZeroMemory(&m_iCatMullRomIndex, sizeof(_uint) * 4);

#pragma region VERTEXBUFFER

	/* 버텍스 버퍼 생성 */
    m_iStride           = sizeof(VTXPOSTEX);
    m_iNumVertices      = 2 * m_tTrailDesc.iMaxCount + 2;
    m_iNumVertexBuffers = 1;
    m_iVertexCount = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i += 2)
    {
        pVertices[i].vPosition     = m_tTrailDesc.vStartPos;
        pVertices[i + 1].vPosition = m_tTrailDesc.vEndPos;

        pVertices[i].vTexcoord        = _float2(1.f, 0.f);
        pVertices[i + 1].vTexcoord    = _float2(1.f, 1.f);
    }

	pVertices[0].vTexcoord = _float2(0.f, 0.f);
	pVertices[1].vTexcoord = _float2(0.f, 1.f);

	/* 인덱스 버퍼 생성 */
    m_iIndexStride = sizeof(FACEINDICES16);
    m_iNumPrimitive  = m_tTrailDesc.iMaxCount * 2;
    m_iNumIndices    = 3 * m_iNumPrimitive;
    m_eIndexFormat   = DXGI_FORMAT_R16_UINT;
    m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	 FACEINDICES16* pIndices = new FACEINDICES16[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumIndices);

    for (_uint i = 0; i < m_iNumPrimitive; i += 2)
    {
        pIndices[i]     = { _ushort(i), _ushort(i + 2), _ushort(i + 3) };
        pIndices[i + 1] = { _ushort(i), _ushort(i + 3), _ushort(i + 1) };
    }

	 /* ------------------------------------------------------------------ */

    ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
    m_BufferDesc.ByteWidth             = m_iStride * m_iNumVertices;
    m_BufferDesc.Usage                 = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.BindFlags             = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.StructureByteStride   = m_iStride;
    m_BufferDesc.CPUAccessFlags        = D3D11_CPU_ACCESS_WRITE;
    m_BufferDesc.MiscFlags             = 0;

    ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    /* ------------------------------------------------------------------ */

    ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
    m_BufferDesc.ByteWidth             = m_iIndexStride * m_iNumPrimitive;
    m_BufferDesc.Usage                 = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags             = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.StructureByteStride   = 0;
    m_BufferDesc.CPUAccessFlags        = 0;
    m_BufferDesc.MiscFlags             = 0;

    ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    m_SubResourceData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    /* ------------------------------------------------------------------ */

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(pIndices);

	return S_OK;
}


void CVIBuffer_Trail::Update(_float fTimeDelta, _fmatrix OwnerWorldMatrix)
{
    if (nullptr == m_pVB)
    {
        MSG_BOX("TrailBuffer Is Null");
        return;
    }

    _matrix ParentMatrix = XMMatrixIdentity();

    //!웨폰
    //ParentMatrix = pOwnerBoneCombine * pOwnerWorldMatrix;
     ParentMatrix.r[0] = XMVector3Normalize(OwnerWorldMatrix.r[0]);
     ParentMatrix.r[1] = XMVector3Normalize(OwnerWorldMatrix.r[1]);
     ParentMatrix.r[2] = XMVector3Normalize(OwnerWorldMatrix.r[2]);

     D3D11_MAPPED_SUBRESOURCE tSubResource;

     m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);

     if (m_iVertexCount >= m_iNumVertices)
     {
        _uint iRemoveCnt(m_iLerpPointNum * 2);
        m_iVertexCount -= iRemoveCnt;

        for (_uint i(0); i < m_iVertexCount; i += 2)
        {
            ((VTXPOSTEX*)tSubResource.pData)[i].vPosition = ((VTXPOSTEX*)tSubResource.pData)[iRemoveCnt + i].vPosition;
            ((VTXPOSTEX*)tSubResource.pData)[i + 1].vPosition = ((VTXPOSTEX*)tSubResource.pData)[iRemoveCnt + i + 1].vPosition;
        }
     }

      
     _vector vPos[2] =
     {
         XMVectorSetW(XMLoadFloat3(&m_tTrailDesc.vStartPos), 1.f),
         XMVectorSetW(XMLoadFloat3(&m_tTrailDesc.vEndPos), 1.f),
     };

     vPos[0] = XMVector3TransformCoord(vPos[0], ParentMatrix);
     vPos[1] = XMVector3TransformCoord(vPos[1], ParentMatrix);

     if (0 == m_iVertexCount)
     {
         for (_uint i(0); i < m_iNumVertices; i += 2)
         {
             XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i].vPosition, vPos[0]);
             XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i+ 1].vPosition, vPos[1]);
         }
     }
     else
     {
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iVertexCount].vPosition, vPos[0]);
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iVertexCount + 1].vPosition, vPos[1]);
     }

     // 12개가 포함된 개수라며 6개면, 6 * (12 + 2) = VtxCnt
     m_iVertexCount += 2;
 
 #pragma region CatMullRom
     _uint iEndIndex(m_iVertexCount + m_iLerpPointNum * 2);
     if (iEndIndex < m_iNumVertices)
     {
         m_iCatMullRomIndex[2] = iEndIndex - 2;//2개씩 빼는 이유 : 위 아래 2개 쌍 만큼 계산하기 때문에     
         m_iCatMullRomIndex[3] = iEndIndex;
 
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex - 2].vPosition, XMLoadFloat3((&((VTXPOSTEX*)tSubResource.pData)[m_iVertexCount - 2].vPosition)));
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex - 1].vPosition, XMLoadFloat3((&((VTXPOSTEX*)tSubResource.pData)[m_iVertexCount - 1].vPosition)));
 
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex].vPosition, vPos[0]);
         XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iEndIndex + 1].vPosition, vPos[1]);
 
         for (_uint i(0); i < m_iLerpPointNum; ++i)
         {
             _uint iIndex(i * 2 + m_iVertexCount - 2);
             _float fWeight(_float(i + 1) / (m_iLerpPointNum + 1));
 
             _vector vPos0 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[0]].vPosition);
             _vector vPos1 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[1]].vPosition);
             _vector vPos2 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[2]].vPosition);
             _vector vPos3 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[3]].vPosition);
 
             _vector LerpPoint = XMVectorCatmullRom(vPos0, vPos1, vPos2, vPos3, fWeight);
             XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iIndex].vPosition, LerpPoint);
 
             vPos0 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[0] + 1].vPosition);
             vPos1 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[1] + 1].vPosition);
             vPos2 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[2] + 1].vPosition);
             vPos3 = XMLoadFloat3(&((VTXPOSTEX*)tSubResource.pData)[m_iCatMullRomIndex[3] + 1].vPosition);
 
             LerpPoint = XMVectorCatmullRom(vPos0, vPos1, vPos2, vPos3, fWeight);
             XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[iIndex + 1].vPosition, LerpPoint);
 
         }
 
         m_iVertexCount = iEndIndex + 2;
 
         m_iCatMullRomIndex[0] = m_iCatMullRomIndex[1];
         m_iCatMullRomIndex[1] = m_iCatMullRomIndex[2];
     }
 #pragma endregion CatMullRom
     for (_uint i(0); i < m_iVertexCount; i += 2)
     {
         ((VTXPOSTEX*)tSubResource.pData)[i].vTexcoord = _float2(_float(i) / _float(m_iVertexCount - 2.f), 1.f);
         ((VTXPOSTEX*)tSubResource.pData)[i + 1].vTexcoord = _float2(_float(i) / _float(m_iVertexCount - 2.f), 0.f);
     }
     
     m_pContext->Unmap(m_pVB, 0);
 
}


void CVIBuffer_Trail::Tick(_float fTimeDelta)
{
    /*if (m_bTrailOn)
    {
        if (m_iVtxCnt < m_iNumVertices)
        {
            D3D11_MAPPED_SUBRESOURCE      SubResource;

            DEVICECONTEXT->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

            _float4 vHandPos = m_vLocalSwordLow;

            if (!m_pRefBone)
            {
                _float4x4 matWorld = m_pOwnerTransform.lock()->Get_WorldMatrix();
                vHandPos = vHandPos.MultiplyCoord(matWorld);
            }
            else
            {
                _matrix matRightHand = m_pRefBone->Get_CombinedMatrix();
                matRightHand = XMMatrixMultiply(matRightHand, m_TransformationMatrix.XMLoad());
                _float4x4 matBone;
                XMStoreFloat4x4(&matBone, matRightHand);
                _float4x4 matWorld = m_pOwnerTransform->Get_WorldMatrix();

                vHandPos = vHandPos.MultiplyCoord(matBone);
                vHandPos = vHandPos.MultiplyCoord(matWorld);
            }


            ((VTXTEX*)SubResource.pData)[m_iVtxCnt].vPosition.x = vHandPos.x;
            ((VTXTEX*)SubResource.pData)[m_iVtxCnt].vPosition.y = vHandPos.y;
            ((VTXTEX*)SubResource.pData)[m_iVtxCnt].vPosition.z = vHandPos.z;


            vHandPos = m_vLocalSwordHigh;
            if (!m_pRefBone)
            {
                _float4x4 matWorld = m_pOwnerTransform->Get_WorldMatrix();
                vHandPos = vHandPos.MultiplyCoord(matWorld);
            }
            else
            {
                _matrix matRightHand = m_pRefBone->Get_CombinedMatrix();
                matRightHand = XMMatrixMultiply(matRightHand, m_TransformationMatrix.XMLoad());
                _float4x4 matBone;
                XMStoreFloat4x4(&matBone, matRightHand);
                _float4x4 matWorld = m_pOwnerTransform->Get_WorldMatrix();

                vHandPos = vHandPos.MultiplyCoord(matBone);
                vHandPos = vHandPos.MultiplyCoord(matWorld);
            }

            ((VTXTEX*)SubResource.pData)[m_iVtxCount + 1].vPosition.x = vHandPos.x;
            ((VTXTEX*)SubResource.pData)[m_iVtxCount + 1].vPosition.y = vHandPos.y;
            ((VTXTEX*)SubResource.pData)[m_iVtxCount + 1].vPosition.z = vHandPos.z;

            m_iVtxCnt += 2;

            if (m_iVtxCnt > m_iNumVertices)
                m_iVtxCnt = m_iNumVertices;

            for (_uint i = 0; i < m_iVtxCnt; i += 2)
            {
                ((VTXTEX*)SubResource.pData)[i].vTexUV = { (_float)i / ((_float)m_iVtxCnt - 2), 1.f };
                ((VTXTEX*)SubResource.pData)[i + 1].vTexUV = { (_float)i / ((_float)m_iVtxCnt - 2), 0.f };
            }

            DEVICECONTEXT->Unmap(m_pVB.Get(), 0);

        }
        else
        {
            _uint   iRemoveCount = 2;

            if (iRemoveCount % 2 > 0)
            {
                iRemoveCount -= 1;
            }

            if (iRemoveCount < 2)
                iRemoveCount = 2;

            if (m_iVtxCnt >= 2)
                m_iVtxCnt -= iRemoveCount;


            D3D11_MAPPED_SUBRESOURCE      SubResource;
            DEVICECONTEXT->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

            if (m_iVtxCnt <= m_iNumVertices / 4)
            {
                for (_uint i = 2; i < m_iNumVertices; i += 2)
                {
                    ((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[0].vPosition;
                    ((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[1].vPosition;
                }
                m_iVtxCnt = 0;

            }

            for (_uint i = 0; i < m_iVtxCnt; i += 2)
            {
                ((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
                ((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
            }
            DEVICECONTEXT->Unmap(m_pVB.Get(), 0);
        }
    }
    else
    {
        _uint   iRemoveCount = 2;

        if (iRemoveCount % 2 > 0)
        {
            iRemoveCount -= 1;
        }

        if (iRemoveCount < 2)
            iRemoveCount = 2;

        if (m_iVtxCnt >= 2)
            m_iVtxCnt -= iRemoveCount;


        D3D11_MAPPED_SUBRESOURCE      SubResource;
        DEVICECONTEXT->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

        if (m_iVtxCnt <= m_iNumVertices / 4)
        {
            for (_uint i = 2; i < m_iNumVertices; i += 2)
            {
                ((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[0].vPosition;
                ((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[1].vPosition;
            }
            m_iVtxCnt = 0;

        }


        for (_uint i = 0; i < m_iVtxCnt; i += 2)
        {
            ((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
            ((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
        }
        DEVICECONTEXT->Unmap(m_pVB.Get(), 0);
    }*/
}

void CVIBuffer_Trail::Reset_Points(_fmatrix OwnerWorldMatrix)
{
    _matrix ParentMatrix = XMMatrixIdentity();

    //!웨폰
    //ParentMatrix = pOwnerBoneCombine * pOwnerWorldMatrix;
    ParentMatrix.r[0] = XMVector3Normalize(OwnerWorldMatrix.r[0]);
    ParentMatrix.r[1] = XMVector3Normalize(OwnerWorldMatrix.r[1]);
    ParentMatrix.r[2] = XMVector3Normalize(OwnerWorldMatrix.r[2]);
    
    D3D11_MAPPED_SUBRESOURCE		tSubResource;
     
    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tSubResource);
     
     
    _vector vPos[2] =
    {
        XMVectorSetW(XMLoadFloat3(&m_tTrailDesc.vStartPos), 1.f),
        XMVectorSetW(XMLoadFloat3(&m_tTrailDesc.vEndPos), 1.f),
    };
     
    vPos[0] = XMVector3TransformCoord(vPos[0], ParentMatrix);
    vPos[1] = XMVector3TransformCoord(vPos[1], ParentMatrix);
     
    for (_uint i = 0; i < m_iNumVertices; i += 2)
    {
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i].vPosition, vPos[0]);
        XMStoreFloat3(&((VTXPOSTEX*)tSubResource.pData)[i + 1].vPosition, vPos[1]);
    }
     
    m_pContext->Unmap(m_pVB, 0);
}



CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
    CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Trail");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Trail::Free()
{

}
