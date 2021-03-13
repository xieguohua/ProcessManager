//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/16?21:32
//description: √Ë ˆ
//*********************************************************************

#ifndef _H_BK_IBKTIMER_H_
#define _H_BK_IBKTIMER_H_

namespace BkTimer
{
	class IBkTimerNotify
	{
	public:

		virtual void OnBkTimer(UINT uItmerID) = 0;
	};

	class IBkWndTimer
	{
	public:
		virtual void KillTimer(IBkTimerNotify* pNotify, UINT uTimerID) = 0;
		virtual	BOOL SetTimer(IBkTimerNotify* pNotify,  UINT uItmerID, UINT uTimerElapse) = 0;
		virtual void KillAlllTimer(IBkTimerNotify* pNotify) = 0;
	};
}


#endif