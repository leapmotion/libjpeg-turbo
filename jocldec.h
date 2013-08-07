/*
 * jocldec.h
 *
 * Copyright (C) 2012-2013, MulticoreWare Inc.
 * In July 2013, Written by Peixuan Zhang <zhangpeixuan.cn@gmail.com>
 * The OpenCL kernel code is written by
 *   Chunli  Zhang <chunli@multicorewareinc.com> and
 *   Peixuan Zhang <peixuan@multicorewareinc.com>
 * Based on the OpenCL extension for IJG JPEG library,
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the decoding JPEG code with OpenCL.
 */

#ifndef __JPEG_OPENCL_DECODING_H__
#define __JPEG_OPENCL_DECODING_H__

/* Global OpenCL buffer and mapping variables. */
extern cl_mem   jocl_global_data_mem_input;
extern cl_mem   jocl_global_data_mem_output;
extern cl_mem   jocl_global_data_mem_qutable;
extern cl_mem   jocl_global_data_mem_inter;
extern float*   jocl_global_data_ptr_qutable;
extern JCOEFPTR jocl_global_data_ptr_input;
extern JSAMPROW jocl_global_data_ptr_output;

/* Compile OpenCL kernels. */
cl_bool jocldec_build_kernels(j_decompress_ptr cinfo);

/* Run decoding kernels for full image. */
cl_bool jocldec_run_kernels_full_image(
  j_decompress_ptr cinfo,
  int block_in_mcu,               /* Number of 8x8 blocks in one MCU */
  int num_of_mcu,                 /* Number of MCUs in one whole image */
  unsigned int mcu_in_width,      /* Number of MCUs in image width */
  unsigned int offset_input,      /* The offset of buffer used to calculate the global ID */
  int total_mcu_num,              /* The total MCU-number of an image */
  int decodeMCU);                 /* The number of MCUs will be decoded */
#endif
