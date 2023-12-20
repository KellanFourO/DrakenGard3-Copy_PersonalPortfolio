#pragma once
#include "Model.h"

//TODO 애니메이션에서 사용하는 뼈의 정보
//! 시간대에 따른 뼈의 상태(Scale, Rotation, Position)을 저장할거야.

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	void	Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame);
	_bool	Blend_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame, KEYFRAME& pPrevKeyFrame);
	vector<KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; }
	KEYFRAME& Get_FrontKeyFrame() { return m_KeyFrames.front(); }

	string  Get_Name() { return m_szName; }

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
	//! 복제해줄때 커런트키프레임은 다 달라야한다. 애니메이션 클래스가 던져주기로 구조가 바뀌었다.
	//_uint				m_iCurrentKeyFrameIndex = { 0 };

	_uint				m_iBoneIndex = { 0 };
	

public:
	//! Create 할때 인자로 뼈들이 있는 벡터는 왜 받아왔어?
	//! 채널은 애니메이션 에서 사용하는 뼈의정보라고 했었어. 뼈들이 있는 벡터를 순회하면서 같은 이름을 가진 뼈를 찾아서 그 친구의 인덱스를 가져오려고 한거야
	//! 이 채널은 누가 들고있어? 애니메이션 객체가 들고있지. 그래서 애니메이션 객체를 생성할때부터 연결고리로 Bones 벡터를 받아와서 최종적으로 채널이 순회할수 있게 해준거야.
	static CChannel* Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	virtual void Free() override;
};

END
