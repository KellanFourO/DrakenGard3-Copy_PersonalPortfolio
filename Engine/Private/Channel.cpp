#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	strcpy_s(m_szName, strName.c_str());

	m_KeyFrames.reserve(Keyframes.size());
	for (auto& iter : Keyframes)
		m_KeyFrames.push_back(iter);

	m_iNumKeyFrames = m_KeyFrames.size();

	m_iBoneIndex = iBoneIndex;
	return S_OK;

}

void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame)
{
	//! Ä¿·±Æ® Æ®·¢Æ÷Áö¼ÇÀÌ 0.0À¸·Î µé¾î¿À´Â °æ¿ì´Â ·çÇÁµ¹¸±¶§ ¸»°í´Â ¾ø¾î. ±×·¯´Ï±î ÀÎµ¦½º¸¦ 0À¸·Î ÃÊ±âÈ­ ½ÃÄÑÁÖÀÚ
	if(0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrame = 0;

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back(); //! ¾Ö´Ï¸ÞÀÌ¼ÇÀÇ ¸¶Áö¸· ÇÁ·¹ÀÓÀ» ¾ê±âÇÏ´Â°Å°ÚÁö?

	//! ¸¶Áö¸· Å°ÇÁ·¹ÀÓÀº ÇØ´ç¾Ö´Ï¸ÞÀÌ¼ÇÀÇ ÀüÃ¼±æÀÌÀÎ µà·¹ÀÌ¼ÇÀÌ Á¾·áµÉ¶§±îÁö ÀÚ±â »óÅÂ¸¦ À¯Áö½ÃÄÑÁà¾ß°ÚÁö?
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	
	//! ÀÌÁ¦ ÇöÀç ÇÁ·¹ÀÓ°ú ´ÙÀ½ ÇÁ·¹ÀÓ°ú ¼±Çüº¸°£À» ÅëÇØ ÇöÀçÀÇ »óÅÂ¸¦ °»½Å½ÃÄÑÁà¾ß°ÚÁö?
	else //! ¸¶Áö¸· ÇÁ·¹ÀÓÀÇ ´ÙÀ½ÇÁ·¹ÀÓÀº ¾øÀÝ¾Æ? ±×·¡¼­ ÇöÀç ÇÁ·¹ÀÓ +1ÀÌ¶ó´Â Á¶°ÇÀÌ ¸»ÀÌ ¾ÈµÅ. ±Ùµ¥ À§¿¡¼­ if·Î °É·¯ÁáÀÝ¾Æ. else ÇØÁÖÀÚ
	{
		//! Á¡ÇÁ¶ó´Â ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖ´Ù°í Ä¡ÀÚ. Á¡ÇÁÇÏ·Á¸é ´Ù¸®ÀÇ ÈûÀ» ÁÖ´Â ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ 0¹ø ÇÁ·¹ÀÓÀÌ¶ó°í ÇØº¸ÀÚ°í.
		//! ÇöÀç ¾Ö´Ï¸ÞÀÌ¼Ç Àç»ýÀ§Ä¡°¡ 0¹øÇÁ·¹ÀÓº¸´Ù Ä¿Á³´Ù¸é ¹«¸­ÀÌ ÆìÁø´Ù´Â ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ 1¹ø ÇÁ·¹ÀÓÀÌ¶ó¸é 1¹øÇÁ·¹ÀÓÀ¸·Î ¹Ù²î¾î¾ß°ÚÁö?
		
		//TODO While¹®À» »ç¿ëÇÑ ÀÌÀ¯.
		//! ÇÁ·¹ÀÓÀÌ ¶³¾îÁö¸é ¸Þ½Ã°¡ ±úÁö´Â Çö»óÀÌ ¹ß»ýÇØ.
		//! ¿Ö ±úÁö´Â°É±î? ¸ðµ¨¿¡ Play_Animation ÇÔ¼ö¸¦ È£ÃâÇÏ¸é¼­ ³»ºÎ¿¡¼­ Invalidate_Combie ÇÔ¼ö¸¦ È£ÃâÇØ¼­ ÇØ´ç »ÀÀÇ Combine Çà·ÄÀ» °»½ÅÇØÁÖ°íÀÖ¾ú¾î
		//! ±×·±µ¥ ÇÁ·¹ÀÓÀÌ ¶³¾îÁö¸é¼­ ³Ñ°ÜÁÖ´Â ÀÎÀÚ°ªÀÌ¾ú´ø TimeDelta °ªÀÌ »ý°¢º¸´Ù ³Ê¹« Å« °ªÀÌ ³Ñ°ÜÁ® ¹ö¸°°Å¾ß.
		//! ±×·¡¼­, ÇÁ·¹ÀÓ ÀÎµ¦½º°¡ ÇÑ¹ø¿¡ °Ç³Ê¶Ù¾î¹ö¸°°ÅÁö. ¿¹¸¦ µéÀÚ¸é 1¿¡¼­ ÇÑ¹ø¿¡ 3À¸·Î.
		//! ¿©±â¼­ ´ÙÀ½ Å°ÇÁ·¹ÀÓÀº 2·Î ¸¸µé¾îÁ®¹ö¸°°Å¾ß. 4°¡¾Æ´Ï¶ó. ¿©±â¼­ Âü»ç°¡ ¹ß»ýÇÑ°Å¾ß ÀÌ°É ¸·±âÀ§ÇØ While¹®À» µ¹·ÁÁØ°ÅÁö.
		
		while(fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrame + 1].fTrackPosition)
			++*pCurrentKeyFrame;

		//! ¼Ò½º¿Í µ¥½ºÆ®¾ß. ¼Ò½º´Â ÇöÀç ÇÁ·¹ÀÓÀÎ 0¹ø ÇÁ·¹ÀÓÀÌ °¡Áø »óÅÂ¸¦ ÀÇ¹ÌÇÏ°í, µ¥½ºÆ®´Â ´ÙÀ½ ÇÁ·¹ÀÓÀÎ 1¹ø ÇÁ·¹ÀÓÀÇ »óÅÂ¸¦ ÀÇ¹ÌÇØ.
		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale		= m_KeyFrames[*pCurrentKeyFrame].vScale;
		vSourRotation	= m_KeyFrames[*pCurrentKeyFrame].vRotation;
		vSourPosition	= m_KeyFrames[*pCurrentKeyFrame].vPosition;
						
		vDestScale		= m_KeyFrames[*pCurrentKeyFrame + 1].vScale;
		vDestRotation	= m_KeyFrames[*pCurrentKeyFrame + 1].vRotation;
		vDestPosition	= m_KeyFrames[*pCurrentKeyFrame + 1].vPosition;


		//! XMVectorLerp¶ó´Â ÇÔ¼ö´Â ¾ÆÁÖ ±âÆ¯ÇÏ°Ôµµ ºñÀ²¸¸ ³Ö¾îÁÖ¸é ¼±Çüº¸°£À» ¿¹»Ú°Ô ÇØÁØ´Ü¸»ÀÌÁö. ºñÀ² ±¸ÇÏÀÚ
		//!  ÇöÀç¾Ö´Ï¸ÞÀÌ¼ÇÀ§Ä¡ ¿¡¼­ 
		_float	fRatio = 
		(fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition) /
		(m_KeyFrames[*pCurrentKeyFrame + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition);

		//! 1¹øÂ° ÀÎÀÚ¿Í 2¹øÂ° ÀÎÀÚ¸¦ ºñÀ²¸¸Å­ ¼±Çüº¸°£ÇØÁà¼­ ÃÖÁ¾ °á°úº¤ÅÍ¸¦ “Ê¹ñ¾îÁØ´Ü¸»ÀÌÁö ¾ÆÁÖ ¯„ÄªÂùÇØ
		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		//! Àã´ñ´Ù. ÄõÅÍ´Ï¾ðÀº ¾îÂ¼Áö? ÀÌ°Íµµ µû·Î ÇÔ¼öÀÖ´Ù °³²Ü?
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
	}

	//! À§¿¡¼­ Àú »óÅÂµé·Î ¹¹ÇÏ·Á°íÇÑ°Å¾ß? ½Ã°£¿¡¸Â´Â TransformationMatrix ¸¸µé¾îÁÖ·Á°í Çß´ø°ÅÀÝ¾Æ?
	//! ¾î¶»°Ô ¸¸µé°Å¾ß? ·ÎÅ×ÀÌ¼ÇÀº ÄõÅÍ´Ï¾ðÀÎµ¥? ÀÌ°Íµµ ÇÔ¼öÀÖ¾î °³²Ü ¤»¤»
	//! ÀÀ? ÀÎÀÚ°ªÀ¸·Î ·ÎÅ×ÀÌ¼Ç ¿øÁ¡À» ´Þ¶ó´Âµ¥? ¿øÁ¡ÀÌ¹¹Áö? 0,0,0,1 ÀÌÀÝ¾Æ ÁÖ¸éµÇÁö ¤»¤»
	_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f,0.f,0.f,1.f), vRotation, vPosition);

	//! ÀÌÁ¦ ÀÌ Ã¤³Î°ú °°Àº ÀÌ¸§À» °¡Áø »À¸¦ Ã£¾Æ¼­ ±× »ÀÀÇ TransformationMatrix °»½ÅÇØÁÙ°Å¾ß.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(strName, Keyframes, iBoneIndex)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
