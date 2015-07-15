#include "packetparser.h"

#define DECT_A_TA		0xe0
#define DECT_A_Q1		0x10
#define DECT_A_BA		0x0e
#define DECT_A_Q2		0x01

#define DECT_Q_TYPE		0x80
#define DECT_N_TYPE		0x60
#define DECT_P_TYPE		0xe0


packetparser::packetparser()
{
   resetinfo();
}

packetparser::~packetparser()
{
}

void packetparser::resetinfo()
{
   int i;

   for(i=0;i<24;i++)
   {
      syncinfo.slot[i].channel=0;
      syncinfo.slot[i].afields=0;
      syncinfo.slot[i].bfields=0;
      syncinfo.slot[i].berrors=0;
      syncinfo.slot[i].lastrssi=0;
   }
   
   infostr[0]=0; 

}
void packetparser::parsepacket(sniffed_packet packet)
{
	unsigned int slot=packet.slot;
	if(slot<24)
	{
		syncinfo.slot[slot].afields++;

		if(bfieldactive(packet))
		{
			syncinfo.slot[slot].bfields++;

			if(!bfieldok(packet))
				syncinfo.slot[slot].berrors++;
		}

		syncinfo.slot[slot].channel=packet.channel;
		syncinfo.slot[slot].lastrssi=packet.rssi;

		processrfpi(packet);
      process_ssi(packet);

	}
}
	
slotinfo_str packetparser::getslotinfo(unsigned int slot)
{
	return syncinfo.slot[slot];
}

int packetparser::bfieldactive(sniffed_packet packet)
{
	if ((packet.data[5] & 0x0e) != 0x0e)
		return 1;
	return 0;
}

int packetparser::bfieldok(sniffed_packet packet)
{
   return packet.bfok;
}

void packetparser::processrfpi(sniffed_packet packet)
{
/*
	if ((packet.data[5] & DECT_A_TA) == DECT_N_TYPE)
		return 1;

	return 0;*/
}

void packetparser::process_ssi(sniffed_packet packet)
{
/*   if ((packet.data[0x17]!=0xe9)||(packet.data[0x18]!=0x8a))
      return;
*/
   if ((packet.data[5] & DECT_A_TA) != DECT_Q_TYPE)
      return;

   if ((packet.data[6] & 0xf0 ) != 0x30) //fixed part capabilities
      return;

   unsigned char b9=packet.data[9];
   sprintf(infostr,"g.726:%i, GAP:%u, DSAA:%u, DSC:%u",((b9&0x80)>>7), ((b9&0x40)>>6), ((b9&0x08)>>3), ((b9&0x04)>>2)); 
}
