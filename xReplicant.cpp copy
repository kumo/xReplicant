// xReplicant: implementation of a base class for replicants to support
// transparency and responding to user movement or workspace changing.
//
// todo:
//   .  if the first replicant is not removed last then tracker crashes...
//   .  option for deciding on which workspaces a replicant is visible 

#ifndef __xREPLICANT_H__
#include "xReplicant.h"
#endif

#define ARGB_FORMAT B_RGBA32_BIG
struct ARGBPixel { uint8 b,g,r,a; };

int32
wait_for_screen_shot(void *data);

// *******************************
// * xReplicant()
// * default constructor
// *******************************
xReplicant::xReplicant(BRect bounds, const char* name)
	: BView(bounds, name, B_FOLLOW_NONE, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
	actual_bounds(bounds)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// we are currently not replicated
	fReplicated = false;

	// a BDragger is placed in bottom right corner so that the view is replicable
	AddDragger();
}

// *******************************
// * xReplicant()
// * replicant constructor
// *******************************
xReplicant::xReplicant(BMessage *data)
	: BView(data)
{
	SetViewColor(B_TRANSPARENT_32_BIT);

	fReplicated = true;
	background = NULL;
		
	// what are our bounds?
	data->FindRect("bounds", &actual_bounds);

	// let's remove the dragger because it might not be working
	for (int i=0; i < CountChildren(); i++) {
		if (strcmp(ChildAt(i)->Name(), "_dragger_") == 0) {
			RemoveChild(ChildAt(i));
		}
	}

	// and lets specifically add one
	AddDragger();

}

// *******************************
// * ~xReplicant()
// * destructor.
// *******************************
xReplicant::~xReplicant() {
	if (fReplicated) {
		// delete all our children as well
		for (int i=0; i < CountChildren(); i++) {
			RemoveChild(ChildAt(i));
		}
		
		status_t status;
		fShouldQuit = true;
		wait_for_thread(heartbeat, &status);

		if (background)
			delete background;
	}
}

// *******************************
// * AttachedToWindow()
// * when the replicant is attached to the window it is still hidden so
// * the background can be captured.
// *******************************
void
xReplicant::AttachedToWindow()
{
	if (fReplicated) {
		BRect init;
	
		// this is our proper size so we can check if it has changed later
		actual_bounds = Bounds();

		// what are our screen coordinates
		init = actual_bounds;
		init = ConvertToScreen(init);
		location.x = init.left;
		location.y = init.top;

		// take a picture of where we are
	UpdateBackgroundTransparency();
		//BRect rect(actual_bounds);
		//rect.OffsetTo(location);
		//screen.GetBitmap(&background, false, &rect);
		//fHaveBackgroundImage = false;

		// add the message filter (locking and unlocking the looper)
		Parent()->Looper()->Lock();
		fMessageFilter = new xMessageFilter(B_WORKSPACE_ACTIVATED, this);
		Parent()->Looper()->AddFilter(fMessageFilter);
//		printf("there are %ld filters\n", Parent()->Looper()->FilterList()->CountItems());
		Parent()->Looper()->Unlock();

		max_val = 0; ben_val = 0;
		bitmap_sem = create_sem(0, "bitmap_sem");

		fShouldQuit = false;
		fTakingScreenShot = false;
		
		fWorkspace = current_workspace(); 
	}
}

void
xReplicant::DetachedFromWindow()
{
	// if we are replicated remove the message filter
	if (Replicated()) {
		Parent()->Looper()->Lock();
		Parent()->Looper()->RemoveFilter(fMessageFilter);
		Parent()->Looper()->Unlock();
	} 
}

#pragma mark -

// archive the monster replicant
status_t
xReplicant::Archive(BMessage *data, bool deep) const
{
	BView::Archive(data, deep);
	
//	data->AddBool("be:unload_on_delete", true);
	data->AddBool("be:load_each_time", true);
	data->AddRect("bounds", actual_bounds);
	
	return B_OK;
}

#pragma mark -

// *******************************
// * Draw()
// *******************************
void
xReplicant::Draw(BRect updateRect)
{
	MovePenTo(0,0);
//	SetDrawingMode(B_OP_COPY);
	if (fReplicated)
		if (fHaveBackgroundImage)
			DrawBitmap(background);
		else
			FillRect(actual_bounds);
}

// *******************************
// * MessageReceived()
// *******************************
void
xReplicant::MessageReceived(BMessage *msg) {
	switch (msg->what) {
		case M_SCREENSHOT_STARTING:
			TakeScreenShot();
			break;
		default:
			BView::MessageReceived(msg);
	}
}

