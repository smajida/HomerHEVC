/*****************************************************************************
 * hmr_binary_encoding.c : homerHEVC encoding library
/*****************************************************************************
 * Copyright (C) 2014 homerHEVC project
 *
 * Juan Casal <jcasal.homer@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/
/*
 * some of the work below is derived from HM HEVC reference software where 
 * the following license applies
 */
/****************************************************************************
/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.  
 *
 * Copyright (c) 2010-2014, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *****************************************************************************/

#include <math.h>

#include "hmr_common.h"
#include "hmr_private.h"
#include "hmr_cabac_tables.h"

#define FAST_BIT_EST	1



uint be_bitcnt(bitstream_t *bs, binary_model_t* bm)
{
	return hmr_bitstream_bitcount(bs) + 8 * bm->m_numBufferedBytes + 23 - bm->m_bitsLeft;
}


void be_write(bitstream_t *bs, binary_model_t* bm)
{
	uint leadByte = bm->m_uiLow >> (24 - bm->m_bitsLeft);
	bm->m_bitsLeft += 8;
	bm->m_uiLow &= 0xffffffff >> bm->m_bitsLeft;

	if ( leadByte == 0xff )
	{
		bm->m_numBufferedBytes++;
	}
	else
	{
		if ( bm->m_numBufferedBytes > 0 )
		{
			uint carry = leadByte >> 8;
			uint byte = bm->m_bufferedByte + carry;
			bm->m_bufferedByte = leadByte & 0xff;
			hmr_bitstream_write_bits(bs,byte,8);

			byte = ( 0xff + carry ) & 0xff;
			while ( bm->m_numBufferedBytes > 1 )
			{
				hmr_bitstream_write_bits(bs,byte,8);
				bm->m_numBufferedBytes--;
			}
		}
		else
		{
			bm->m_numBufferedBytes = 1;
			bm->m_bufferedByte = leadByte;
		}      
	}    
}


void be_encode_bin_EP( enc_env_t* ee, uint binValue)
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += bm->m_binCountIncrement;
	bm->m_uiLow <<= 1;
	if( binValue )
	{
		bm->m_uiLow += bm->m_uiRange;
	}
	bm->m_bitsLeft--;

	if ( bm->m_bitsLeft < 12 )
	{
		be_write(ee->bs, bm);//writeout
	}
}

void be_encode_bins_EP( enc_env_t* ee, uint binValues, int numBins )
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += numBins & -bm->m_binCountIncrement;

	while ( numBins > 8 )
	{
		uint pattern;
		numBins -= 8;
		pattern = binValues >> numBins; 
		bm->m_uiLow <<= 8;
		bm->m_uiLow += bm->m_uiRange * pattern;
		binValues -= pattern << numBins;
		bm->m_bitsLeft -= 8;

//		testAndWriteOut();
		if ( bm->m_bitsLeft < 12 )
		{
			be_write(ee->bs, bm);//writeout
		}
	}

	bm->m_uiLow <<= numBins;
	bm->m_uiLow += bm->m_uiRange * binValues;
	bm->m_bitsLeft -= numBins;

	if ( bm->m_bitsLeft < 12 )
	{
		be_write(ee->bs, bm);//writeout
	}
}


static const byte g_bc_next_state_MPS[ 128 ] =
{
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
  34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
  50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
  66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
  82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
  98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
  114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 124, 125, 126, 127
};

static const byte g_bc_next_state_LPS[ 128 ] =
{
  1, 0, 0, 1, 2, 3, 4, 5, 4, 5, 8, 9, 8, 9, 10, 11,
  12, 13, 14, 15, 16, 17, 18, 19, 18, 19, 22, 23, 22, 23, 24, 25,
  26, 27, 26, 27, 30, 31, 30, 31, 32, 33, 32, 33, 36, 37, 36, 37,
  38, 39, 38, 39, 42, 43, 42, 43, 44, 45, 44, 45, 46, 47, 48, 49,
  48, 49, 50, 51, 52, 53, 52, 53, 54, 55, 54, 55, 56, 57, 58, 59,
  58, 59, 60, 61, 60, 61, 60, 61, 62, 63, 64, 65, 64, 65, 66, 67,
  66, 67, 66, 67, 68, 69, 68, 69, 70, 71, 70, 71, 70, 71, 72, 73,
  72, 73, 72, 73, 74, 75, 74, 75, 74, 75, 76, 77, 76, 77, 126, 127
};


