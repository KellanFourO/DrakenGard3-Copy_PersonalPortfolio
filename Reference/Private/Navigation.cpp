#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
    : CComponent(rhs)
    , m_Cells(rhs.m_Cells)
#ifdef _DEBUG //!  내비게이션 메쉬 렌더링 부분은 디버그모드일때만 보여줄 것이기 때문에 셰이더 컴객체도 debug모드일때만 복사해주자.
    , m_pShader(rhs.m_pShader)
#endif
{
    for(auto& pCell : m_Cells)
        Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif

}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationFilePath)
{
    HANDLE hFile = CreateFile(strNavigationFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if(0 == hFile)
        return E_FAIL;

    _ulong dwByte = { 0 };

    while (true)
    {
        _float3 vPoints[3];

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        if(0 == dwByte)
            break;

        //! 여기서 Create 할 것. CCell*
        CCell*  pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        if(nullptr == pCell)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif


    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if(nullptr != pArg)
        m_iCurrentIndex = ((NAVI_DESC*)pArg)->iCurrentIndex;

    return S_OK;
}

HRESULT CNavigation::Render()
{
    for (auto& pCell : m_Cells)
    {
        if (nullptr != pCell)
            pCell->Render(m_pShader);
    }

    return S_OK;
}

_bool CNavigation::isMove(_fvector vPosition)
{
    _int        iNeighborIndex = { -1 };

    if(true == m_Cells[m_iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
        return true;

    else
    {
        if (-1 != iNeighborIndex)
        {
            while (true)
            {
                if(-1 == iNeighborIndex)
                    return false;

                if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
                {
                    m_iCurrentIndex = iNeighborIndex;
                    return true;
                }
            }
        }
        else
            return false;
    }
    
}

HRESULT CNavigation::Make_Neighbors()
{
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDescCell : m_Cells)
        {
            if(pSourCell == pDescCell)
                continue;

            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
            {
               pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDescCell);
            }
            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
            {
                pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDescCell);
            }
            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
            {
                pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDescCell);
            }
        }
    }
    
    return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationFilePath)
{
    CNavigation*    pInstance = new CNavigation(pDevice, pContext);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize_Prototype(strNavigationFilePath)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    /* 원형객체를 초기화한다.  */
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavigation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);

    m_Cells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif
}
