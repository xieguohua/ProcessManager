//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/16?21:33
//description: √Ë ˆ
//*********************************************************************

#ifndef _H_BKWIN_BKTIMERMGR_H_
#define _H_BKWIN_BKTIMERMGR_H_

#include <set>
#include <map>
#include <list>
#include "IBkTimer.h"

namespace BkTimer 
{
	class CBkWndTimerMgr : public IBkWndTimer
	{
	public:

		struct TIMER_DATA
		{
			IBkTimerNotify* pNotify;
			DWORD		  dwTimerLast;
			DWORD		  dwStartTick;
			BOOL		  bKilled;
			UINT		  uTimerID;

			TIMER_DATA()
			{
				pNotify = NULL;
				dwTimerLast = NULL;
				dwStartTick = 0;
				bKilled = FALSE;
				uTimerID = 0;
			}
		};

		CBkWndTimerMgr(void);
		~CBkWndTimerMgr(void);

		void Init(HWND hWndOwner, UINT uTimerID, UINT uTimerLast);
		void UnInit();

		void TriggerTimerEvent(UINT uTimerID);

	protected:
		virtual BOOL SetTimer(IBkTimerNotify* pNotify,  UINT uItmerID, UINT uTimerLast );
		virtual void KillTimer(IBkTimerNotify* pNotify,  UINT uTimerID );
		virtual void KillAlllTimer(IBkTimerNotify* pNotify);

	protected:

		typedef UINT TIMER_ID;
		typedef std::list<TIMER_DATA*> LSTNotifys;

		TIMER_DATA*	Find(IN IBkTimerNotify* pNotify, UINT uTimerID);
		void		EraseTimerData(TIMER_DATA* pTimerData);
		void		OnTimerImpl(LSTNotifys& listTimerData);


	private:

		BOOL		m_bStartTimer;
		UINT		m_uTimerID;
		UINT		m_uTimerElapse;
		HWND		m_hWndOwner;
		BOOL		m_bInint;

		LSTNotifys  m_lstTimerData;
	};
}



#endif