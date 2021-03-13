/*************************************************
//  File name:       IBkWindow.h
//  Date:              2021/01/20
//  <author>      hexujun@cmcm.com
//  <time>          2021/01/20
//   <version>     1.0    
*************************************************/
#ifndef _BkWin_IBkWindow_h_
#define _BkWin_IBkWindow_h_

namespace BkWin {\

class IBkWindow 
{
public:
	virtual void InvalidatePosition() = 0;
	virtual long ScaleByDpi(long lSize) = 0;
	virtual long UnScaleByDpi(long lSize) = 0;
};

}

#endif