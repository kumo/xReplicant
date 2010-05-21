// xMessageFilter.h: definition of a custom messagefilter class
#ifndef __xMESSAGEFILTER_H__
#define __xMESSAGEFILTER_H__

#include <MessageFilter.h>

class xReplicant;
class xMessageFilter : public BMessageFilter
{
public:
							xMessageFilter(uint32 command, xReplicant *parent);
protected:
	virtual filter_result	Filter(BMessage *message, BHandler **target);
private:
	xReplicant				*fParent;
};

#endif