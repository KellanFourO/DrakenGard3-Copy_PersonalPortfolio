#pragma once
#include "AnimObject.h"
#include "BrainTree/BrainTree.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

//TODO 브레인트리의 구성요소는 다음과 같다. #브레인트리구성요소

	//TODO BehaviorTree
	//!루트 노드를 뜻한다. 블랙보드를 가지고 있으며 모든 ai가 각각 가지고있을 것.

	//TODO Composite
	//! 해당 분기가 실행되는 기본 규칙을 정의하는 곳. 
	//! 대표적으로 Selector 와 Sequence 와 Parallel 세 종류가 있다. 


	//TODO Selector
	//! Selector Composite는 각 하위 노드를 순서대로 선택합니다.
	//! 자식이 성공하거나 실행되면 Selector는 동일한 상태를 반환합니다.
	//! 다음 틱에서는 ​​각 자식을 다시 순서대로 실행하려고 시도합니다.
	//! 모든 자식이 실패하는 경우에만 Selector가 실패합니다.

	//TODO Sequence
	//! Sequence Composite는 각 하위 노드를 순서대로 틱합니다.
	//! 자식이 실패하거나 실행되면 Sequence는 동일한 상태를 반환합니다.
	//! 다음 틱에서는 ​​각 자식을 다시 순서대로 실행하려고 시도합니다.
	//! 모든 자식이 성공하면 Sequence가 ​​성공합니다.

	// TODO Parallel
	//! 단순 병렬 노드이며, 전체 노드 트리와 동시에 하나의 태스크를 실행 할 수 있다. ex) 적을 향해 이동하면서 사격한다던지의 행동을 가능하게 해줌


	//TODO StatefulSelector (상태 저장 셀렉터)
	//! StatefulSelector Composite는 각 하위 노드를 순서대로 틱하고 이전에 틱하려고 시도한 하위 노드를 기억합니다.
	//! 자식이 성공하거나 실행되면 StatefulSelector는 동일한 상태를 반환합니다.
	//! 다음 틱에서는 ​​다음 자식을 실행하거나 처음부터 다시 시작하려고 시도합니다.
	//! 모든 자식이 실패하면 StatefulSelector가 실패합니다.

	//TODO StatefulSequence (상태 저장 시퀀스)
	//! StatefulSequence Composite는 각 하위 노드를 순서대로 틱하고 이전에 틱하려고 시도한 하위 노드를 기억합니다.
	//! 자식이 실패하거나 실행되면 StatefulSequence는 동일한 상태를 반환합니다.
	//! 다음 틱에서는 ​​다음 자식을 실행하거나 처음부터 다시 시작하려고 시도합니다.
	//! 모든 하위 항목이 성공하면 StatefulSequence만 성공합니다.


	//TODO Decorator
	//! 자식노드 하나를 가지며 조건을 검사하기위해 사용되는 노드, Composite 노드 위에 장착되는 경우도 있다.


	//TODO Succeeder
	//! Succeeder Decorator는 자식에게 무슨 일이 일어나든 관계없이 성공을 반환합니다.

	//TODO Failer
	//! Failer Decorator는 자식에게 무슨 일이 일어나든 관계없이 실패를 반환합니다.

	//TODO Inverter
	//! Inverter Decorator는 자식 노드의 상태를 반전시킵니다. 즉, 실패는 성공이 되고 성공은 실패가 됩니다.
	//! 자식이 실행되면 인버터도 실행 중이라는 상태를 반환합니다.

	//TODO Repeater
	//! Repeater Decorator는 자식이 성공을 반환할 때까지 무한히 또는 한도까지 반복합니다.

	//TODO UntilSuccess
	//! UntilSuccess Decorator는 자식이 성공을 반환할 때까지 반복한 다음 성공을 반환합니다.

	//TODO UntilFailure
	//! UntilFailure Decorator는 자식이 실패를 반환하고 성공을 반환할 때까지 반복됩니다.

BEGIN(Client)

class CMonster abstract : public CAnimObject
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT		Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Priority_Tick(_float fTimeDelta);
	virtual void		Tick(_float fTimeDelta);
	virtual void		Late_Tick(_float fTimeDelta);
	virtual HRESULT		Render();

public:
	virtual void		Write_Json(json& Out_Json);
	virtual void		Load_FromJson(const json& In_Json);
	virtual void		Init_Desc();

protected:
	virtual HRESULT						Ready_BehaviorTree();

protected:
	CShader*							m_pShaderCom	= { nullptr };
	CModel*								m_pModelCom		= { nullptr }; //#버퍼컴에서_모델컴으로_변경됨
	CCollider*							m_pColliderCom	= { nullptr };
	shared_ptr<BrainTree::BehaviorTree> m_BehaviorTree	= { nullptr };

private:
	//STATE_LINK_MONSTER_DESC      m_tLinkStateDesc;

public:
	virtual void Free() override;

};

END

