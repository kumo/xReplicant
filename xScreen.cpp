// xScreen.cpp: implementation of a nicer screen class

#ifndef __xSCREEN_H__
#include "xScreen.h"
#endif

xScreen::xScreen()
	: BScreen()
{
	BPath		path;
	BNode 		node;
	BMessage 	msg;
	char 		*data = NULL;
	status_t 	status;
	attr_info 	info;
	int32 		data_read;

	// for now there are no backgrounds
	for (int i=0; i<32; i++)
		_backgrounds[i] = false;

	// find the desktop directory and get a node to it
	find_directory(B_DESKTOP_DIRECTORY, &path);
	node.SetTo(path.Path());
	
	// now lets see if the background attribute exists
	status = node.GetAttrInfo(B_BACKGROUND_INFO, &info);
	if (status == B_OK)
		data = (char*)malloc(info.size);
	else
		data = NULL;
		
	if (data != NULL) {
		data_read = node.ReadAttr(B_BACKGROUND_INFO, B_MESSAGE_TYPE, 0, data, info.size);
		if (data_read > 0) {
			msg.Unflatten(data);
			status = B_OK;
		} else 
			status = B_ERROR;
	} else
		status = B_ERROR;
	
	// we have the message but now what?
	if (status != B_ERROR) {
		int32 index=0;
		int32 value;

//		msg.PrintToStream();
		while (msg.FindInt32(B_BACKGROUND_WORKSPACES, index++, &value) != B_BAD_INDEX) {
//			printf("value: %ld\n", value);
		
			for (int i=0; i<32; i++) {
				if (value & (1 << i)) {
//					printf("[%ld]: true\n", i);
					_backgrounds[i] = true;	
				}
			}
		}
	}
}