// *******************************
// * WorkspaceChanged()
// *******************************
void
xReplicant::WorkspaceChanged(int32 workspace)
{
	fWorkspace = workspace;
	UpdateBackgroundTransparency();
}

#pragma mark -

// *******************************
// * FrameMoved()
// * to check if we moved or the user moved us
// *******************************
void
xReplicant::FrameMoved(BPoint parentPoint) {
	// make sure we are not hidden (but why?!)
	if (!IsHidden()) {
		// get our current position.
		BPoint pos = Window()->ConvertToScreen(parentPoint);

		// if the position is different to what we think then update!
		if (pos.x != location.x || pos.y != location.y) {
			location.x = pos.x;
			location.y = pos.y;
			UpdateBackgroundTransparency();
		}
	}
}

// *******************************
// * xFrameResized()
// *******************************
void
xReplicant::xResizeTo(float width, float height) {
	ResizeTo(width, height);
	fDragger->MoveTo(width-7, height-7);
	
	actual_bounds = Bounds();
	UpdateBackgroundTransparency();
}

// *******************************
// * MouseDown()
// * the user clicks and we refresh
// *******************************
void
xReplicant::MouseDown(BPoint point) {
	UpdateBackgroundTransparency();	
}

#pragma mark -

// *******************************
// * UpdateBackgroundTransparency()
// * checks the current screen to see if a screen shot is needed or just the colour
// * spawns a thread to take a screen shot of the hidden window
// *******************************
void
xReplicant::UpdateBackgroundTransparency()
{
	// we do not want to try to access the "old" bitmap between deleting and getting
	fHaveBackgroundImage = false;
	
	bool result = screen.HasBackgroundImage();
	
	if (result)
		PoseForScreenShot();
	else
		SetHighColor(screen.DesktopColor());
}

void
xReplicant::PoseForScreenShot()
{
//	// printf("PoseForScreenShot().start\n");
	thread_id timethread;

	if (!fTakingScreenShot) {
		int32 previous = atomic_add(&ben_val, 1);
		if (previous >= 1)
			if (acquire_sem(bitmap_sem) != B_NO_ERROR)
				goto get_out;
				
		fTakingScreenShot = true;

		Hide();

		// we have to spawn a thread because the calls to Hide() and Show()
		// are buffered and the app_server will not hide us
		timethread = spawn_thread(wait_for_screen_shot, "posing_for_screenshot", 10, this);
		resume_thread(timethread);
	}
	// printf("PoseForScreenShot().end\n");
get_out:
	printf(""); // nasty fudge
}		

// *******************************
// * TakeScreenShot()
// * replicant should be hidden, so we can now take a screen shot
// *******************************
void
xReplicant::TakeScreenShot(bool should_show=true)
{
	// delete the old background
	if (background)
		delete background;

	// set up the screen coordinates
	BScreen screen;
	BRect rect(actual_bounds);
	rect.OffsetTo(location);

	// and take the screen shot
	screen.GetBitmap(&background, false, &rect);
	
	// now we can show ourselves
	if (should_show)
		Show();
	
	// and declare that we have a background image
	fHaveBackgroundImage = true;

	fTakingScreenShot = false;

	int32 previous = atomic_add(&ben_val, -1);
	if (previous > 1)
		release_sem(bitmap_sem);
}

#pragma mark -

// *******************************
// * AddDragger()
// * it appears that the archive does not always
// * fix the dragger, so we do it ourselves.
// *******************************
void
xReplicant::AddDragger()
{
	BRect frame(actual_bounds);

	// place it bottom right, but not the full view
	frame.OffsetTo(0,0);
	frame.left = frame.right - 7;
	frame.top = frame.bottom - 7;
		
	// give ourselves a new dragger
	fDragger = new BDragger(frame, this, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	AddChild(fDragger);
}

// *******************************
// * wait_for_screen_shot()
// *******************************
int32
wait_for_screen_shot(void *data) {
	// printf("wait_for_screen_shot().start\n");
	xReplicant *neko = (xReplicant*) data;
	
	snooze(40000);
	BMessage *msg = new BMessage(M_SCREENSHOT_STARTING);
	neko->Window()->PostMessage(msg, neko);
	
	// printf("wait_for_screen_shot().end\n");
	return 0;
}
// ------------------------------------------------------------- xReplicant.cpp --