void be_encode_bin( enc_env_t* ee, context_model_t *cm, uint binValue)
{
	binary_model_t* bm = ee->b_ctx;
	uint  LPS   = g_cabac_LPST_table[ (cm->state>>1) ][ ( bm->m_uiRange >> 6 ) & 3 ];
	bm->m_uiBinsCoded+=bm->m_binCountIncrement;
	cm->num_bins_coded = 1;
	bm->m_uiRange -= LPS;
	
	if(binValue != (cm->state&1))
	{
		int numBits = g_cabac_renorm_table[ LPS >> 3 ];
		bm->m_uiLow     = ( bm->m_uiLow + bm->m_uiRange ) << numBits;
		bm->m_uiRange   = LPS << numBits;
		cm->state = g_bc_next_state_LPS[cm->state];
    
		bm->m_bitsLeft -= numBits;	
	}
	else
	{
		cm->state = g_bc_next_state_MPS[cm->state];
		if ( bm->m_uiRange >= 256 )
		{
			return;
		}

		bm->m_uiLow <<= 1;
		bm->m_uiRange <<= 1;
		bm->m_bitsLeft--;
	}

	if ( bm->m_bitsLeft < 12 )
	{
		be_write(ee->bs, bm);
	}
}

void be_encode_bin_TRM( enc_env_t* ee, uint binValue)
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += bm->m_binCountIncrement;
	bm->m_uiRange -= 2;
	if( binValue )
	{
		bm->m_uiLow     = ( bm->m_uiLow + bm->m_uiRange ) << 7;
		bm->m_uiRange = 2 << 7;
		bm->m_bitsLeft -= 7;
	}
	else if ( bm->m_uiRange >= 256 )
	{
		return;
	}
	else
	{
		bm->m_uiLow   <<= 1;
		bm->m_uiRange <<= 1;
		bm->m_bitsLeft--;    
	}

	if ( bm->m_bitsLeft < 12 )
	{
		be_write(ee->bs, bm);
	}
}


void be_finish(enc_env_t* ee)
{	
	binary_model_t* bm = ee->b_ctx;

	if ( bm->m_uiLow >> ( 32 - bm->m_bitsLeft ) )
	{
		hmr_bitstream_write_bits(ee->bs,bm->m_bufferedByte + 1,8);

		while ( bm->m_numBufferedBytes > 1 )
		{
			hmr_bitstream_write_bits(ee->bs,0x00,8);

			bm->m_numBufferedBytes--;
		}
		bm->m_uiLow -= 1 << ( 32 - bm->m_bitsLeft );
	}
	else
	{
		if ( bm->m_numBufferedBytes > 0 )
		{
			hmr_bitstream_write_bits(ee->bs,bm->m_bufferedByte,8);
		}
		while ( bm->m_numBufferedBytes > 1 )
		{
			hmr_bitstream_write_bits(ee->bs,0xff,8);
			bm->m_numBufferedBytes--;
		}    
	}
	hmr_bitstream_write_bits(ee->bs,bm->m_uiLow >> 8, 24 - bm->m_bitsLeft);

}





