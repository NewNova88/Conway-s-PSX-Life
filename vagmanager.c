#include "vagmanager.h"
#include <stdio.h>

SpuCommonAttr l_c_attr;                   // structure for changing common voice attributes
SpuVoiceAttr g_s_attr;                    // structure for changing individual voice attributes

unsigned long l_vag1_spu_addr;            // address in memory for first sound file
unsigned long l_size;                     // number of bytes transferred within RAM

long g_master_volume;                     // master volume for both sound sources
long l_top;

short g_master_pan;                       // master pan
short g_leftright_pan;                    // pan value for left / right pan
short g_frontback_pan;                    // pan value for front / back pan
short g_increment;                        // increment value for pan control
short g_vol_increment;                    // increment value for master volume control

char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

void InitMusicSystem()
{
	SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);

	l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
	l_c_attr.mvol.left  = 0x3fff; // set master left volume
	l_c_attr.mvol.right = 0x3fff; // set master right volume
	
    SpuSetCommonAttr (&l_c_attr);
}


void SendVAGToRAM(DWORD VAG_data_size)
{
	SpuSetTransferMode (SpuTransByDMA); // set transfer mode to DMA
	l_vag1_spu_addr = SpuMalloc(VAG_data_size); // allocate SPU memory for sound 1

	l_top = SpuSetTransferStartAddr (l_vag1_spu_addr); // set transfer starting address to malloced area

   
	l_size = SpuWrite (VAGaddr + sizeof(VAGhdr_t), VAG_data_size); // perform actual transfer
	SpuIsTransferCompleted (SPU_TRANSFER_WAIT); // wait for DMA to complete
}


void SetVoiceAttr(DWORD pitch)
{
  // mask which specific voice attributes are to be set
  g_s_attr.mask =
    (
      SPU_VOICE_VOLL |
	  SPU_VOICE_VOLR |
	  SPU_VOICE_PITCH |
	  SPU_VOICE_WDSA |
	  SPU_VOICE_ADSR_AMODE |
	  SPU_VOICE_ADSR_SMODE |
	  SPU_VOICE_ADSR_RMODE |
	  SPU_VOICE_ADSR_AR |
	  SPU_VOICE_ADSR_DR |
	  SPU_VOICE_ADSR_SR |
	  SPU_VOICE_ADSR_RR |
	  SPU_VOICE_ADSR_SL
	);

	g_s_attr.voice = (SPU_0CH);
	
	g_s_attr.volume.left  = 0x1fff;
	g_s_attr.volume.right = 0x1fff;

	g_s_attr.pitch        = pitch;
	g_s_attr.addr         = l_vag1_spu_addr;
	g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	g_s_attr.ar           = 0x0;
	g_s_attr.dr           = 0x0;
	g_s_attr.sr           = 0x0;
	g_s_attr.rr           = 0x0;
	g_s_attr.sl           = 0xf;

	SpuSetVoiceAttr (&g_s_attr);
}


void PlaySFX(void)
{
	SpuSetKey(SpuOn,SPU_0CH);
}