/**
 * @file      ac_tlm_adouble.cpp
 * @author    Felipe Mesquita
 *
 * @brief     Implements a ac_tlm matrix transpose.
 *
 * @attention Copyright (C) 2002-2005 --- The ArchC Team
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *
 */

//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include "ac_tlm_transpose.h"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate lock from ArchC
using user::ac_tlm_transpose;

/// Constructor
ac_tlm_transpose::ac_tlm_transpose( sc_module_name module_name ) :
  sc_module( module_name ),
  target_export("iport")
{
    /// Binds target_export to the matrix transpose
    target_export( *this );

    fpu_reg = new uint8_t[1048575];

    for (int i=0; i < 1048575; i++) fpu_reg[i] = 0;
}

/// Destructor
ac_tlm_transpose::~ac_tlm_adouble() {
  delete [] fpu_reg;
}

/** Read Result
  * @param a is the address to read
  * @param d id the data that will be read
  * @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status ac_tlm_adouble::transpose(long n1, double *src, double *dest, long MyNum, long MyFirst, long MyLast, long pad_length)
{

/* !!! $$$ VARIAVEIS GLOBAIS DO ARQUIVO fft.c QUE ESTAO SENDO UTILIZADAS AQUI MAS QUE PRECISAM SER MAPEADAS PARA CA! NAO SEI COMO IREMOS PASSA-LAS! !!!! $$$$
  
  P  (numero de processadores)
  num_cache_lines


  TB tem q adaptar as leituras e escritas na memoria...estou com duvidas qto a isso assim como tb tenho duvidas a respeito do construtor!

*/
  long i; 
  long j; 
  long k; 
  long l; 
  long m;
  long blksize;
  long numblks;
  long firstfirst;
  long h_off;
  long v_off;
  long v;
  long h;
  long n1p;
  long row_count;

  blksize = MyLast-MyFirst;
  numblks = (2*blksize)/num_cache_lines;
  if (numblks * num_cache_lines != 2 * blksize) {
    numblks ++;
  }
  blksize = blksize / numblks;
  firstfirst = MyFirst;
  row_count = n1/P;
  n1p = n1+pad_length;
  for (l=MyNum+1;l<P;l++) {
    v_off = l*row_count;
    for (k=0; k<numblks; k++) {
      h_off = firstfirst;
      for (m=0; m<numblks; m++) {
        for (i=0; i<blksize; i++) {
	  v = v_off + i;
          for (j=0; j<blksize; j++) {
	    h = h_off + j;
            dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
            dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
          }
        }
	h_off += blksize;
      }
      v_off+=blksize;
    }
  } 

  for (l=0;l<MyNum;l++) {
    v_off = l*row_count;
    for (k=0; k<numblks; k++) {
      h_off = firstfirst;
      for (m=0; m<numblks; m++) {
        for (i=0; i<blksize; i++) {
	  v = v_off + i;
          for (j=0; j<blksize; j++) {
            h = h_off + j;
            dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
            dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
          }
        }
	h_off += blksize;
      }
      v_off+=blksize;
    }
  }

  v_off = MyNum*row_count;
  for (k=0; k<numblks; k++) {
    h_off = firstfirst;
    for (m=0; m<numblks; m++) {
      for (i=0; i<blksize; i++) {
        v = v_off + i;
        for (j=0; j<blksize; j++) {
          h = h_off + j;
          dest[2*(h*n1p+v)] = src[2*(v*n1p+h)];
          dest[2*(h*n1p+v)+1] = src[2*(v*n1p+h)+1];
	}
      }
      h_off += blksize;
    }
    v_off+=blksize;
  }

  return SUCCESS;
}



