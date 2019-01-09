/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_error.h                                        */
/* Description:   DG/DirectX errors                                  */
/*                                                                   */
/*********************************************************************/

#ifndef __DX5_ERROR_H
#define __DX5_ERROR_H

#include <ddraw.h>

const char* dx5_error_message(HRESULT);

#define DX5_CHECK_RESULT(hResult) if (FAILED(hResult)) printf(dx5_error_message(hResult));

#endif
