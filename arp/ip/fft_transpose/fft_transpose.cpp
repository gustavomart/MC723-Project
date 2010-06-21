/**
 * @file      fft_transpose.cpp
 * @author    Felipe Mesquita
 *
 * @brief     Implements a fft matrix transpose.
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

#include "fft_transpose.h"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate transpose from ArchC
using user::fft_transpose;

/// Constructor
fft_transpose::fft_transpose( sc_module_name module_name ) :
  sc_module( module_name ),
  target_export("iport"),
  R_port_mem("R_port_mem", 5242880U)
{
    /// Binds target_export to the matrix transpose
    target_export( *this );
}

/// Destructor
fft_transpose::~fft_transpose() { }

ac_tlm_rsp_status fft_transpose::write(const uint32_t &a , const uint32_t &d)
{ 
 //sizeof(long) = 4bytes
 //sizeof(double *) = 4bytes
 // NUMERO TOTAL DE PARAMETROS = 9
 // parametros do tipo long = n1, MyNum, MyFirst, MyLast, pad_length, P , num_cache_lines (7)
 // parametros do tipo double = *src, *dest (2)
 //TOTAL em BYTES = (7 * 4) + (2 * 4)= 28 bytes + 8 bytes = 36 bytes 
 
 //intervalos relativos das variaveis em bytes
 //0 - 3 = n1
 //4 - 7 = *src
 //8 - 11 = *dest
 //12 - 15 = MyNum
 //16 - 19 = MyFirst
 //20 - 23 = MyLast
 //24 - 27 = pad_length
 //28 - 31 = P
 //32 - 35 = num_cache_lines
 
   switch(*((uint32_t *)&a)){
    // grava n1
     case 0:
      *((uint32_t *)&_n1) = *((uint32_t *)&d);
      break;
      // grava src
    case 4:
      *((uint32_t *)&_src) = *((uint32_t *)&d);
      break;
      // grava dest
    case 8:
      *((uint32_t *)&_dest) = *((uint32_t *)&d);
      break;
      // grava MyNum
    case 12:
      *((uint32_t *)&_MyNum) = *((uint32_t *)&d);
      break;
      // grava MyFirst
    case 16:
      *((uint32_t *)&_MyFirst) = *((uint32_t *)&d);
      break;
      // grava MyLast
    case 20:
      *((uint32_t *)&_MyLast) = *((uint32_t *)&d);
      break;
      // grava pad_length
    case 24:
      *((uint32_t *)&_pad_length) = *((uint32_t *)&d);
      break;
      // grava P
    case 28:
      *((uint32_t *)&_P) = *((uint32_t *)&d);
      break;
      // grava num_cache_lines
    case 32:
      *((uint32_t *)&_num_cache_lines) = *((uint32_t *)&d);
      break;
    default:
      return ERROR;
  }
  return SUCCESS;
}

double fft_transpose::read_double( double* a )
{
  ac_tlm_req request1, request2;
  uint32_t value[2];
  uint32_t addr = (uint32_t)a;

  // le primeira parte
  request1.addr = addr;
  request1.type = READ;
  ac_tlm_rsp first_part = R_port_mem->transport( request1 );
  *((uint32_t *)&value[1]) = *((uint32_t *) &first_part.data);

  // le segunda parte
  request2.addr = addr+4;
  request2.type = READ;
  ac_tlm_rsp second_part = R_port_mem->transport( request2 );
  *((uint32_t *)&value[0]) = *((uint32_t *) &second_part.data);

  return *(double*)&value;
}

void fft_transpose::write_double( double* a, double d )
{
  ac_tlm_req request1, request2;
  uint32_t addr = (uint32_t)a;
  uint32_t value[2];
  *((double*) value) = *((double *) &d);

  // escreve primeira parte
  request1.addr = addr;
  request1.type = WRITE;
  *((uint32_t *) &request1.data) = *((uint32_t *) &value[1]);
  R_port_mem->transport( request1 );

  // escreve segunda parte
  request2.addr = addr+4;
  request2.type = WRITE;
  *((uint32_t *) &request2.data) = *((uint32_t *) &value[0]);
  R_port_mem->transport( request2 );
}


/** Transpose matrix and save the result on dest.
  * @returns A TLM response packet with SUCCESS and a transpose matrix dest
*/
ac_tlm_rsp_status fft_transpose::read(const uint32_t &a , const uint32_t &d)
{
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
  
  if((*((const uint32_t *) &a) == 36)) 
  {
 
        blksize = (long)(_MyLast-_MyFirst);
        numblks = (2*blksize)/(long)_num_cache_lines;

        if (numblks * (long)_num_cache_lines != 2 * blksize) 
        {
          numblks ++;
        }

        blksize = blksize / numblks;
        firstfirst = (long)_MyFirst;
        row_count = (long)(_n1/_P);
        n1p = (long)(_n1+_pad_length);

        for (l=(long)_MyNum+1;l< (long)_P;l++) 
        {
            v_off = l*row_count;
          
            for (k=0; k<numblks; k++) 
            {
                h_off = firstfirst;
                for (m=0; m<numblks; m++) 
                {
                    for (i=0; i<blksize; i++) 
                    {
                  	    v = v_off + i;
                        for (j=0; j<blksize; j++) 
                        {
                      	    h = h_off + j;
                            // acesso direto a memoria
                            write_double( _dest+(2*(h*n1p+v)), read_double( _src+(2*(v*n1p+h)) ) );
                            write_double( _dest+(2*(h*n1p+v)+1), read_double( _src+(2*(v*n1p+h)+1) ) );
                        }
                    }
	                  
                    h_off += blksize;
                }
                v_off+=blksize;
            }
        }

        for (l=0;l< (long)_MyNum;l++) {
            v_off = l*row_count;
            for (k=0; k<numblks; k++) {
                h_off = firstfirst;
                for (m=0; m<numblks; m++) {
                    for (i=0; i<blksize; i++) {
                    	  v = v_off + i;
                        for (j=0; j<blksize; j++) {
                            h = h_off + j;
                            // acesso direto a memoria
                            write_double( _dest+(2*(h*n1p+v)), read_double( _src+(2*(v*n1p+h)) ) );
                            write_double( _dest+(2*(h*n1p+v)+1), read_double( _src+(2*(v*n1p+h)+1) ) );
                        }
                     }
	                  h_off += blksize;
                }
                v_off+=blksize;
            }
        }

        v_off = (long)_MyNum*row_count;
        for (k=0; k<numblks; k++) 
        {
            h_off = firstfirst;
            for (m=0; m<numblks; m++) 
            {
                for (i=0; i<blksize; i++) 
                {
                    v = v_off + i;
                    for (j=0; j<blksize; j++) 
                    {
                        h = h_off + j;
                        // acesso direto a memoria
                        write_double( _dest+(2*(h*n1p+v)), read_double( _src+(2*(v*n1p+h)) ) );
                        write_double( _dest+(2*(h*n1p+v)+1), read_double( _src+(2*(v*n1p+h)+1) ) );
	                  }
                }
                h_off += blksize;
            }
            v_off+=blksize;
        }

  }

  return SUCCESS;
}



