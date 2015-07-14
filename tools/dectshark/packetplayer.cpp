#include "packetplayer.h"


packetplayer::packetplayer()
{
    audio = open("/dev/dsp", O_WRONLY);
    if (audio < 0)
    {
            printf("couldn't open audio driver(/dev/dsp)");
            audio=NULL;
    }

    // 16 bit, 8khz samplerate, mono
    int bps=16,channels=1,samplerate=8000;
    if (ioctl(audio, SOUND_PCM_WRITE_BITS, &bps) == -1)             printf("couldn't send SOUND_PCM_WRITE_BITS to /dev/dsp\n");
    if (ioctl(audio, SOUND_PCM_WRITE_CHANNELS, &channels) == -1)    printf("couldn't send SOUND_PCM_WRITE_CHANNELS to /dev/dsp\n");
    if (ioctl(audio, SOUND_PCM_WRITE_RATE, &samplerate) == -1)      printf("couldn't send SOUND_PCM_WRITE_RATE to /dev/dsp\n");

}

packetplayer::~packetplayer()
{
        if(audio) close(audio);
}

void packetplayer::setslot(int slot)
{
    if((slot>=0)&&(slot<24))
        this->slot=slot;
}

void packetplayer::setpackettype(packettype type)
{
    ptype=type;
}

void packetplayer::playpacket(sniffed_packet packet)
{
    if(packet.slot!=slot)
        return;

//    if(packet.

//    if(packet.channel!=channel)
//        return;

    if((packet.data[PKT_OFF_H] & DECT_H_BA_MASK) == DECT_H_BA_NO_B_FIELD)
        return;

    if ((packet.data[PKT_OFF_H] & DECT_H_BA_MASK) == DECT_H_BA_HALF_SLOT)
        printf("half slot");

    if ((packet.data[PKT_OFF_H] & DECT_H_BA_MASK) == DECT_H_BA_DOUBLE_SLOT)
        printf("double slot");


    unsigned char* p = packet.data+PKT_OFF_B_FIELD+6;

    // Each full slot packet has 40 useful bytes.
    for (int i = 0; i<40; i++)
    {
        short sample;

        // Processing the high nibble
        unsigned char code = *p>>4;
        sample = (short) g721_decoder(code, AUDIO_ENCODING_LINEAR, &state);
        if(audio) write(audio,&sample,2);

        // Processing the low nibble
        code = *p&0x0f;
        sample = (short) g721_decoder(code, AUDIO_ENCODING_LINEAR, &state);
        if(audio) write(audio,&sample,2);

        p++;

    }
}
