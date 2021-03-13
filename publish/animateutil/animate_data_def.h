/* -----------------------------------------------------------------------
 *  File		:	animate_data_def.h
 *  Author	    :	hexujun
 *  Datet   	:	2014/2/18 11:03
 *  Brief		:	
 *
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------*/

#ifndef __animate_data_def__h__
#define __animate_data_def__h__
//
///////////////////////////////////////////////////////////////////////////
namespace ease_tween
{

template<class TEaseTween, class ValueT = float>
class KEaseTweenSimpleAnimateData
{
public:
    KEaseTweenSimpleAnimateData()
    {
        Clear();
    }

    virtual void Clear()
    {
        m_fCurTime		= 0;
        m_fTotalTime	= 0;
        m_fStartValue	= 0;
        m_fChangeValue  = 0;
    }

    ValueT AddTime(ValueT fAddTime)
    {
        ValueT fOldCurrTime = m_fCurTime;

        m_fCurTime += fAddTime;
        if (m_fCurTime > m_fTotalTime)
        {
            m_fCurTime = m_fTotalTime;
        }

        return fOldCurrTime;
    }


    virtual void SetValue(ValueT fTotalTime, ValueT fStartValue, ValueT fChangeValue)
    {
        m_fStartValue  = fStartValue;
        m_fTotalTime   = fTotalTime;
        m_fChangeValue = fChangeValue;
    }

    void SetParam(void* pParam)
    {
        m_pParam = pParam;
    }

    void* GetParam() const
    {
        return m_pParam;
    }

    ValueT Caculate() const
    {
		if(IsStop()) 
		{
			return m_fStartValue + m_fChangeValue;
		}

        return TEaseTween::calculate(m_fCurTime, m_fStartValue, m_fChangeValue, m_fTotalTime);
    }

    ValueT Caculate(ValueT fStartValue, ValueT fChangeValue) const
    {
        return TEaseTween::calculate(m_fCurTime, fStartValue, fChangeValue, m_fTotalTime);
    }

    bool IsStop() const
    {
        return m_fCurTime >= m_fTotalTime;
    }

    ValueT GetChangeValue() const
    {
        return m_fChangeValue;
    }

    ValueT GetStartValue() const
    {
        return m_fStartValue;
    }

    void SetCurrentTime(ValueT fCur)
    {
        m_fCurTime = fCur;
    }

    ValueT GetCurrentTime() const
    {
        return m_fCurTime;
    }

    ValueT GetTotalTime() const
    {
        return m_fTotalTime;
    }

protected:
    ValueT  m_fCurTime;
    ValueT  m_fTotalTime;
    ValueT  m_fStartValue;
    ValueT  m_fChangeValue;
    void*  m_pParam;
};

template<class T, class TEaseTween, class ValueT = float>
class KEaseTweenAnimateData
    : public KEaseTweenSimpleAnimateData<TEaseTween, ValueT>
{
public:

	typedef int (T::*pfun)(void*, ValueT);

	KEaseTweenAnimateData()
	{
	}

	virtual void Clear()
	{
        KEaseTweenSimpleAnimateData<TEaseTween>::Clear();

		m_pObj   = NULL;
		m_pParam = NULL;
		m_pFunc  = NULL;
	}

    virtual void SetValue(ValueT fTotalTime, ValueT fStartValue, ValueT fChangeValue)
    {
        KEaseTweenSimpleAnimateData<TEaseTween>::SetValue(fTotalTime, fStartValue, fChangeValue);

        m_pObj  = NULL;
        m_pFunc = NULL;
    }

	void SetValue(T* pObject, int (T::*f)(void*, ValueT), ValueT fTotalTime, ValueT fStartValue, ValueT fChangeValue)
	{
		m_fStartValue  = fStartValue;
		m_fTotalTime   = fTotalTime;
		m_fChangeValue = fChangeValue;

		m_pObj  = pObject;
		m_pFunc = f;
	}

	int CallFunction(ValueT fValue)
	{
		int nRetCode = -1;

		if (m_pObj && m_pFunc)
		{
			nRetCode = (m_pObj->*m_pFunc)(m_pParam, fValue);
		}

		return nRetCode;
	}

private:
	T*	   m_pObj;
	pfun   m_pFunc;
};

template<class TEaseTween, class ValueT = float>
class ETSimpleAnimateData
{
public:
	ETSimpleAnimateData()
	{
		Clear();
	}

	void Clear()
	{
		m_dwStartTick		= 0;
		m_fTotalTime	= 0;
		m_fStartValue	= 0;
		m_fChangeValue  = 0;
	}

	void Start(ValueT fTotalTime, ValueT fStartValue, ValueT fChangeValue)
	{
		m_fStartValue  = fStartValue;
		m_fTotalTime   = fTotalTime;
		m_fChangeValue = fChangeValue;
		m_dwStartTick = ::GetTickCount();
	}

	void SetParam(void* pParam)
	{
		m_pParam = pParam;
	}

	void* GetParam() const
	{
		return m_pParam;
	}

	ValueT Caculate() const
	{
		if(IsReachEnd()) 
		{
			return m_fStartValue + m_fChangeValue;
		}

		return TEaseTween::calculate(GetDuration(), m_fStartValue, m_fChangeValue, m_fTotalTime);
	}

	ValueT Caculate(ValueT fStartValue, ValueT fChangeValue) const
	{
		return TEaseTween::calculate(GetDuration(), fStartValue, fChangeValue, m_fTotalTime);
	}

	ValueT GetDuration() const
	{
		if(m_dwStartTick == 0) {
			return 0;
		}

		ValueT fDuration = ::GetTickCount() - m_dwStartTick;
		if (fDuration >= m_fTotalTime) {
			return m_fTotalTime;
		}

		return fDuration;
	}

	bool IsReachEnd() const
	{
		return m_dwStartTick == 0 || GetDuration() >= m_fTotalTime;
	}

	ValueT GetChangeValue() const
	{
		return m_fChangeValue;
	}

	ValueT GetStartValue() const
	{
		return m_fStartValue;
	}

	ValueT GetTotalTime() const
	{
		return m_fTotalTime;
	}

protected:
	DWORD m_dwStartTick;
	ValueT  m_fTotalTime;
	ValueT  m_fStartValue;
	ValueT  m_fChangeValue;
	void*  m_pParam;
};

}//end ease_tween 
///////////////////////////////////////////////////////////////////////////
//
#endif // __animate_data_def__h__