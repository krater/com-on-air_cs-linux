#if !defined(PACKETPLAYER_H)
#define PACKETPLAYER_H

#include "dectshark.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <time.h>
#include <limits.h>
#include <signal.h>
#include <ctype.h>
#include <linux/soundcard.h>

#include "codec/g72x.h"

#define PKT_OFF_H              0x05
#define PKT_OFF_B_FIELD        0x07

#define DECT_H_BA_MASK         0x0e
#define DECT_H_BA_NO_B_FIELD   0x0e
#define DECT_H_BA_HALF_SLOT    0x08
#define DECT_H_BA_DOUBLE_SLOT  0x04

enum packettype
{
	PTYPE_FP,
	PTYPE_PP,
	PTYPE_BOTH
};

class packetplayer
{
public:
        packetplayer();
        ~packetplayer();

        void setslot(int slot);
	void setpackettype(packettype type);

        void playpacket(sniffed_packet packet);
	
protected:
        int audio;
        int slot;
	packettype ptype;

        struct g72x_state state;
};




#endif
