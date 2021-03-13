/*************************************************
//  File name:       IBkDialogWndConext.h
//  Date:              2020/12/14
//  <author>      hexujun@cmcm.com
//  <time>          2020/12/14
//   <version>     1.0    
*************************************************/
#ifndef _BKWIN_IBkDialogWndConext_h_
#define _BKWIN_IBkDialogWndConext_h_

#include "IBkRunnableContext.h"
#include "bkedit/IBkTimer.h"

namespace BkWin {\
class IBkDialogWndContext
{
public:
	virtual BkTimer::IBkWndTimer& GetBkTimer() = 0;
	virtual IBkRunnableContext& GetBkRunnableContext() = 0;
};
}
#endif