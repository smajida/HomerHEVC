/*****************************************************************************
 * hmr_sse42_functions.h : homerHEVC encoding library
/*****************************************************************************
 * Copyright (C) 2014 homerHEVC project
 *
 * Juan Casal <jcasal@homerhevc.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *****************************************************************************/

#ifndef __HOMER_HEVC_SSE42_FUNCTIONS__
#define __HOMER_HEVC_SSE42_FUNCTIONS__


#include "hmr_private.h"
#include "hmr_common.h"
//SSE 4.2

//------------------------------------------------------copy -------------------------------------------------------------------------

void copy_16_8(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);
void sse_copy_16_8(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);

void copy_8_16(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);
void sse_copy_8_16(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);

void copy_16_16(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);
void sse_copy_16_16(void* vsrc, uint32_t src_stride, void* vdst, uint32_t dst_stride, int height, int width);


//------------------------------------------------------sad -------------------------------------------------------------------------
uint32_t sse_aligned_sad(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride, int size);

//#define NOINLINE	__declspec(noinline) 

uint32_t sse_aligned_sad_4x4(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_sad_8x8(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
//uint32_t sse_aligned_sad_nxn_16x16(int16_t * src, uint32_t src_stride, uint8_t * pred, uint32_t pred_stride, int size);
uint32_t sse_aligned_sad_16x16(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_sad_32x32(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_sad_64x64(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);


//------------------------------------------------------ssd -------------------------------------------------------------------------
/*uint32_t sse_aligned_ssd(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride, int size);


//uint32_t sse_ssd_nxn_16x16(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride, uint32_t size);
uint32_t sse_aligned_ssd_16x16(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd_32x32(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd_64x64(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd_8x8(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd_4x4(int16_t * src, uint32_t src_stride, int16_t * pred, uint32_t pred_stride);

*/
//------------------------------------------------------ssd16b -------------------------------------------------------------------------
uint32_t sse_aligned_ssd16b(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride, int size);

uint32_t sse_aligned_ssd16b_4x4(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd16b_8x8(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd16b_16x16(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd16b_32x32(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
uint32_t sse_aligned_ssd16b_64x64(int16_t *src, uint32_t src_stride, int16_t *pred, uint32_t pred_stride);
//------------------------------------------------------predict -------------------------------------------------------------------------

void sse_aligned_predict(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride, int size);

void sse_aligned_predict_4x4(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride);
void sse_aligned_predict_8x8(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride);
void sse_aligned_predict_16x16(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride);
void sse_aligned_predict_32x32(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride);
void sse_aligned_predict_64x64(int16_t *  orig, int orig_stride, int16_t*  pred, int pred_stride, int16_t *  residual, int residual_stride);

void sse_adi_filter(int16_t  *ptr, int16_t  *ptr_filter, int depth, int adi_size, int partition_size, int max_cu_size_shift, int intra_smooth_enable, int bit_depth);
//------------------------------------------------------reconst -------------------------------------------------------------------------
void sse_aligned_reconst(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride, int size);

void sse_aligned_reconst_4x4(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride);
void sse_aligned_reconst_8x8(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride);
void sse_aligned_reconst_16x16(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride);
void sse_aligned_reconst_32x32(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride);
void sse_aligned_reconst_64x64(int16_t* pred, int pred_stride, int16_t * residual, int residual_stride, int16_t* decoded, int decoded_stride);

//------------------------------------------------------transform -------------------------------------------------------------------------
void sse_transform(int bit_depth, int16_t *block,int16_t *coeff, int block_size, int iWidth, int iHeight, int width_shift, int height_shift, unsigned short uiMode, int16_t *aux);
void sse_itransform(int bit_depth, short *block,short *coeff, int block_size, int iWidth, int iHeight, unsigned int uiMode, short *aux);

void sse_aligned_dst_4x4(int16_t *src, int16_t *dst, int stride);
void sse_aligned_inv_dst_4x4(int16_t *src, int16_t *dst, int stride);

void sse_aligned_dct_4x4(int16_t *src, int16_t *dst, int stride);
void sse_aligned_inv_dct_4x4(int16_t *src, int16_t *dst, int stride);

void sse_aligned_dct_8x8(int16_t *src, int16_t *dst, int stride);
void sse_aligned_dct_8x8_1(int16_t *src, int16_t *dst, int stride);
void sse_aligned_inv_dct_8x8(int16_t *src, int16_t *dst, int stride);
void sse_aligned_inv_dct_8x8_v2(int16_t *src, int16_t *dst, int dst_stride, int16_t *aux);

void sse_aligned_dct_16x16(int16_t *src, int16_t *dst, int stride) ;
void sse_aligned_inv_dct_16x16(int16_t *src, int16_t *dst, int dst_stride, int16_t *aux);

void sse_aligned_dct_32x32(int16_t *src, int16_t *dst, int stride, int16_t *aux) ;
void sse_aligned_inv_dct_32x32(int16_t *src, int16_t *dst, int stride, int16_t *aux);




//-----------------------------------------------quant -------------------------------------------
void sse_aligned_quant(henc_thread_t* et, int16_t* src, int16_t* dst, int scan_mode, int depth, int comp, int cu_mode, int is_intra, int *ac_sum, int cu_size, int per, int rem);
void sse_aligned_inv_quant(henc_thread_t* et, short *src, short *dst, int depth, int comp, int is_intra, int cu_size, int per, int rem);


//----------------------------------------------- intra prediction -------------------------------------------
void sse_create_intra_planar_prediction_4(int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size);
void sse_create_intra_planar_prediction_8(int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size);
void sse_create_intra_planar_prediction_16_32_64(int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_size_shift);


void sse_create_intra_angular_prediction_4x4(henc_thread_t* et, ctu_info_t* ctu, int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size, int cu_mode, int is_luma);
void sse_create_intra_angular_prediction_8x8(henc_thread_t* et, ctu_info_t* ctu, int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size, int cu_mode, int is_luma);
void sse_create_intra_angular_prediction_nxn(henc_thread_t* et, ctu_info_t* ctu, int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_mode, int is_luma);

//void sse_create_intra_planar_prediction(uint8_t* ref_wnd, int ref_wnd_stride_2D, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_size_shift);
void sse_create_intra_planar_prediction(henc_thread_t* et, int16_t* pred, int pred_stride, short  *adi_pred_buff, int adi_size, int cu_size, int cu_size_shift);
void sse_create_intra_angular_prediction(henc_thread_t* et, ctu_info_t* ctu, int16_t *pred, int pred_stride, int16_t  *adi_pred_buff, int adi_size, int cu_size, int cu_mode, int is_luma);


//----------------------------------------------- inter prediction -------------------------------------------
void sse_interpolate_luma(int16_t *src, int src_stride, int16_t *dst, int dst_stride, int fraction, int width, int height, int is_vertical, int is_first, int is_last);
void sse_interpolate_chroma(int16_t *src, int src_stride, int16_t *dst, int dst_stride, int fraction, int width, int height, int is_vertical, int is_first, int is_last);

void sse_weighted_average_motion(int16_t* src0, int src0_stride, int16_t* src1, int src1_stride, int16_t* dst, int dst_stride, int height, int width, int bit_depth);

void sse_filter_copy(int16_t *src, int src_stride, int16_t *dst, int dst_stride, int fraction, int width, int height, int is_vertical, int is_first, int is_last);


//----------------------------------------------variance ------------------------------------------------
uint32_t sse_modified_variance(int16_t * p, int size, int stride, int modif);


//---------------------------------------------- sao ------------------------------------------------
void sse_sao_get_ctu_stats(henc_thread_t *wpp_thread, slice_t *currslice, ctu_info_t* ctu, sao_stat_data_t stats[][NUM_SAO_NEW_TYPES]);

#endif /*__HOMER_HEVC_SSE42_FUNCTIONS__*/