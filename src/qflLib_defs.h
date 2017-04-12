/* 
*   File: qflLib_defs.h
*   Purpose: For Quest for Life Camp Registration and Check-in
*	Author: David Zhang
*	Date: April 12, 2017
*
*/

#ifndef _QFLLIB_DEF_H_
#define _QFLLIB_DEF_H_

#ifdef  QFLLIB_EXPORTS
#undef  QFLLIB_EXPORT
#define QFLLIB_EXPORT __declspec( dllexport )
#else // QFLLIB_EXPORT
#define QFLLIB_EXPORT
#endif

#endif // _QFLLIB_DEF_H_
