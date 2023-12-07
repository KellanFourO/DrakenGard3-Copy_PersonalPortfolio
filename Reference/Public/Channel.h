#pragma once
#include "Base.h"

//TODO 애니메이션에서 사용하는 뼈의 정보
//! 시간대에 따른 뼈의 상태(Scale, Rotation, Position)을 저장할거야.

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pChannel);
	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition);

private:
	_char				m_szName[MAX_PATH] = ""; //! CBone에 저장된 뼈의 이름과 동일해.

	//TODO 키프레임이 뭐야?
	//!특정 동작 을 의미해. 애니메이션에서 사용하고있는 뼈들이 시간에 맞는 동작들이 정의되있다고 했었잖아.
	//! 그 각각의 동작을 키프레임이라고 불러.
	
	//! 애니메이션이 가지고있는 동작의 개수라는거겠지? 이건 주의가 필요해  cpp에서 설명할게 #키프레임개수_주의점
	_uint				m_iNumKeyFrames = { 0 }; 
	vector<KEYFRAME>	m_KeyFrames; //! 키프레임 구조체는 뼈 하나에 대한 상태를 가질거야.

	//! 매번 순회해서 현재 인덱스를 찾아오기 보다는 애초에 재생할때부터 인덱스를 갱신 해주는게 좋을 거같아 멤버변수로 둔거야.
	//! 현재 키프레임이 0이고 트랙포지션이 0과 1사이에 있다고 가정하면 현재 인덱스는 0이겠지?
	_uint				m_iCurrentKeyFrameIndex = { 0 };

public:
	static CChannel* Create(const aiNodeAnim* pChannel);
	virtual void Free() override;
};

END
