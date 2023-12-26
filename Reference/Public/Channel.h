#pragma once
#include "Model.h"

//TODO �ִϸ��̼ǿ��� ����ϴ� ���� ����
//! �ð��뿡 ���� ���� ����(Scale, Rotation, Position)�� �����Ұž�.

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
	_char				m_szName[MAX_PATH] = ""; //! CBone�� ����� ���� �̸��� ������.
	_uint				m_iNumKeyFrames = { 0 }; 
	vector<KEYFRAME>	m_KeyFrames; //! Ű������ ����ü�� �� �ϳ��� ���� ���¸� �����ž�.
	vector<KEYFRAME>	m_PreKeyFrames; //! ���� ������ Ű������
	_uint				m_iBoneIndex = { 0 };
	_uint				m_iCurrentKeyFrameIndex = { 0 };

public:
	//! Create �Ҷ� ���ڷ� ������ �ִ� ���ʹ� �� �޾ƿԾ�?
	//! ä���� �ִϸ��̼� ���� ����ϴ� ����������� �߾���. ������ �ִ� ���͸� ��ȸ�ϸ鼭 ���� �̸��� ���� ���� ã�Ƽ� �� ģ���� �ε����� ���������� �Ѱž�
	//! �� ä���� ���� ����־�? �ִϸ��̼� ��ü�� �������. �׷��� �ִϸ��̼� ��ü�� �����Ҷ����� ������� Bones ���͸� �޾ƿͼ� ���������� ä���� ��ȸ�Ҽ� �ְ� ���ذž�.
	static CChannel* Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	virtual void Free() override;
};

END
