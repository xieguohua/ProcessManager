/*************************************************
//  File name:       BkRef.h
//  Date:              2020/11/10
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/10
//   <version>     1.0    
*************************************************/
#ifndef _BkRef_h_
#define _BkRef_h_

class BkRefImpl {
public:


	BkRefImpl(): refCount(1){
	}

	virtual ~BkRefImpl() 
	{
	}

	void AddRef(){
		::InterlockedIncrement(&refCount);
	}

	void Release () {
		if(InterlockedDecrement(&refCount) == 0) {
			delete this;
		}
	}
protected:
	void operator= (const BkRefImpl& other){}
private:
	volatile long refCount;
};

template<class T>
class BkSafePtr {
public:
	static BkSafePtr<T> createObject() {
		return BkSafePtr<T>(new T(), false);
	}
public:

	BkSafePtr() {
		this->pObj = NULL;
	}

	BkSafePtr(T* pObj, bool addRef) {
		this->pObj = pObj;
		if (pObj && addRef) {
			pObj->AddRef();
		}
	}

	BkSafePtr(T* pObj) {
		this->pObj = pObj;
		if (pObj) {
			pObj->AddRef();
		}
	}

	BkSafePtr(const BkSafePtr& other) {
		this->pObj = NULL;
		*this = other;
	}

	~BkSafePtr() {
		this->Release();
	}

	void Release() {
		if (this->pObj) {
			this->pObj->Release();
			this->pObj = NULL;
		}
	}

	T* Detach() {
		T* pObj_ = this->pObj;
		this->pObj = NULL;
		return pObj_;
	}

	void operator = (const T* pObj_) {
		if (this->pObj == pObj_) {
			return;
		}

		this->Release();
		if(pObj_) {
			((T*)pObj_)->AddRef();
			this->pObj = (T*)pObj_;
		}
	}

	void operator = (const BkSafePtr& other) {
		if (this == &other) {
			return;
		}

		if(this->pObj == other.pObj) {
			return;
		}

		this->Release();
		this->pObj = other.pObj;
		if(this->pObj) {
			this->pObj->AddRef();
		}
	}

	bool operator == (const BkSafePtr& other) const {
		return this->pObj == other.pObj;
	}

	bool operator != (const BkSafePtr& other) const {
		return this->pObj != other.pObj;
	}

	bool operator == (T* pObj_) const {
		return this->pObj == pObj_;
	}

	bool operator < (const BkSafePtr& other) const {
		return pObj < other.pObj;
	}

	bool operator <= (const BkSafePtr& other) const {
		return pObj < other.pObj;
	}

	bool operator > (const BkSafePtr& other) const {
		return pObj > other.pObj;
	}

	bool operator >= (const BkSafePtr& other) const {
		return pObj >= other.pObj;
	}


	T* Ptr() {
		return pObj;
	}

	const T* Ptr() const {
		return pObj;
	}

	operator T*() {
		return pObj;
	}

	operator const T*() const {
		return pObj;
	}

	T* operator ->() {
		return pObj;
	}

	const T* operator ->() const {
		return pObj;
	}

	operator bool() const {
		return pObj !=NULL;
	}

private:
	T* pObj;
};


template<class T>
class BkSharePtr 
{
public:
	static BkSharePtr<T> createObject() {
		return BkSharePtr(new T());
	}

	static BkSharePtr<T> createObject(const T& value) {
		return BkSharePtr(new T(value));
	}

	BkSharePtr() {
		m_pObj = NULL;
		m_pRefCount = NULL;
	}

	explicit BkSharePtr(T* pObj) {
		m_pObj = pObj;
		m_pRefCount = new LONG(1);
	}

	BkSharePtr(const BkSharePtr<T>& other) : m_pRefCount(NULL), m_pObj(NULL)
	{
		*this = other;
	}

	~BkSharePtr() {
		Release();
	}

	void Release() 
	{
		if(!m_pRefCount) {
			return;
		}

		if(InterlockedDecrement(m_pRefCount) != 0)
		{
			m_pObj = NULL;
			m_pRefCount = NULL;
			return;
		}

		delete m_pRefCount;
		m_pRefCount = NULL;
		if(m_pObj) {
			delete m_pObj;
			m_pObj = NULL;
		}
	}

	void operator=(const BkSharePtr<T>& other)
	{
		if(other.m_pObj == m_pObj) {
			return;
		}

		Release();
		if (other.m_pRefCount && *other.m_pRefCount > 0) {
			m_pRefCount = other.m_pRefCount;
			::InterlockedIncrement(m_pRefCount);
			m_pObj = other.m_pObj;
		}
	}

	T& DataRef() {
		return *m_pObj;
	}

	const T& DataRef() const{
		return *m_pObj;
	}

	T* Ptr() {
		return m_pObj;
	}

	const T* Ptr() const {
		return m_pObj;
	}

	operator T*() {
		return m_pObj;
	}

	operator const T*() const {
		return m_pObj;
	}

	T* operator ->() {
		return m_pObj;
	}

	const T* operator ->() const {
		return m_pObj;
	}

	operator bool() const {
		return m_pObj !=NULL;
	}

	bool operator == (const BkSharePtr& other) const {
		return this->m_pObj == other.m_pObj;
	}

	bool operator != (const BkSharePtr& other) const {
		return this->m_pObj != other.m_pObj;
	}

	bool operator == (T* pObj_) const {
		return this->m_pObj == pObj_;
	}

	bool operator < (const BkSharePtr& other) const {
		return m_pObj < other.m_pObj;
	}

	bool operator <= (const BkSharePtr& other) const {
		return m_pObj < other.m_pObj;
	}

	bool operator > (const BkSharePtr& other) const {
		return m_pObj > other.m_pObj;
	}

	bool operator >= (const BkSharePtr& other) const {
		return m_pObj >= other.m_pObj;
	}

protected:
	void operator=(const T* pObj) {}


protected:
	T* m_pObj;
	LONG volatile* m_pRefCount;
};

#endif