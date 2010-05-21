// xReplicant.h: definition of a transparent replicant class

#ifndef __xREPLICANT_H__
#define __xREPLICANT_H__

#include <View.h>
#include <Message.h>
#include <Dragger.h>
#include <Screen.h>
#include <Bitmap.h>
#include <String.h>
#include <Application.h>
#include "stdio.h"
#include <OS.h>
#include <Window.h>
#include <be/interface/InterfaceDefs.h>

#ifndef __xMESSAGEFILTER_H__ 
#include "xMessageFilter.h"
#endif

#ifndef __xSCREEN_H__
#include "xScreen.h"
#endif

#define M_SCREENSHOT_STARTING '_scr'
#define M_WORKSPACE_CHANGED '_wks'
#define M_HELLO_THERE '__hi'
#define M_BYE_BYE '_bye'

// this should be exported by the derived class
// class _EXPORT xReplicant;

class xReplicant : public BView
{
public:
						xReplicant(BRect bounds, const char* name);
						xReplicant(BMessage *data);
						~xReplicant();
	
	void				PoseForScreenShot();
	void				TakeScreenShot(bool should_show=true);
	void				UpdateBackgroundTransparency();

	void				WorkspaceChanged(int32 workspace);

	bool				ShouldQuit() const { return fShouldQuit; };
	bool				TakingScreenShot() const { return fTakingScreenShot; };

	virtual void		xResizeTo(float width, float height);

	sem_id				bitmap_sem;
	uint32				max_val;
	int32				ben_val;
	
	inline bool			Replicated() const { return fReplicated; };

	BRect				xBounds() const { return actual_bounds; };
protected:
	virtual void		AttachedToWindow();
	virtual void		DetachedFromWindow();

	virtual void 		MessageReceived(BMessage *msg);
		
	static BArchivable* Instantiate(BMessage *data);
	virtual status_t 	Archive(BMessage *data, bool deep = true) const;
	
	virtual void		Draw(BRect updateRect);
//	virtual void 		Pulse();
	virtual void		FrameMoved(BPoint parentPoint);
//	virtual void		MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	virtual void		MouseDown(BPoint point);
	
	void				AddDragger();
private:
	bool				fReplicated;
	BRect				actual_bounds;
	BPoint				location;
	int					fFriends;
	BView				*desktop;
	bool				fHaveBackgroundImage;
	BBitmap				*background;
	bool				fShouldQuit;
	thread_id			heartbeat;
	bool				fTakingScreenShot;
	BDragger			*fDragger;
	
	xMessageFilter		*fMessageFilter;
	xScreen				screen;
	
	int					fWorkspace;
};
#endif
