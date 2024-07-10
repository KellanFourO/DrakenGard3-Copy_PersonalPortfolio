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
	string			  Get_Name() { return m_szName; }
	_uint			  Get_CurrentKeyFrame() { return m_iCurrentKeyFrameIndex; }
	_uint			  Get_NumKeyFrames() { return m_iNumKeyFrames; }

public:
	HRESULT			  Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	void			  Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones);
	void			  Blend_TransformationMatrix(_float fBlendTime, _float fRatio, const CModel::BONES& Bones);
	void			  Reset_KeyFrame();

private:
	_char				m_szName[MAX_PATH] = ""; //! CBone에 저장된 뼈의 이름과 동일해.
	_uint				m_iNumKeyFrames = { 0 }; 
	vector<KEYFRAME>	m_KeyFrames; //! 키프레임 구조체는 뼈 하나에 대한 상태를 가질거야.
	vector<KEYFRAME>	m_PreKeyFrames; //! 이전 동작의 키프레임
	_uint				m_iBoneIndex = { 0 };
	_uint				m_iCurrentKeyFrameIndex = { 0 };

public:
	//! Create 할때 인자로 뼈들이 있는 벡터는 왜 받아왔어?
	//! 채널은 애니메이션 에서 사용하는 뼈의정보라고 했었어. 뼈들이 있는 벡터를 순회하면서 같은 이름을 가진 뼈를 찾아서 그 친구의 인덱스를 가져오려고 한거야
	//! 이 채널은 누가 들고있어? 애니메이션 객체가 들고있지. 그래서 애니메이션 객체를 생성할때부터 연결고리로 Bones 벡터를 받아와서 최종적으로 채널이 순회할수 있게 해준거야.
	static CChannel* Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	virtual void Free() override;
};

END
