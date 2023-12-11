#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pChannel->mNodeName.data);


	//! ¸ğµ¨ÀÌ µé°íÀÖ´Â »À¿Í Æ¯Á¤ ¾Ö´Ï¸ŞÀÌ¼Ç¿¡¼­ »ç¿ëÇÏ´Â »ÀÀÇÁ¤º¸ÀÇ ÀÌ¸§°ú °°Àº °É Ã£¾Æ¼­ ÀÎµ¦½º¸¦ °¡Á®¿Ã°Å¾ß.
	_uint	iBoneIndex = { 0 };

	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
			{
				return true;
			}

			++iBoneIndex;

			return false;
		});

	if(iter == Bones.end())
		return E_FAIL;

	m_iBoneIndex = iBoneIndex; //! Ã£¾Ò´Ù

	//!#Å°ÇÁ·¹ÀÓ°³¼ö_ÁÖÀÇÁ¡
	//!  ½ºÄÉÀÏ, ·ÎÅ×ÀÌ¼Ç,Æ÷Áö¼ÇÀÇ Å°ÇÁ·¹ÀÓÀÇ °³¼ö´Â °¢ÀÚ ´Ù¸¦ ¼ö ÀÖ¾î.
	//!  ¿Ö³ÄÇÏ¸é,  ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ ±¸µ¿µÉ¶§ Å©±âÀÇ º¯È­´Â ¾øÀÌ È¸Àü¸¸ ÇÒ ¼öµµ ÀÖ°í ¸¶Âù°¡Áö·Î À§Ä¡´Â ´Ş¶óÁö´Âµ¥ È¸ÀüÀº ÇÏÁö ¾ÊÀ»¼öµµ ÀÖ´Â °ÅÁö.
	//! Áï, ÀÌÀü À§Ä¡ ±×´ë·ÎÀÏ ¼öµµ ÀÖ´Ù´Â°Å¾ß. ±×·¡¼­ Å° ÇÁ·¹ÀÓÀÇ °³¼ö´Â ´Ù ´Ù¸£´Ï 
	//! ÃÑ Å°ÇÁ·¹ÀÓÀÇ °³¼ö´Â ½ºÄÉÀÏ,·ÎÅ×ÀÌ¼Ç,Æ÷Áö¼Ç °¢ °¡Áö°íÀÖ´Â Å°ÇÁ·¹ÀÓÀÇ °³¼öµéÁß °¡Àå Å«°ªÀ» °¡Áø ³ğÀ¸·Î Ã£¾Æ¼­ º¸°üÇÒ°Å¾ß.
	m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(pChannel->mNumPositionKeys, m_iNumKeyFrames);

	//TODO À§ ÁÖÀÇÁ¡¿¡¼­ ¸»Çßµí ÀÌÀü À§Ä¡¸¦ ±â¾ïÇØ¾ß°ÚÁö?
	//!  ¹Ø¿¡¼­ ·çÇÁ¸¦ µ¹¸±°Çµ¥, ¿¹¸¦µé¾î °ø°İ ¾Ö´Ï¸ŞÀÌ¼ÇÀÇ 3¹ø ÇÁ·¹ÀÓÀÇ ½ºÄÉÀÏÀº Á¸ÀçÇÏÁö¾ÊÀ» ¼ö ÀÖ¾î.
	//! ±×·¡¼­ °¢°¢ Á¶°Ç¹®À» °É¾î¼­ °É·¯ÁÙ°Çµ¥, ±× Á¶°Ç¹®À» Åë°úÇÏÁö¸øÇÑ´Ù¸é ±×³É 0À¸·Î Ã¤¿öÁú°Å¾ß. 
	//! ·çÇÁ¹® µ¹±âÀü¿¡ º¯¼ö¸¦ ¼±¾ğÇØ³õ°í °ªÀ» Ã¤¿öÁà¼­ ÀÌÀü °ªÀ» ±â¾ïÇÏ°Ô ÇÏÀÚ.
	_float3 vScale;
	_float4	vRotation;
	_float3	vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame = {};

		if (i < pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mScalingKeys[i].mTime;
		}

		if (i < pChannel->mNumRotationKeys)
		{
			//! ¾î½ÉÇÁÀÇ ·ÎÅ×ÀÌ¼ÇÅ°ÀÇ ¹ë·ù´Â aiQuaternionÀÌ¾ß. x,y,z,w ¼øÀÌ¾Æ´Ñ w,x,y,z ¼øÀ¸·Î µÇ¾îÀÖ¾î
			//! ¿ì¸®°¡ ¾²´ø °Å¶û ¼ø¼­°¡ ´Ù¸£Áö? ±×·¡¼­ memcpy¸¦ »ç¿ëÇÏ¸é ¾ÈµÅ.
		
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pChannel->mRotationKeys[i].mTime;
		}

		if (i < pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

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

	KEYFRAME	LastKeyFrame = m_KeyFrames.back(); //! ¾Ö´Ï¸ŞÀÌ¼ÇÀÇ ¸¶Áö¸· ÇÁ·¹ÀÓÀ» ¾ê±âÇÏ´Â°Å°ÚÁö?

	//! ¸¶Áö¸· Å°ÇÁ·¹ÀÓÀº ÇØ´ç¾Ö´Ï¸ŞÀÌ¼ÇÀÇ ÀüÃ¼±æÀÌÀÎ µà·¹ÀÌ¼ÇÀÌ Á¾·áµÉ¶§±îÁö ÀÚ±â »óÅÂ¸¦ À¯Áö½ÃÄÑÁà¾ß°ÚÁö?
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	
	//! ÀÌÁ¦ ÇöÀç ÇÁ·¹ÀÓ°ú ´ÙÀ½ ÇÁ·¹ÀÓ°ú ¼±Çüº¸°£À» ÅëÇØ ÇöÀçÀÇ »óÅÂ¸¦ °»½Å½ÃÄÑÁà¾ß°ÚÁö?
	else //! ¸¶Áö¸· ÇÁ·¹ÀÓÀÇ ´ÙÀ½ÇÁ·¹ÀÓÀº ¾øÀİ¾Æ? ±×·¡¼­ ÇöÀç ÇÁ·¹ÀÓ +1ÀÌ¶ó´Â Á¶°ÇÀÌ ¸»ÀÌ ¾ÈµÅ. ±Ùµ¥ À§¿¡¼­ if·Î °É·¯ÁáÀİ¾Æ. else ÇØÁÖÀÚ
	{
		//! Á¡ÇÁ¶ó´Â ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ ÀÖ´Ù°í Ä¡ÀÚ. Á¡ÇÁÇÏ·Á¸é ´Ù¸®ÀÇ ÈûÀ» ÁÖ´Â ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ 0¹ø ÇÁ·¹ÀÓÀÌ¶ó°í ÇØº¸ÀÚ°í.
		//! ÇöÀç ¾Ö´Ï¸ŞÀÌ¼Ç Àç»ıÀ§Ä¡°¡ 0¹øÇÁ·¹ÀÓº¸´Ù Ä¿Á³´Ù¸é ¹«¸­ÀÌ ÆìÁø´Ù´Â ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ 1¹ø ÇÁ·¹ÀÓÀÌ¶ó¸é 1¹øÇÁ·¹ÀÓÀ¸·Î ¹Ù²î¾î¾ß°ÚÁö?
		
		//TODO While¹®À» »ç¿ëÇÑ ÀÌÀ¯.
		//! ÇÁ·¹ÀÓÀÌ ¶³¾îÁö¸é ¸Ş½Ã°¡ ±úÁö´Â Çö»óÀÌ ¹ß»ıÇØ.
		//! ¿Ö ±úÁö´Â°É±î? ¸ğµ¨¿¡ Play_Animation ÇÔ¼ö¸¦ È£ÃâÇÏ¸é¼­ ³»ºÎ¿¡¼­ Invalidate_Combie ÇÔ¼ö¸¦ È£ÃâÇØ¼­ ÇØ´ç »ÀÀÇ Combine Çà·ÄÀ» °»½ÅÇØÁÖ°íÀÖ¾ú¾î
		//! ±×·±µ¥ ÇÁ·¹ÀÓÀÌ ¶³¾îÁö¸é¼­ ³Ñ°ÜÁÖ´Â ÀÎÀÚ°ªÀÌ¾ú´ø TimeDelta °ªÀÌ »ı°¢º¸´Ù ³Ê¹« Å« °ªÀÌ ³Ñ°ÜÁ® ¹ö¸°°Å¾ß.
		//! ±×·¡¼­, ÇÁ·¹ÀÓ ÀÎµ¦½º°¡ ÇÑ¹ø¿¡ °Ç³Ê¶Ù¾î¹ö¸°°ÅÁö. ¿¹¸¦ µéÀÚ¸é 1¿¡¼­ ÇÑ¹ø¿¡ 3À¸·Î.
		//! ¿©±â¼­ ´ÙÀ½ Å°ÇÁ·¹ÀÓÀº 2·Î ¸¸µé¾îÁ®¹ö¸°°Å¾ß. 4°¡¾Æ´Ï¶ó. ¿©±â¼­ Âü»ç°¡ ¹ß»ıÇÑ°Å¾ß ÀÌ°É ¸·±âÀ§ÇØ While¹®À» µ¹·ÁÁØ°ÅÁö.
		
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
		//!  ÇöÀç¾Ö´Ï¸ŞÀÌ¼ÇÀ§Ä¡ ¿¡¼­ 
		_float	fRatio = 
		(fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition) /
		(m_KeyFrames[*pCurrentKeyFrame + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition);

		//! 1¹øÂ° ÀÎÀÚ¿Í 2¹øÂ° ÀÎÀÚ¸¦ ºñÀ²¸¸Å­ ¼±Çüº¸°£ÇØÁà¼­ ÃÖÁ¾ °á°úº¤ÅÍ¸¦ “Ê¹ñ¾îÁØ´Ü¸»ÀÌÁö ¾ÆÁÖ ¯„ÄªÂùÇØ
		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		//! Àã´ñ´Ù. ÄõÅÍ´Ï¾ğÀº ¾îÂ¼Áö? ÀÌ°Íµµ µû·Î ÇÔ¼öÀÖ´Ù °³²Ü?
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
	}

	//! À§¿¡¼­ Àú »óÅÂµé·Î ¹¹ÇÏ·Á°íÇÑ°Å¾ß? ½Ã°£¿¡¸Â´Â TransformationMatrix ¸¸µé¾îÁÖ·Á°í Çß´ø°ÅÀİ¾Æ?
	//! ¾î¶»°Ô ¸¸µé°Å¾ß? ·ÎÅ×ÀÌ¼ÇÀº ÄõÅÍ´Ï¾ğÀÎµ¥? ÀÌ°Íµµ ÇÔ¼öÀÖ¾î °³²Ü ¤»¤»
	//! ÀÀ? ÀÎÀÚ°ªÀ¸·Î ·ÎÅ×ÀÌ¼Ç ¿øÁ¡À» ´Ş¶ó´Âµ¥? ¿øÁ¡ÀÌ¹¹Áö? 0,0,0,1 ÀÌÀİ¾Æ ÁÖ¸éµÇÁö ¤»¤»
	_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f,0.f,0.f,1.f), vRotation, vPosition);

	//! ÀÌÁ¦ ÀÌ Ã¤³Î°ú °°Àº ÀÌ¸§À» °¡Áø »À¸¦ Ã£¾Æ¼­ ±× »ÀÀÇ TransformationMatrix °»½ÅÇØÁÙ°Å¾ß.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannel, Bones)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
