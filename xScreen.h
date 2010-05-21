// xScreen.h: definition of a nicer screen class
// but note this. i cannot know about the current workspace so
// an external class tells me my current one. ah but i can "current_workspace"

#include <Screen.h>
#include <Path.h>
#include <Node.h>
#include <Message.h>
#include "fs_attr.h"
#include <Background.h>
#include "InterfaceDefs.h"
#include <FindDirectory.h>
#include "stdlib.h"
#include "stdio.h"

class xScreen : public BScreen {
public:
	xScreen();
	
	bool		HasBackgroundImage() const { return _backgrounds[current_workspace()]; };
private:
	bool		_backgrounds[32];
};