//------------------------------counter -------------------------------------------------
const int g_bc_entropy_bits[128] =
{
#if FAST_BIT_EST
  0x07b23, 0x085f9, 0x074a0, 0x08cbc, 0x06ee4, 0x09354, 0x067f4, 0x09c1b, 0x060b0, 0x0a62a, 0x05a9c, 0x0af5b, 0x0548d, 0x0b955, 0x04f56, 0x0c2a9,
  0x04a87, 0x0cbf7, 0x045d6, 0x0d5c3, 0x04144, 0x0e01b, 0x03d88, 0x0e937, 0x039e0, 0x0f2cd, 0x03663, 0x0fc9e, 0x03347, 0x10600, 0x03050, 0x10f95,
  0x02d4d, 0x11a02, 0x02ad3, 0x12333, 0x0286e, 0x12cad, 0x02604, 0x136df, 0x02425, 0x13f48, 0x021f4, 0x149c4, 0x0203e, 0x1527b, 0x01e4d, 0x15d00,
  0x01c99, 0x166de, 0x01b18, 0x17017, 0x019a5, 0x17988, 0x01841, 0x18327, 0x016df, 0x18d50, 0x015d9, 0x19547, 0x0147c, 0x1a083, 0x0138e, 0x1a8a3,
  0x01251, 0x1b418, 0x01166, 0x1bd27, 0x01068, 0x1c77b, 0x00f7f, 0x1d18e, 0x00eda, 0x1d91a, 0x00e19, 0x1e254, 0x00d4f, 0x1ec9a, 0x00c90, 0x1f6e0,
  0x00c01, 0x1fef8, 0x00b5f, 0x208b1, 0x00ab6, 0x21362, 0x00a15, 0x21e46, 0x00988, 0x2285d, 0x00934, 0x22ea8, 0x008a8, 0x239b2, 0x0081d, 0x24577,
  0x007c9, 0x24ce6, 0x00763, 0x25663, 0x00710, 0x25e8f, 0x006a0, 0x26a26, 0x00672, 0x26f23, 0x005e8, 0x27ef8, 0x005ba, 0x284b5, 0x0055e, 0x29057,
  0x0050c, 0x29bab, 0x004c1, 0x2a674, 0x004a7, 0x2aa5e, 0x0046f, 0x2b32f, 0x0041f, 0x2c0ad, 0x003e7, 0x2ca8d, 0x003ba, 0x2d323, 0x0010c, 0x3bfbb
#else
  0x08000, 0x08000, 0x076da, 0x089a0, 0x06e92, 0x09340, 0x0670a, 0x09cdf, 0x06029, 0x0a67f, 0x059dd, 0x0b01f, 0x05413, 0x0b9bf, 0x04ebf, 0x0c35f,
  0x049d3, 0x0ccff, 0x04546, 0x0d69e, 0x0410d, 0x0e03e, 0x03d22, 0x0e9de, 0x0397d, 0x0f37e, 0x03619, 0x0fd1e, 0x032ee, 0x106be, 0x02ffa, 0x1105d,
  0x02d37, 0x119fd, 0x02aa2, 0x1239d, 0x02836, 0x12d3d, 0x025f2, 0x136dd, 0x023d1, 0x1407c, 0x021d2, 0x14a1c, 0x01ff2, 0x153bc, 0x01e2f, 0x15d5c,
  0x01c87, 0x166fc, 0x01af7, 0x1709b, 0x0197f, 0x17a3b, 0x0181d, 0x183db, 0x016d0, 0x18d7b, 0x01595, 0x1971b, 0x0146c, 0x1a0bb, 0x01354, 0x1aa5a,
  0x0124c, 0x1b3fa, 0x01153, 0x1bd9a, 0x01067, 0x1c73a, 0x00f89, 0x1d0da, 0x00eb7, 0x1da79, 0x00df0, 0x1e419, 0x00d34, 0x1edb9, 0x00c82, 0x1f759,
  0x00bda, 0x200f9, 0x00b3c, 0x20a99, 0x00aa5, 0x21438, 0x00a17, 0x21dd8, 0x00990, 0x22778, 0x00911, 0x23118, 0x00898, 0x23ab8, 0x00826, 0x24458,
  0x007ba, 0x24df7, 0x00753, 0x25797, 0x006f2, 0x26137, 0x00696, 0x26ad7, 0x0063f, 0x27477, 0x005ed, 0x27e17, 0x0059f, 0x287b6, 0x00554, 0x29156,
  0x0050e, 0x29af6, 0x004cc, 0x2a497, 0x0048d, 0x2ae35, 0x00451, 0x2b7d6, 0x00418, 0x2c176, 0x003e2, 0x2cb15, 0x003af, 0x2d4b5, 0x0037f, 0x2de55
#endif
};



