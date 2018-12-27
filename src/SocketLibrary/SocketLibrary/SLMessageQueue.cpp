#include ".\slmessagequeue.h"
namespace SL
{

CSLMessageQueueNode::CSLMessageQueueNode()
{
	attachID	= 0;
	attachInfo  = NULL;
	next		= NULL;
};

CSLMessageQueueNode::~CSLMessageQueueNode()
{
};

};
