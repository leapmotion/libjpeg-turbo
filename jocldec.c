/*
 * jocldec.c
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

#include "jinclude.h"
#include "jpeglib.h"
#include "CL/opencl.h"
#include "joclinit.h"
#include "jocldec.h"
#include "jocldec_kernels.h"

#ifdef WITH_OPENCL_DECODING_SUPPORTED
cl_mem   jocl_global_data_mem_input   = NULL;
cl_mem   jocl_global_data_mem_output  = NULL;
cl_mem   jocl_global_data_mem_qutable = NULL;
cl_mem   jocl_global_data_mem_inter   = NULL;
JCOEFPTR jocl_global_data_ptr_input   = NULL;
JSAMPROW jocl_global_data_ptr_output  = NULL;
float*   jocl_global_data_ptr_qutable = NULL;

static JOCL_CL_RUNDATA* jocldec_cl_rundata = NULL;

cl_bool jocldec_build_kernels(j_decompress_ptr cinfo)
{
  cl_int  err_code;
  cl_uint data_m = 6;
  cl_uint blocksWidth  = 64;
  int     offset_input = 0;
  size_t  global_ws[1], local_ws[1];

  static  const char **jocldec_cl_source;
  char    **jocldec_cl_source_inter;
  int     i;
  jocldec_cl_source_inter = (char**) malloc(7 * sizeof(char*));
  for(i=0; i<7; ++i) {
    jocldec_cl_source_inter[i] = (char*) malloc(60000 * sizeof(char));
  }
  strcpy(jocldec_cl_source_inter[0], jocldec_cl_source1);
  strcpy(jocldec_cl_source_inter[1], jocldec_cl_source2);
  strcpy(jocldec_cl_source_inter[2], jocldec_cl_source3);
  strcpy(jocldec_cl_source_inter[3], jocldec_cl_source4);
  strcpy(jocldec_cl_source_inter[4], jocldec_cl_source5);
  strcpy(jocldec_cl_source_inter[5], jocldec_cl_source6);
  strcpy(jocldec_cl_source_inter[6], jocldec_cl_source7);
  jocldec_cl_source = jocldec_cl_source_inter;

  global_ws[0] = 256;
  local_ws[0] = 64;

  if (!jocldec_cl_rundata)  { 
    const char* kernel_names[] = {"IDCT_FAST_SHORT",
                                  "IDCT_SLOW_INT",
                                  "IDCT_FAST_FLOAT",

                                  "UPSAMPLE_H1V1_RGB",
                                  "UPSAMPLE_H1V2_RGB",
                                  "UPSAMPLE_H2V1_RGB",
                                  "UPSAMPLE_H2V2_RGB",

                                  "UPSAMPLE_H2V1_FANCY_RGB",
                                  "UPSAMPLE_H2V2_FANCY_RGB",

                                  "UPSAMPLE_H1V1_RGBA",
                                  "UPSAMPLE_H1V2_RGBA",
                                  "UPSAMPLE_H2V1_RGBA",
                                  "UPSAMPLE_H2V2_RGBA",

                                  "UPSAMPLE_H2V1_FANCY_RGBA",
                                  "UPSAMPLE_H2V2_FANCY_RGBA",

                                  "UPSAMPLE_H1V1_BGR",
                                  "UPSAMPLE_H1V2_BGR",
                                  "UPSAMPLE_H2V1_BGR",
                                  "UPSAMPLE_H2V2_BGR",

                                  "UPSAMPLE_H2V1_FANCY_BGR",
                                  "UPSAMPLE_H2V2_FANCY_BGR",
                                   NULL};
    jocldec_cl_rundata = jocl_cl_compile_and_build(jocldec_cl_source, kernel_names);
    /* IDCT FAST SHORT */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[0],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_input),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[0],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[0],
                                                 2,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_qutable),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[0],
                                                 3,
                                                 1024*sizeof(int),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[0],
                                                 4,
                                                 sizeof(int),
                                                 &data_m),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[0],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* IDCT SLOW INT */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[1],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_input),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[1],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[1],
                                                 2,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_qutable),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[1],
                                                 3,
                                                 1024*sizeof(int),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[1],
                                                 4,
                                                 sizeof(int),
                                                 &data_m),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[1],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* IDCT FAST FLOAT */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[2],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_input),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[2],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[2],
                                                 2,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_qutable),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[2],
                                                 3,
                                                 1024*sizeof(int),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[2],
                                                 4,
                                                 sizeof(float),
                                                 &data_m),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[2],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H1V1 RGB */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[3],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[3],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[3],
                                                 2,
                                                 192*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[3],
                                                 3,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);
    
    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[3],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),);
    /* H1V2 RGB */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 3,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 4,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);  
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[4],
                                                 5,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[4],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
   /* H2V1 RGB */
   CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[5],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[5],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V2 RGB */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 2,
                                                 768*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[6],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[6],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V1 RGB FANCY */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[7],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[7],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V2 RGB FANCY */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 2,
                                                 768*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[8],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[8],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /*H1V1_RGBA*/
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[9],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[9],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[9],
                                                 2,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[9],
                                                 3,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[9],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),);

    /*H1V2_RGBA*/
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 2,
                                                 512*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 3,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 4,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);  
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[10],
                                                 5,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[10],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /*H2V1_RGBA*/
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 2,
                                                 512*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[11],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);
    
    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[11],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);

    /*H2V2_RGBA*/
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 2,
                                                 1024*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[12],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[12],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /*H2V1_RGBA_FANCY*/
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 2,
                                                 512*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[13],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[13],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /*H2V2_RGBA_FANCY */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 2,
                                                 1024*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[14],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[14],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H1V1 BGR */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[15],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[15],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[15],
                                                 2,
                                                 192*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[15],
                                                 3,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[15],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),);
    /* H1V2 BGR */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 3,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 4,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);  
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[16],
                                                 5,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[16],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V1 BGR */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[17],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[17],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V2 BGR */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 2,
                                                 768*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[18],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[18],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V1 BGR FANCY  */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19], 
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 2,
                                                 384*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 4,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 5,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 6,
                                                 128*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[19],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[19],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    /* H2V2 BGR FANCY */
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_output),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 2,
                                                 768*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 3,
                                                 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 4, 64*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE); 
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 5, 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 6,
                                                 256*sizeof(unsigned char),
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_rundata->kernel[20],
                                                 7,
                                                 sizeof(unsigned int),
                                                 &blocksWidth),
                                                 CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_rundata->kernel[20],
                                                 1,
                                                 0,
                                                 global_ws,
                                                 local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clFinish(jocl_cl_get_command_queue()),CL_FALSE);
  }
  return CL_TRUE;
}

