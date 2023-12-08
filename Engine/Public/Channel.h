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
	HRESULT Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones);

public:
	void Set_AnimationIndex(_uint AnimIndex) { m_iCurrentKeyFrameIndex = AnimIndex; }

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
	_uint				m_iCurrentKeyFrameIndex = { 0 };

	_uint				m_iBoneIndex = { 0 };
	_float				m_fInterpolation = { 0.2f };

public:
	//! Create �Ҷ� ���ڷ� ������ �ִ� ���ʹ� �� �޾ƿԾ�?
	//! ä���� �ִϸ��̼� ���� ����ϴ� ����������� �߾���. ������ �ִ� ���͸� ��ȸ�ϸ鼭 ���� �̸��� ���� ���� ã�Ƽ� �� ģ���� �ε����� ���������� �Ѱž�
	//! �� ä���� ���� ����־�? �ִϸ��̼� ��ü�� �������. �׷��� �ִϸ��̼� ��ü�� �����Ҷ����� ������� Bones ���͸� �޾ƿͼ� ���������� ä���� ��ȸ�Ҽ� �ְ� ���ذž�.
	static CChannel* Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END
