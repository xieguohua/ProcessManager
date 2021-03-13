#include "stdafx.h"
#include "BkTimerMgr.h"

namespace BkTimer {

CBkWndTimerMgr::CBkWndTimerMgr( void )
{
	m_uTimerID = 0;
	m_uTimerElapse = 0;
	m_hWndOwner = NULL;
	m_bStartTimer = FALSE;
	m_bInint = FALSE;
}

CBkWndTimerMgr::~CBkWndTimerMgr( void )
{

}

BOOL CBkWndTimerMgr::SetTimer( IBkTimerNotify* pNotify, UINT uTimerID, UINT uTimerElapse )
{
	BOOL bReturn = FALSE;

	if (!m_bInint || pNotify == NULL || uTimerElapse == 0 || uTimerID == 0) goto Exit0;

	if (!m_bStartTimer)
	{
		::SetTimer(m_hWndOwner, m_uTimerID, m_uTimerElapse, NULL);
		m_bStartTimer = TRUE;
	}

	TIMER_DATA* pTimerData = Find(pNotify, uTimerID);
	if (pTimerData)
	{
		pTimerData->bKilled = FALSE;
		pTimerData->pNotify = pNotify;
		pTimerData->dwStartTick = GetTickCount();
		pTimerData->dwTimerLast = uTimerElapse;

		bReturn = TRUE;
		goto Exit0;
	}


	pTimerData = new TIMER_DATA;
	if (NULL == pTimerData) goto Exit0;

	if (pTimerData)
	{
		pTimerData->pNotify = pNotify;
		pTimerData->dwStartTick = GetTickCount();
		pTimerData->dwTimerLast = uTimerElapse;
		pTimerData->uTimerID = uTimerID;
		pTimerData->bKilled = FALSE;

		m_lstTimerData.push_back(pTimerData);
	}
	
	bReturn = TRUE;

Exit0:
	return bReturn;
}


void CBkWndTimerMgr::KillTimer( IBkTimerNotify* pNotify, UINT uTimerID )
{
	TIMER_DATA* pTimerData = Find(pNotify, uTimerID);

	if (pTimerData)
	{
		pTimerData->bKilled = TRUE;
		pTimerData->dwStartTick = 0;
	}
}

void CBkWndTimerMgr::Init( HWND hWndOwner, UINT uTimerID, UINT uTimerLast )
{
	UnInit();

	m_hWndOwner = hWndOwner;
	m_uTimerID = uTimerID;
	m_uTimerElapse = uTimerLast;
	m_bInint = TRUE;
}

void CBkWndTimerMgr::UnInit()
{
	if (m_bStartTimer)
	{
		::KillTimer(m_hWndOwner, m_uTimerID);
        m_bStartTimer = FALSE;
	}

	m_hWndOwner = NULL;
	m_uTimerID = 0;
	m_uTimerElapse = 0;

	LSTNotifys::iterator iter = m_lstTimerData.begin();

	while (iter != m_lstTimerData.end())
	{
		if (*iter)
		{
			delete *iter;
		}

		++iter;
	}

	m_lstTimerData.clear();
	m_bInint = FALSE;
}

void CBkWndTimerMgr::TriggerTimerEvent( UINT uTimerID )
{
	if (!m_bStartTimer || m_uTimerID != uTimerID )
		return;

	if  (m_lstTimerData.size() == 0)
	{
		::KillTimer(m_hWndOwner, m_uTimerID);
		m_bStartTimer = FALSE;
		return;
	}

	LSTNotifys lstTimerData = m_lstTimerData;

	OnTimerImpl(lstTimerData);
}

CBkWndTimerMgr::TIMER_DATA* CBkWndTimerMgr::Find( IN IBkTimerNotify* pNotify, UINT uTimerID )
{
	TIMER_DATA* pTimerDataFind = NULL;
	TIMER_DATA* pTimerData2 = NULL;

	if (pNotify != NULL)
	{
		LSTNotifys::iterator iter = m_lstTimerData.begin();

		while (iter != m_lstTimerData.end())
		{
			pTimerData2 = (*iter);

			if (pTimerData2 && 
				pTimerData2->pNotify == pNotify && 
				pTimerData2->uTimerID == uTimerID)
			{
				pTimerDataFind = *iter;
				break;
			}

			++iter;
		}
	}

	return pTimerDataFind;
}


void CBkWndTimerMgr::OnTimerImpl(LSTNotifys& listTimerData)
{
	TIMER_DATA* pTimerData = NULL;
	LSTNotifys::iterator iter = listTimerData.begin();

	while (iter != listTimerData.end())
	{
		pTimerData = *iter;

		if (pTimerData == NULL || pTimerData->bKilled)
		{
			EraseTimerData(pTimerData);
			++iter;
			continue;
		}

		if (GetTickCount() - pTimerData->dwStartTick >= pTimerData->dwTimerLast)
		{
			pTimerData->dwStartTick = GetTickCount();

			if (pTimerData->pNotify)
				pTimerData->pNotify->OnBkTimer(pTimerData->uTimerID);
		}

		++iter;
	}
}


void CBkWndTimerMgr::EraseTimerData( TIMER_DATA* pTimerData )
{
	if (pTimerData != NULL)
	{
		LSTNotifys::iterator iter = m_lstTimerData.begin();

		while (iter != m_lstTimerData.end())
		{
			if (pTimerData == *iter)
			{
				iter = m_lstTimerData.erase(iter);
				delete pTimerData;
				break;
			}

			++iter;
		}
	}
}

void CBkWndTimerMgr::KillAlllTimer( IBkTimerNotify* pNotify )
{
	if (pNotify != NULL)
	{
		LSTNotifys::iterator iter = m_lstTimerData.begin();

		while (iter != m_lstTimerData.end())
		{
			if (*iter && (*iter)->pNotify == pNotify)
				(*iter)->bKilled = TRUE;

			++iter;
		}
	}
}
}