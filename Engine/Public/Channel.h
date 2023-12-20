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
	HRESULT Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	void	Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame);
	_bool	Blend_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame, KEYFRAME& pPrevKeyFrame);
	vector<KEYFRAME>& Get_KeyFrames() { return m_KeyFrames; }
	KEYFRAME& Get_FrontKeyFrame() { return m_KeyFrames.front(); }

	string  Get_Name() { return m_szName; }

private:
	_char				m_szName[MAX_PATH] = ""; //! CBone�� ����� ���� �̸��� ������.

	//TODO Ű�������� ����?
	//!Ư�� ���� �� �ǹ���. �ִϸ��̼ǿ��� ����ϰ��ִ� ������ �ð��� �´� ���۵��� ���ǵ��ִٰ� �߾��ݾ�.
	//! �� ������ ������ Ű�������̶�� �ҷ�.
	
	//! �ִϸ��̼��� �������ִ� ������ ������°Ű���? �̰� ���ǰ� �ʿ���  cpp���� �����Ұ� #Ű�����Ӱ���_������
	_uint				m_iNumKeyFrames = { 0 }; 
	vector<KEYFRAME>	m_KeyFrames; //! Ű������ ����ü�� �� �ϳ��� ���� ���¸� �����ž�.

	//! �Ź� ��ȸ�ؼ� ���� �ε����� ã�ƿ��� ���ٴ� ���ʿ� ����Ҷ����� �ε����� ���� ���ִ°� ���� �Ű��� ��������� �аž�.
	//! ���� Ű�������� 0�̰� Ʈ���������� 0�� 1���̿� �ִٰ� �����ϸ� ���� �ε����� 0�̰���?
	//! �������ٶ� Ŀ��ƮŰ�������� �� �޶���Ѵ�. �ִϸ��̼� Ŭ������ �����ֱ�� ������ �ٲ����.
	//_uint				m_iCurrentKeyFrameIndex = { 0 };

	_uint				m_iBoneIndex = { 0 };
	

public:
	//! Create �Ҷ� ���ڷ� ������ �ִ� ���ʹ� �� �޾ƿԾ�?
	//! ä���� �ִϸ��̼� ���� ����ϴ� ����������� �߾���. ������ �ִ� ���͸� ��ȸ�ϸ鼭 ���� �̸��� ���� ���� ã�Ƽ� �� ģ���� �ε����� ���������� �Ѱž�
	//! �� ä���� ���� ����־�? �ִϸ��̼� ��ü�� �������. �׷��� �ִϸ��̼� ��ü�� �����Ҷ����� ������� Bones ���͸� �޾ƿͼ� ���������� ä���� ��ȸ�Ҽ� �ְ� ���ذž�.
	static CChannel* Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex);
	virtual void Free() override;
};

END
