/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg.h                                               */
/* Description:   Direct GUI main interface                          */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_H
#define __DG_H

#ifndef DG_API
/*!	
* Defines API exportation type
*/
#define DG_API extern
#endif

/*!	
* Makes long from two short values 
*/
#define DG_MAKE_LONG(l, h) (unsigned long)((l & 0xFFFF)|(unsigned long)((h)<<16))

/*!	
* Returns the low short value from a long 
*/
#define DG_WORD_LO(l)        (short)(((l) & 0xFFFF))

/*!	
* Returns the high short value from a long 
*/
#define DG_WORD_HI(l)        (short)(((l) >> 16))

/*!	
* Returns the greater between two values 
*/
#define MAX(a, b)            (a)>(b)?(a):(b)

/*!	
* Returns the lower between two values 
*/
#define MIN(a, b)            (a)>(b)?(b):(a)

#endif
