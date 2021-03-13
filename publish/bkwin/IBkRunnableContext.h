/*************************************************
//  File name:       IBkRunnableContext.h
//  Date:              2020/12/14
//  <author>      hexujun@cmcm.com
//  <time>          2020/12/14
//   <version>     1.0    
*************************************************/
#ifndef _BKWIN_IBkRunnableContext_h_
#define _BKWIN_IBkRunnableContext_h_

#include "BkRunnable.h"

namespace BkWin {\

class IBkRunnableContext 
{
public:
	/**
	 * 发送Runnable：可以在其他线程 发生runnable到界面线程执行
	 * 参考 BkRunnable.h文件的：BkWin::FuncFactory创建runnable方法。
	 */
	virtual BOOL SendRunnable(BkSafePtr<IRunnable>& runnable) = 0;

	/*
	* PostRunnable: 不等待结束， 可以在其他线程 发生runnable到界面线程执行
	* LPVOID lpTarget 附加参数，可以通过改参数cancel runnable
	* uDelayTime 表是延时执行
	* return runnable unique id, 利用改id可以取消
	*/
	virtual DWORD  PostRunnable(BkSafePtr<IRunnable>& runnable, LPVOID lpTarget = NULL, UINT uDelayTime = 0) = 0;

	/**
	 * 取消还没有执行的ruannble
	 */
	virtual void CancelRunnableByID(DWORD dwRunnbaleID) = 0;
	virtual void CancelTargetRunnables(LPVOID lpszTarget) = 0;
};
}
#endif