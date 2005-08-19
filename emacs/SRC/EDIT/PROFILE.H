/********************************************************************************************************************************/
/* Created  4-NOV-1992 02:17:40 by VAX SDL V3.2-12     Source: 28-FEB-1987 18:24:45 SCOTT_VMS_VAX_SRC_ROOT:[SRC.EDITOR]PROFILE_ST */
/********************************************************************************************************************************/
 
/*** MODULE emacs$profile ***/
/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and nick Emery                               */
/* Definitions for the EMACS profiler                                       */
struct emacs$profile {
    int *histogram;                /* pointer to histogram vector      */
    int total_samples;             /* number of samples taken          */
    int outside_samples;           /* samples outside of the emacs code */
    int histogram_size;            /* size of vector                   */
    int resolution;                /* resolution for profileing        */
    int enabled;                   /* true is profileing is enabled    */
    } ;
