#ifndef __xMESSAGEFILTER_H__
#include "xMessageFilter.h"
#endif

#ifndef __xREPLICANT_H__
#include "xReplicant.h"
#endif

xMessageFilter::xMessageFilter(uint32 command, xReplicant *parent)
	: BMessageFilter(command),
	fParent(parent)
{

}

filter_result
xMessageFilter::Filter(BMessage *message, BHandler **target)
{
	filter_result 	result = B_DISPATCH_MESSAGE;
	int32			workspace;
	
	fParent->xBounds().PrintToStream();
	message->PrintToStream();
	workspace = message->FindInt32("workspace");
	
	fParent->WorkspaceChanged(workspace);
	
	
	return result;
}
