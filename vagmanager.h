#ifndef _VAGMANAGER_H_
#define _VAGMANAGER_H_
#include <libspu.h>



#define MALLOC_MAX 3

#define SWAP_ENDIAN32(x) (((x)>>24) | (((x)>>8) & 0xFF00) | (((x)<<8) & 0x00FF0000) | ((x)<<24))

typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    DWORD;

extern BYTE VAGaddr[];

typedef struct VAGhdr_s{		// All the values in this header must be big endian
        char id[4];			// VAGp
        DWORD version;              
        DWORD reserved;
        DWORD dataSize;
        DWORD samplingFrequency;
        char  reserved2[12];
        char  name[16];
}VAGhdr_t;                     


void SpuSetVoiceAttr(SpuVoiceAttr *p_attr);
void SpuSetKey(long p_onoff, unsigned long p_voice_bit);
void SendVAGToRAM(DWORD VAG_data_size);
void SetVoiceAttr(DWORD pitch);
void PlaySFX(void);
void InitMusicSystem();

#endif
