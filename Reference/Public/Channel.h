#pragma once
#include "Base.h"

//TODO �ִϸ��̼ǿ��� ����ϴ� ���� ����
//! �ð��뿡 ���� ���� ����(Scale, Rotation, Position)�� �����Ұž�.

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

public:
	static CChannel* Create(const aiNodeAnim* pChannel);
	virtual void Free() override;
};

END
