/*
 * joclinit.h
 *
 * Copyright (C) 2012-2013, MulticoreWare Inc.
 * In July 2012, Written by Peixuan Zhang <zhangpeixuan.cn@gmail.com>
 * Based on the OpenCL extension for IJG JPEG library,
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the initialization of OpenCL.
 *
 * Any module that need to call OpenCL API,
 * should include this header file first.
 */

#ifndef __DYNAMIC_LOADING_OPENCL_INIT_H__
#define __DYNAMIC_LOADING_OPENCL_INIT_H__


/*
 * In order to ensure that this extension can be successfully compiled
 * on multiple platforms, we chose the dynamic loading of OpenCL.
 * The following APIs belong OpenCL 1.2 standard.
 */

typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetPlatformIDs                 ) (cl_uint, cl_platform_id*, cl_uint*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetPlatformInfo                ) (cl_platform_id, cl_platform_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetDeviceIDs                   ) (cl_platform_id, cl_device_type, cl_uint, cl_device_id*, cl_uint*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetDeviceInfo                  ) (cl_device_id, cl_device_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clCreateSubDevices               ) (cl_device_id, const cl_device_partition_property*, cl_uint, cl_device_id*, cl_uint*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainDevice                   ) (cl_device_id);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseDevice                  ) (cl_device_id);
typedef CL_API_ENTRY cl_context       (CL_API_CALL *h_clCreateContext                  ) (const cl_context_properties*, cl_uint, const cl_device_id*, void (CL_CALLBACK*)(const char*, const void*, size_t, void*), void*, cl_int*);
typedef CL_API_ENTRY cl_context       (CL_API_CALL *h_clCreateContextFromType          ) (const cl_context_properties*, cl_device_type, void (CL_CALLBACK*)(const char*, const void*, size_t, void*), void*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainContext                  ) (cl_context);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseContext                 ) (cl_context);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetContextInfo                 ) (cl_context, cl_context_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_command_queue (CL_API_CALL *h_clCreateCommandQueue             ) (cl_context, cl_device_id, cl_command_queue_properties, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainCommandQueue             ) (cl_command_queue);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseCommandQueue            ) (cl_command_queue);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetCommandQueueInfo            ) (cl_command_queue, cl_command_queue_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_mem           (CL_API_CALL *h_clCreateBuffer                   ) (cl_context, cl_mem_flags, size_t, void*, cl_int*);
typedef CL_API_ENTRY cl_mem           (CL_API_CALL *h_clCreateSubBuffer                ) (cl_mem, cl_mem_flags, cl_buffer_create_type, const void*, cl_int*);
typedef CL_API_ENTRY cl_mem           (CL_API_CALL *h_clCreateImage                    ) (cl_context, cl_mem_flags, const cl_image_format*, const cl_image_desc*, void*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainMemObject                ) (cl_mem);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseMemObject               ) (cl_mem);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetSupportedImageFormats       ) (cl_context, cl_mem_flags, cl_mem_object_type, cl_uint, cl_image_format*, cl_uint*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetMemObjectInfo               ) (cl_mem, cl_mem_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetImageInfo                   ) (cl_mem, cl_image_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clSetMemObjectDestructorCallback ) (cl_mem, void (CL_CALLBACK*)(cl_mem,void*), void*);
typedef CL_API_ENTRY cl_sampler       (CL_API_CALL *h_clCreateSampler                  ) (cl_context, cl_bool, cl_addressing_mode, cl_filter_mode, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainSampler                  ) (cl_sampler);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseSampler                 ) (cl_sampler);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetSamplerInfo                 ) (cl_sampler, cl_sampler_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_program       (CL_API_CALL *h_clCreateProgramWithSource        ) (cl_context, cl_uint, const char**, const size_t*, cl_int*);
typedef CL_API_ENTRY cl_program       (CL_API_CALL *h_clCreateProgramWithBinary        ) (cl_context, cl_uint, const cl_device_id*, const size_t*, const unsigned char**, cl_int*, cl_int*);
typedef CL_API_ENTRY cl_program       (CL_API_CALL *h_clCreateProgramWithBuiltInKernels) (cl_context, cl_uint, const cl_device_id*, const char*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainProgram                  ) (cl_program);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseProgram                 ) (cl_program);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clBuildProgram                   ) (cl_program, cl_uint, const cl_device_id*, const char*, void (CL_CALLBACK*)(cl_program, void*), void*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clCompileProgram                 ) (cl_program, cl_uint, const cl_device_id*, const char*, cl_uint, const cl_program*, const char**, void (CL_CALLBACK*)(cl_program, void*), void*);
typedef CL_API_ENTRY cl_program       (CL_API_CALL *h_clLinkProgram                    ) (cl_context, cl_uint, const cl_device_id*, const char*, cl_uint, const cl_program*, void (CL_CALLBACK*)(cl_program, void*), void*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clUnloadPlatformCompiler         ) (cl_platform_id);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetProgramInfo                 ) (cl_program, cl_program_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetProgramBuildInfo            ) (cl_program, cl_device_id, cl_program_build_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_kernel        (CL_API_CALL *h_clCreateKernel                   ) (cl_program, const char*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clCreateKernelsInProgram         ) (cl_program, cl_uint, cl_kernel*, cl_uint*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainKernel                   ) (cl_kernel);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseKernel                  ) (cl_kernel);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clSetKernelArg                   ) (cl_kernel, cl_uint, size_t, const void*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetKernelInfo                  ) (cl_kernel, cl_kernel_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetKernelArgInfo               ) (cl_kernel, cl_uint, cl_kernel_arg_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetKernelWorkGroupInfo         ) (cl_kernel, cl_device_id, cl_kernel_work_group_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clWaitForEvents                  ) (cl_uint, const cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetEventInfo                   ) (cl_event, cl_event_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_event         (CL_API_CALL *h_clCreateUserEvent                ) (cl_context, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clRetainEvent                    ) (cl_event);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clReleaseEvent                   ) (cl_event);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clSetUserEventStatus             ) (cl_event, cl_int);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clSetEventCallback               ) (cl_event, cl_int, void (CL_CALLBACK*)(cl_event, cl_int, void*), void*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clGetEventProfilingInfo          ) (cl_event, cl_profiling_info, size_t, void*, size_t*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clFlush                          ) (cl_command_queue);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clFinish                         ) (cl_command_queue);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueReadBuffer              ) (cl_command_queue, cl_mem, cl_bool, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueReadBufferRect          ) (cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueWriteBuffer             ) (cl_command_queue, cl_mem, cl_bool, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueWriteBufferRect         ) (cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueFillBuffer              ) (cl_command_queue, cl_mem, const void*, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueCopyBuffer              ) (cl_command_queue, cl_mem, cl_mem, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueCopyBufferRect          ) (cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, const size_t*, size_t, size_t, size_t, size_t, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueReadImage               ) (cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, size_t, size_t, void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueWriteImage              ) (cl_command_queue, cl_mem, cl_bool, const size_t*, const size_t*, size_t, size_t, const void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueFillImage               ) (cl_command_queue, cl_mem, const void*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueCopyImage               ) (cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueCopyImageToBuffer       ) (cl_command_queue, cl_mem, cl_mem, const size_t*, const size_t*, size_t, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueCopyBufferToImage       ) (cl_command_queue, cl_mem, cl_mem, size_t, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY void *           (CL_API_CALL *h_clEnqueueMapBuffer               ) (cl_command_queue, cl_mem, cl_bool, cl_map_flags, size_t, size_t, cl_uint, const cl_event*, cl_event*, cl_int*);
typedef CL_API_ENTRY void *           (CL_API_CALL *h_clEnqueueMapImage                ) (cl_command_queue, cl_mem, cl_bool, cl_map_flags, const size_t*, const size_t*, size_t*, size_t*, cl_uint, const cl_event*, cl_event*, cl_int*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueUnmapMemObject          ) (cl_command_queue, cl_mem, void*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueMigrateMemObjects       ) (cl_command_queue, cl_uint, const cl_mem*, cl_mem_migration_flags, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueNDRangeKernel           ) (cl_command_queue, cl_kernel, cl_uint, const size_t*, const size_t*, const size_t*, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueTask                    ) (cl_command_queue, cl_kernel, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueNativeKernel            ) (cl_command_queue, void (CL_CALLBACK*)(void*), void*, size_t, cl_uint, const cl_mem*, const void**, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueMarkerWithWaitList      ) (cl_command_queue, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clEnqueueBarrierWithWaitList     ) (cl_command_queue, cl_uint, const cl_event*, cl_event*);
typedef CL_API_ENTRY cl_int           (CL_API_CALL *h_clSetPrintfCallback              ) (cl_context, void (CL_CALLBACK*)(cl_context, cl_uint, char*, void*), void*);


/*
 * Declaration or definition the API pointers.
 * Use macros to streamline the code.
 */

#ifdef __JOCL_CL_INIT_MAIN__
#define CL_LOADING_PREFIX
#define CL_LOADING_SUFFIX = NULL
#else
#define CL_LOADING_PREFIX extern
#define CL_LOADING_SUFFIX
#endif

CL_LOADING_PREFIX h_clGetPlatformIDs                  jocl_clGetPlatformIDs                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetPlatformInfo                 jocl_clGetPlatformInfo                 CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetDeviceIDs                    jocl_clGetDeviceIDs                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetDeviceInfo                   jocl_clGetDeviceInfo                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateSubDevices                jocl_clCreateSubDevices                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainDevice                    jocl_clRetainDevice                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseDevice                   jocl_clReleaseDevice                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateContext                   jocl_clCreateContext                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateContextFromType           jocl_clCreateContextFromType           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainContext                   jocl_clRetainContext                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseContext                  jocl_clReleaseContext                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetContextInfo                  jocl_clGetContextInfo                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateCommandQueue              jocl_clCreateCommandQueue              CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainCommandQueue              jocl_clRetainCommandQueue              CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseCommandQueue             jocl_clReleaseCommandQueue             CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetCommandQueueInfo             jocl_clGetCommandQueueInfo             CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateBuffer                    jocl_clCreateBuffer                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateSubBuffer                 jocl_clCreateSubBuffer                 CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateImage                     jocl_clCreateImage                     CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainMemObject                 jocl_clRetainMemObject                 CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseMemObject                jocl_clReleaseMemObject                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetSupportedImageFormats        jocl_clGetSupportedImageFormats        CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetMemObjectInfo                jocl_clGetMemObjectInfo                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetImageInfo                    jocl_clGetImageInfo                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clSetMemObjectDestructorCallback  jocl_clSetMemObjectDestructorCallback  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateSampler                   jocl_clCreateSampler                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainSampler                   jocl_clRetainSampler                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseSampler                  jocl_clReleaseSampler                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetSamplerInfo                  jocl_clGetSamplerInfo                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateProgramWithSource         jocl_clCreateProgramWithSource         CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateProgramWithBinary         jocl_clCreateProgramWithBinary         CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateProgramWithBuiltInKernels jocl_clCreateProgramWithBuiltInKernels CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainProgram                   jocl_clRetainProgram                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseProgram                  jocl_clReleaseProgram                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clBuildProgram                    jocl_clBuildProgram                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCompileProgram                  jocl_clCompileProgram                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clLinkProgram                     jocl_clLinkProgram                     CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clUnloadPlatformCompiler          jocl_clUnloadPlatformCompiler          CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetProgramInfo                  jocl_clGetProgramInfo                  CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetProgramBuildInfo             jocl_clGetProgramBuildInfo             CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateKernel                    jocl_clCreateKernel                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateKernelsInProgram          jocl_clCreateKernelsInProgram          CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainKernel                    jocl_clRetainKernel                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseKernel                   jocl_clReleaseKernel                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clSetKernelArg                    jocl_clSetKernelArg                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetKernelInfo                   jocl_clGetKernelInfo                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetKernelArgInfo                jocl_clGetKernelArgInfo                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetKernelWorkGroupInfo          jocl_clGetKernelWorkGroupInfo          CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clWaitForEvents                   jocl_clWaitForEvents                   CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetEventInfo                    jocl_clGetEventInfo                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clCreateUserEvent                 jocl_clCreateUserEvent                 CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clRetainEvent                     jocl_clRetainEvent                     CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clReleaseEvent                    jocl_clReleaseEvent                    CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clSetUserEventStatus              jocl_clSetUserEventStatus              CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clSetEventCallback                jocl_clSetEventCallback                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clGetEventProfilingInfo           jocl_clGetEventProfilingInfo           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clFlush                           jocl_clFlush                           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clFinish                          jocl_clFinish                          CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueReadBuffer               jocl_clEnqueueReadBuffer               CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueReadBufferRect           jocl_clEnqueueReadBufferRect           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueWriteBuffer              jocl_clEnqueueWriteBuffer              CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueWriteBufferRect          jocl_clEnqueueWriteBufferRect          CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueFillBuffer               jocl_clEnqueueFillBuffer               CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueCopyBuffer               jocl_clEnqueueCopyBuffer               CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueCopyBufferRect           jocl_clEnqueueCopyBufferRect           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueReadImage                jocl_clEnqueueReadImage                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueWriteImage               jocl_clEnqueueWriteImage               CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueFillImage                jocl_clEnqueueFillImage                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueCopyImage                jocl_clEnqueueCopyImage                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueCopyImageToBuffer        jocl_clEnqueueCopyImageToBuffer        CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueCopyBufferToImage        jocl_clEnqueueCopyBufferToImage        CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueMapBuffer                jocl_clEnqueueMapBuffer                CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueMapImage                 jocl_clEnqueueMapImage                 CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueUnmapMemObject           jocl_clEnqueueUnmapMemObject           CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueMigrateMemObjects        jocl_clEnqueueMigrateMemObjects        CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueNDRangeKernel            jocl_clEnqueueNDRangeKernel            CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueTask                     jocl_clEnqueueTask                     CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueNativeKernel             jocl_clEnqueueNativeKernel             CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueMarkerWithWaitList       jocl_clEnqueueMarkerWithWaitList       CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clEnqueueBarrierWithWaitList      jocl_clEnqueueBarrierWithWaitList      CL_LOADING_SUFFIX;
CL_LOADING_PREFIX h_clSetPrintfCallback               jocl_clSetPrintfCallback               CL_LOADING_SUFFIX;

#undef CL_LOADING_PREFIX
#undef CL_LOADING_SUFFIX


/*
 * Definition run_data structure of kernels.
 * Forward declaration the OpenCL initialization API.
 * Please check joclinit.c for the specific features of these functions.
 */

typedef struct
{
  cl_program program;
  cl_kernel* kernel;
  size_t*    work_group_size;
} JOCL_CL_RUNDATA;


const char*      jocl_cl_errstring                 (cl_int err_code);
cl_bool          jocl_cl_init                      (void);
JOCL_CL_RUNDATA* jocl_cl_compile_and_build         (const char** program_source,
                                                    const char*  kernel_name[]);
cl_bool          jocl_cl_is_support_opencl         (void);
cl_bool          jocl_cl_is_available              (void);
cl_platform_id   jocl_cl_get_platform              (void);
cl_device_id     jocl_cl_get_device                (void);
cl_context       jocl_cl_get_context               (void);
cl_command_queue jocl_cl_get_command_queue         (void);
void             jocl_cl_set_opencl_failure        (void);
void             jocl_cl_set_opencl_success        (void);
void             jocl_cl_set_opencl_support_failure(void);
cl_bool          jocl_cl_is_opencl_decompress      (j_decompress_ptr cinfo);
cl_bool          jocl_cl_is_nvidia_opencl          (void);
void             jocl_cl_set_fancy_status          (void);
cl_bool          jocl_cl_get_fancy_status          (void);

/*
 * The macro definition for exception handling code.
 */

#define CL_DEBUG_NOTE(...)                                               \
  printf(__VA_ARGS__)

#define CL_SAFE_CALL0(func, action)                                      \
  func;                                                                  \
  if(CL_SUCCESS != err_code)                                             \
  {                                                                      \
    CL_DEBUG_NOTE("OpenCL error in %s, Line %u in file %s\nError:%s\n",  \
      #func, __LINE__, __FILE__, jocl_cl_errstring(err_code));           \
    action;                                                              \
  }

#define CL_SAFE_CALL1(func, action, ptr)                                 \
  func;                                                                  \
  if(CL_SUCCESS != err_code)                                             \
  {                                                                      \
    CL_DEBUG_NOTE("OpenCL error in %s, Line %u in file %s\nError:%s\n",  \
      #func, __LINE__, __FILE__, jocl_cl_errstring(err_code));           \
    free(ptr);                                                           \
    action;                                                              \
  }

#define CL_SAFE_CALL2(func, action, ptr1, ptr2)                          \
  func;                                                                  \
  if(CL_SUCCESS != err_code)                                             \
  {                                                                      \
    CL_DEBUG_NOTE("OpenCL error in %s, Line %u in file %s\nError:%s\n",  \
      #func, __LINE__, __FILE__, jocl_cl_errstring(err_code));           \
    free(ptr1);                                                          \
    free(ptr2);                                                          \
    action;                                                              \
  }

#define CL_SAFE_CALL3(func, action, ptr1, ptr2, ptr3)                    \
  func;                                                                  \
  if(CL_SUCCESS != err_code)                                             \
  {                                                                      \
    CL_DEBUG_NOTE("OpenCL error in %s, Line %u in file %s\nError:%s\n",  \
      #func, __LINE__, __FILE__, jocl_cl_errstring(err_code));           \
    free(ptr1);                                                          \
    free(ptr2);                                                          \
    free(ptr3);                                                          \
    action;                                                              \
  }

#define CL_SAFE_CALL4(func, action, ptr1, ptr2, ptr3, ptr4)              \
  func;                                                                  \
  if(CL_SUCCESS != err_code)                                             \
  {                                                                      \
    CL_DEBUG_NOTE("OpenCL error in %s, Line %u in file %s\nError:%s\n",  \
      #func, __LINE__, __FILE__, jocl_cl_errstring(err_code));           \
    free(ptr1);                                                          \
    free(ptr2);                                                          \
    free(ptr3);                                                          \
    free(ptr4);                                                          \
    action;                                                              \
  }


/* The end of joclinit.h */
#endif