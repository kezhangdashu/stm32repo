#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "main.h"

#define MAX_SYS_MSG_FIFO_DEPTH 3

typedef enum _SYS_MSG_FIFO_INDEX
{
	SYS_MSG_FIFO_DEV_CTRL,
	SYS_MSG_FIFO_GUI,

	SYS_MSG_FIFO_COUNT			//message fifo count

} SYS_MSG_FIFO_INDEX;

//
//define key event.
//
typedef enum _SYS_MESSAGE
{
	MSG_NONE = 0x00,
	KEY_MSG_POWS,
	KEY_MSG_POWL,
	KEY_MSG_POWE,
	KEY_MSG_F1,
	KEY_MSG_F2,
	KEY_MSG_F3,
	DP_MSG_QCD,
	WF_MSG_USI,
  CD_MSG_INIT,
	CD_MSG_WR,
	DM_MSG_UP,
} SYS_MESSAGE;


// message module initial
void message_init (void);

// send message process
u8 message_send (u8 msg_fifo_index, u8 event);

// get message process
u8 message_get(u8 msg_fifo_index);


#endif
