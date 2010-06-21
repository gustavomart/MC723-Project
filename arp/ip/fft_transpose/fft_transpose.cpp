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
      // grava MyNum
    case 0:
      *((uint32_t *)&_MyNum) = *((uint32_t *)&d);
      break;
      // grava n1
    case 4:
      *((uint32_t *)&_n1[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava src
    case 8:
      *((uint32_t *)&_src[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava dest
    case 12:
      *((uint32_t *)&_dest[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava MyFirst
    case 16:
      *((uint32_t *)&_MyFirst[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava MyLast
    case 20:
      *((uint32_t *)&_MyLast[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava pad_length
    case 24:
      *((uint32_t *)&_pad_length[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava P
    case 28:
      *((uint32_t *)&_P[_MyNum]) = *((uint32_t *)&d);
      break;
      // grava num_cache_lines
    case 32:
      *((uint32_t *)&_num_cache_lines[_MyNum]) = *((uint32_t *)&d);
      break;
    default:
      return ERROR;
  }
  return SUCCESS;
}

/** Transpose matrix and save the result on dest.
  * @returns A TLM response packet with SUCCESS and a transpose matrix dest
*/
ac_tlm_rsp_status fft_transpose::read(const uint32_t &a , const uint32_t &d)
{
  
  if((*((const uint32_t *) &a) >= 36)) 
  {
      int pos = (a-36)/4;
      Transpose(_n1[pos], _src[pos], _dest[pos], pos, _MyFirst[pos], _MyLast[pos], _pad_length[pos], _num_cache_lines[pos], _P[pos]);
  }

  return SUCCESS;
}

void fft_transpose::Transpose(long n1, double *src, double *dest, long MyNum, long MyFirst, long MyLast, long pad_length, long num_cache_lines, long P)
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
 
        blksize = (long)(MyLast-MyFirst);
        numblks = (2*blksize)/(long)num_cache_lines;

        if (numblks * (long)num_cache_lines != 2 * blksize) 
        {
          numblks ++;
        }

        blksize = blksize / numblks;
        firstfirst = (long)MyFirst;
        row_count = (long)(n1/P);
        n1p = (long)(n1+pad_length);

        for (l=(long)MyNum+1;l< (long)P;l++) 
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
                            write_double( dest+(2*(h*n1p+v)), read_double( src+(2*(v*n1p+h)) ) );
                            write_double( dest+(2*(h*n1p+v)+1), read_double( src+(2*(v*n1p+h)+1) ) );
                        }
                    }
	                  
                    h_off += blksize;
                }
                v_off+=blksize;
            }
        }

        for (l=0;l< (long)MyNum;l++) {
            v_off = l*row_count;
            for (k=0; k<numblks; k++) {
                h_off = firstfirst;
                for (m=0; m<numblks; m++) {
                    for (i=0; i<blksize; i++) {
                    	  v = v_off + i;
                        for (j=0; j<blksize; j++) {
                            h = h_off + j;
                            // acesso direto a memoria
                            write_double( dest+(2*(h*n1p+v)), read_double( src+(2*(v*n1p+h)) ) );
                            write_double( dest+(2*(h*n1p+v)+1), read_double( src+(2*(v*n1p+h)+1) ) );
                        }
                     }
	                  h_off += blksize;
                }
                v_off+=blksize;
            }
        }

        v_off = (long)MyNum*row_count;
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
                        write_double( dest+(2*(h*n1p+v)), read_double( src+(2*(v*n1p+h)) ) );
                        write_double( dest+(2*(h*n1p+v)+1), read_double( src+(2*(v*n1p+h)+1) ) );
	                  }
                }
                h_off += blksize;
            }
            v_off+=blksize;
        }


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


