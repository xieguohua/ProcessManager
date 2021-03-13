#include "stdafx.h"
#include "BkTaskData.h"

namespace BkWin {

TaskBase::TaskBase()
{
	m_uDelayExcute = 0;
	m_dwStartTickCount = 0;
	m_bMustExcute = FALSE;
	m_lpTarget = NULL;
}

TaskBase::~TaskBase()
{

}

void TaskBase::StartTickCount()
{
	m_dwStartTickCount = ::GetTickCount();
}

bool TaskBase::ShouldExcute()
{
	return ::GetTickCount() - m_dwStartTickCount >= m_uDelayExcute;
}

void TaskBase::SetMustExcute(bool bMustExcute)
{
	m_bMustExcute = bMustExcute;
}

bool TaskBase::IsMustExcute() const
{
	return m_bMustExcute;
}

TaskBase* TaskBase::SetDelayExcuteTime(UINT uDelayTime)
{
	m_uDelayExcute = uDelayTime;

	return this;
}

void TaskBase::SetTarget(LPVOID lpTarget)
{
	m_lpTarget = lpTarget;
}

LPVOID TaskBase::GetTarget() const
{
	return m_lpTarget;
}

}