static byte g_bc_next_state[128][2];
void bc_init_next_state_table()
{
	int /*i, */j;
	for(j=0;j<128;j++)
	{
		if((j&1) == 0)
		{	
			g_bc_next_state[j][0] = g_bc_next_state_MPS[j];g_bc_next_state[j][1] = g_bc_next_state_LPS[j];
		}
		else
		{	
			g_bc_next_state[j][0] = g_bc_next_state_LPS[j];g_bc_next_state[j][1] = g_bc_next_state_MPS[j];
		}
	}
}


uint bc_bitcnt(bitstream_t *bs, binary_model_t* bm)
{
   return (uint)(bm->m_fracBits >> 15 );
}

void bc_encode_bin_EP( enc_env_t* ee, uint binValue)
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += bm->m_binCountIncrement;
	bm->m_fracBits += 32768;
}

void bc_encode_bins_EP( enc_env_t* ee, uint binValues, int numBins )
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += numBins & -bm->m_binCountIncrement;
	bm->m_fracBits += 32768 * numBins;
}

void bc_encode_bin( enc_env_t* ee, context_model_t *cm, uint binValue)
{

	binary_model_t* bm = ee->b_ctx;

	bm->m_uiBinsCoded += bm->m_binCountIncrement;
  
	bm->m_fracBits += g_bc_entropy_bits[ cm->state ^ binValue ];
	cm->state = g_bc_next_state[cm->state][binValue];
}

void bc_encode_bin_TRM( enc_env_t* ee, uint binValue)//encode terminating bit
{
	binary_model_t* bm = ee->b_ctx;
	bm->m_uiBinsCoded += bm->m_binCountIncrement;
	bm->m_fracBits += g_bc_entropy_bits[126 ^ binValue];
}

void bc_finish(enc_env_t* ee)
{
}


void bm_start(binary_model_t* b_ctx)
{
  b_ctx->m_uiLow            = 0;
  b_ctx->m_bitsLeft         = 23;
  b_ctx->m_numBufferedBytes = 0;
  b_ctx->m_bufferedByte     = 0xff;
  b_ctx->m_uiRange          = 510;
}


void bm_reset_bits(binary_model_t* b_ctx)
{
  b_ctx->m_uiLow            = 0;
  b_ctx->m_bitsLeft         = 23;
  b_ctx->m_numBufferedBytes = 0;
  b_ctx->m_bufferedByte     = 0xff;
  if ( b_ctx->m_binCountIncrement )
  {
    b_ctx->m_uiBinsCoded = 0;
  }
#if FAST_BIT_EST
  b_ctx->m_fracBits &= 32767;
#endif
}


void bm_map_funcs(enc_env_t* ee)
{
	if(ee->type == EE_ENCODER)
	{
		ee->ee_reset_bits = bm_reset_bits;
		ee->ee_start = bm_start;
		ee->ee_bitcnt = be_bitcnt;
		ee->ee_encode_bin = be_encode_bin;
		ee->ee_encode_bin_EP = be_encode_bin_EP;
		ee->ee_encode_bins_EP = be_encode_bins_EP;
		ee->ee_encode_bin_TRM = be_encode_bin_TRM;
		ee->ee_finish = be_finish;
	}
	else if(ee->type == EE_COUNTER)
	{
		ee->ee_reset_bits = bm_reset_bits;
		ee->ee_start = bm_start;
		ee->ee_bitcnt = bc_bitcnt;
		ee->ee_encode_bin = bc_encode_bin;
		ee->ee_encode_bin_EP = bc_encode_bin_EP;
		ee->ee_encode_bins_EP = bc_encode_bins_EP;
		ee->ee_encode_bin_TRM = bc_encode_bin_TRM;
		ee->ee_finish = bc_finish;
	}
}


void bm_copy_binary_model(binary_model_t *bm_src, binary_model_t *bm_dst)
{
  bm_dst->m_uiLow           = bm_src->m_uiLow;
  bm_dst->m_uiRange         = bm_src->m_uiRange;
  bm_dst->m_bitsLeft        = bm_src->m_bitsLeft;
  bm_dst->m_bufferedByte    = bm_src->m_bufferedByte;
  bm_dst->m_numBufferedBytes = bm_src->m_numBufferedBytes;
#if FAST_BIT_EST
  bm_dst->m_fracBits = bm_src->m_fracBits;
#endif
}