cl_bool jocldec_run_kernels_full_image(
                            j_decompress_ptr cinfo,
                            int data_m,
                            int mcu_num,
                            unsigned int blocksWidth,
                            unsigned int offset_mcu,
                            int total_mcu_num,
                            int decodeMCU)
{
  cl_int err_code;
  size_t global_ws,local_ws;
  int    mcu_out,mcu_in;
  int    data_m_inter = data_m;
  int    data_index = 0,sign = 0;
  int    size_map;
  unsigned int offset_output,offset_input;
  cl_kernel jocldec_cl_kernel_use;

  int KernelArg2 = 0;
  int KernelArg5 = 0;
  int KernelArg6 = 0;

  if (data_m==5) data_m_inter = 4;
  switch(data_m) {
    case 3: mcu_out   = 192;
            mcu_in    = 192;
            global_ws = (mcu_num * mcu_in /8+255)/256*256;
            break;
    case 5: mcu_out   = 384;
            mcu_in    = 256;
            global_ws = (mcu_num * mcu_in /8+255)/256*256;
            break;
    case 4: mcu_out   = 384;
            mcu_in    = 256;
            global_ws = (mcu_in / 8 + mcu_num * mcu_in / 8 +255)/256*256;
            break;
    case 6: mcu_out   = 768;
            mcu_in    = 384;
            global_ws = (mcu_num * mcu_in /8+255)/256*256;
            break;
  }
  if(CL_TRUE == jocl_cl_is_nvidia_opencl()) {
    CL_SAFE_CALL0(jocl_global_data_mem_input = jocl_clCreateBuffer(jocl_cl_get_context(),
      CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
      MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * 4, jocl_global_data_ptr_input, &err_code),return CL_FALSE);
    CL_SAFE_CALL0(jocl_global_data_mem_qutable = jocl_clCreateBuffer(jocl_cl_get_context(),
      CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
      4096, jocl_global_data_ptr_qutable, &err_code),return CL_FALSE);
}
#ifndef JOCL_CL_OS_WIN32
  if(CL_FALSE == jocl_cl_is_nvidia_opencl()) {
    CL_SAFE_CALL0(err_code = jocl_clEnqueueUnmapMemObject(
      jocl_cl_get_command_queue(), jocl_global_data_mem_input,
      jocl_global_data_ptr_input, 0, NULL, NULL),return CL_FALSE);
  }
#endif
  offset_input = offset_mcu * mcu_in/8;
  switch (cinfo->dct_method){
    case JDCT_IFAST:
      local_ws = 256;
      jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[0];
      break;
    case JDCT_ISLOW:
      local_ws = 128;
      jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[1];
      break;
    case JDCT_FLOAT:
      local_ws = 128;
      jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[2];
      break;
  }
  if (0 == offset_mcu) {
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                 0,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_input),
                                                 return CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                 1,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_inter),
                                                 return CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                 2,
                                                 sizeof(cl_mem),
                                                 &jocl_global_data_mem_qutable),
                                                 return CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                 3,
                                                 1024*sizeof(int),
                                                 NULL),
                                                 return CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                 4,
                                                 sizeof(int),
                                                 &data_m_inter),
                                                 return CL_FALSE);
  }
  CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                 jocldec_cl_kernel_use,
                                                 1,
                                                 &offset_input,
                                                 &global_ws,
                                                 &local_ws,
                                                 0,
                                                 NULL,
                                                 NULL),
                                                 return CL_FALSE);
  if(3 == data_m){
	global_ws     = mcu_num * 64;
    offset_output = offset_mcu * 64;
    local_ws      = 64;
	switch (cinfo->out_color_space){
	  case JCS_RGB:
        jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[3];
        break;
	  case JCS_EXT_RGBA:
        jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[9];
        break;
	  case JCS_EXT_BGR:
        jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[15];
        break;
	}/*end switch */
	if (0 == offset_mcu) {
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    0,
                                                    sizeof(cl_mem),
                                                    &jocl_global_data_mem_inter),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    1,
                                                    sizeof(cl_mem),
                                                    &jocl_global_data_mem_output),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    2,
                                                    256*sizeof(unsigned char),
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    3,
                                                    sizeof(unsigned int),
                                                    &blocksWidth),
                                                    return CL_FALSE);
        }
       CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                       jocldec_cl_kernel_use,
                                                       1,
                                                       &offset_output,
                                                       &global_ws,
                                                       &local_ws,
                                                       0,
                                                       NULL,
                                                       NULL),
                                                       return CL_FALSE);
  }  /*1v1 end*/
  else if(5 == data_m){ 
	offset_output = offset_mcu * 128;
    global_ws     = mcu_num * 128;
    local_ws      = 128;
	switch (cinfo->out_color_space){
	  case JCS_RGB:
		KernelArg2 = 384 * sizeof(unsigned char);
		jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[4];
	    break;
	  case JCS_EXT_RGBA:
	  	KernelArg2 = 512 * sizeof(unsigned char);
	  	jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[10];
        break;
	  case JCS_EXT_BGR:
	  	KernelArg2 = 384 * sizeof(unsigned char);
	  	jocldec_cl_kernel_use = jocldec_cl_rundata->kernel[16];
        break;
	}/*end switch */
	if (0 == offset_mcu) {
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   0,
                                                   sizeof(cl_mem),
                                                   &jocl_global_data_mem_inter),
                                                   return CL_FALSE);
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   1,
                                                   sizeof(cl_mem),
                                                   &jocl_global_data_mem_output),
                                                   return CL_FALSE);
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   2,
                                                   KernelArg2,
                                                   NULL),
                                                   return CL_FALSE);
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   3,
                                                   128*sizeof(unsigned char),
                                                   NULL),
                                                   return CL_FALSE);
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   4,
                                                   128*sizeof(unsigned char),
                                                   NULL),
                                                   return CL_FALSE);
      CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                   5,
                                                   sizeof(unsigned int),
                                                   &blocksWidth),
                                                   return CL_FALSE);
      }
      CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                       jocldec_cl_kernel_use,
                                                       1,
                                                       &offset_output,
                                                       &global_ws,
                                                       &local_ws,
                                                       0,
                                                       NULL,
                                                       NULL),
                                                       return CL_FALSE);
  }/*1v2 end*/
  else if((4 == data_m ) || (6 == data_m )){
	switch (cinfo->out_color_space){
	  case JCS_RGB:{
	    if(4 == data_m){
          KernelArg2 = 384 * sizeof(unsigned char);
	  	  KernelArg5 = 128 * sizeof(unsigned char);
	  	  KernelArg6 = 128 * sizeof(unsigned char);
	  
	  	  offset_output = offset_mcu * 64;
          local_ws      = 64;
	  	  if(CL_TRUE == jocl_cl_get_fancy_status()){
	  	    global_ws = (mcu_num+1) * 64;
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[7];
          }
	  	  else{
	  	    global_ws = mcu_num * 64;
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[5];
	  	  }
	    }/*end if(data_m == 4)*/
	    else if(6 == data_m){
	      KernelArg2 = 768 * sizeof(unsigned char);
	  	  KernelArg5 = 256 * sizeof(unsigned char);
	  	  KernelArg6 = 256 * sizeof(unsigned char);
	  
	  	  offset_output = offset_mcu * 128;
          global_ws     = mcu_num * 128;
          local_ws      = 128;
	  	  if(1){
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[6];
	  	  }
	  	  else{
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[8];
	  	  }      
	    }/*end if(data_m == 6)*/
	  }/*end of case JCS_RGB:*/
	  break;
	  case JCS_EXT_RGBA:{
	    if(4 == data_m){
          KernelArg2 = 512 * sizeof(unsigned char);
	  	  KernelArg5 = 128 * sizeof(unsigned char);
	  	  KernelArg6 = 128 * sizeof(unsigned char);
	  
	  	  offset_output = offset_mcu * 64;
          global_ws     = mcu_num * 64;
          local_ws      = 64;
	  	  if(CL_TRUE == jocl_cl_get_fancy_status()){
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[13];
	  	  }
	  	  else{
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[11];
	  	  }
	    }/*end if(data_m == 4)*/
	    else if(6 == data_m){
	      KernelArg2 = 1024 * sizeof(unsigned char);
	  	  KernelArg5 = 256 * sizeof(unsigned char);
	  	  KernelArg6 = 256 * sizeof(unsigned char);
	  
	  	  offset_output = offset_mcu * 128;
          global_ws     = mcu_num * 128;
          local_ws      = 128;
	  	  if(1){
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[12];
	  	  }
	  	  else{
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[14];
	  	  }    
	    }/* end if(data_m == 6) */ 
	  }/*end of case JCS_EXT_RGBA:*/
	  break;
	  case JCS_EXT_BGR:{
		if(4 == data_m){
          KernelArg2 = 348 * sizeof(unsigned char);
	  	  KernelArg5 = 128 * sizeof(unsigned char);
	  	  KernelArg6 = 128 * sizeof(unsigned char);
	  
	  	  offset_output = offset_mcu * 64;
          local_ws      = 64;
	  	  if(CL_TRUE == jocl_cl_get_fancy_status()){
		    global_ws = (mcu_num + 1) * 64;
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[19];
	  	  }
	  	  else{
			global_ws = mcu_num * 64;
	  	    jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[17];
	  	  }
        }/*end if(data_m == 4)*/
        else if(6 == data_m){
          KernelArg2 = 768 * sizeof(unsigned char);
          KernelArg5 = 256 * sizeof(unsigned char);
          KernelArg6 = 256 * sizeof(unsigned char);

          offset_output = offset_mcu * 128;
          global_ws     = mcu_num * 128;
          local_ws      = 128;
          if(1){
            jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[18];
          }
          else{
            jocldec_cl_kernel_use = jocldec_cl_rundata -> kernel[20];
          }
      }/* end if(data_m == 6)*/
     }/*end of case JCS_EXT_BGR:*/
     break;
    }/*end switch */
    if (0 == offset_mcu) {
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    0,
                                                    sizeof(cl_mem),
                                                    &jocl_global_data_mem_inter),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    1,
                                                    sizeof(cl_mem),
                                                    &jocl_global_data_mem_output),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    2,
                                                    KernelArg2,
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    3,
                                                    64*sizeof(unsigned char),
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    4,
                                                    64*sizeof(unsigned char),
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    5,
                                                    KernelArg5,
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    6,
                                                    KernelArg6,
                                                    NULL),
                                                    return CL_FALSE);
       CL_SAFE_CALL0(err_code = jocl_clSetKernelArg(jocldec_cl_kernel_use,
                                                    7,
                                                    sizeof(unsigned int),
                                                    &blocksWidth),
                                                    return CL_FALSE);
       }
       CL_SAFE_CALL0(err_code = jocl_clEnqueueNDRangeKernel(jocl_cl_get_command_queue(),
                                                         jocldec_cl_kernel_use,
                                                         1,
                                                         &offset_output,
                                                         &global_ws,
                                                         &local_ws,
                                                         0,
                                                         NULL,
                                                         NULL),
                                                         return CL_FALSE);

  }
  size_map = cinfo->max_h_samp_factor * cinfo->MCUs_per_row * DCTSIZE *
    cinfo->image_height * NUM_COMPONENT;

  if (decodeMCU != total_mcu_num) {
    CL_SAFE_CALL0(err_code = jocl_clFlush(jocl_cl_get_command_queue()),return CL_FALSE);
  }
  else {
    CL_SAFE_CALL0(err_code = jocl_clFinish(jocl_cl_get_command_queue()),return CL_FALSE);
    CL_SAFE_CALL0(jocl_global_data_ptr_output = (JSAMPROW)jocl_clEnqueueMapBuffer(
      jocl_cl_get_command_queue(), jocl_global_data_mem_output, CL_TRUE,
      CL_MAP_READ, 0, size_map, 0, NULL, NULL, &err_code),return CL_FALSE);
    CL_SAFE_CALL0(err_code = jocl_clEnqueueUnmapMemObject(
      jocl_cl_get_command_queue(), jocl_global_data_mem_output,
      jocl_global_data_ptr_output, 0, NULL, NULL),return CL_FALSE);

    CL_SAFE_CALL0(err_code = jocl_clFinish(jocl_cl_get_command_queue()),return CL_FALSE);
  }
  return CL_TRUE;
}
#endif
