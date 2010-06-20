/**
 * @file      fft_fft1d.cpp
 * @author    Gustavo Solaira
 *
 * @brief     Implements a fft fft1donce peripheral.
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

#include "fft_fft1d.h"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate fft1d from ArchC
using user::fft_fft1d;

/// Constructor
fft_fft1d::fft_fft1d( sc_module_name module_name ) :
  sc_module( module_name ),
  target_export("iport"),
  R_port_mem("R_port_mem", 5242880U)
{
    /// Binds target_export to the fft1donce
    target_export( *this );
}

/// Destructor
fft_fft1d::~fft_fft1d() { }

/** Write the operands and perform calculation after wrote the second
  * Note: Always write 32 bits
  * @param a is the address to write
  * @param d id the data being write
  * @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status fft_fft1d::write( const uint32_t &a , const uint32_t &d )
{
  switch(a) {
    case 0:
      *((uint32_t *) &MyNum) = *((uint32_t *) &d);
    case 4:
      *((uint32_t *) &zdirection[MyNum]) = *((uint32_t *) &d);
      break;
    case 8:
      *((uint32_t *) &zM[MyNum]) = *((uint32_t *) &d);
      break;
    case 12:
      *((uint32_t *) &zN[MyNum]) = *((uint32_t *) &d);
      break;
    case 16:
      *((uint32_t *) &vet_u[MyNum]) = *((uint32_t *) &d);
      break;
    case 20:
      *((uint32_t *) &vet_x[MyNum]) = *((uint32_t *) &d);
      break;
  }
}

/** Read Result
  * @param a is the address to read
  * @param d id the data that will be read
  * @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status fft_fft1d::read( const uint32_t &a , uint32_t &d )
{
  printf("FFT1DOnce: %d %d %d %d %p %p\n", a/4, zdirection[a/4], zM[a/4], zN[a/4], vet_u[a/4], vet_x[a/4]);

  FFT1DOnce(zdirection[a/4], zM[a/4], zN[a/4], vet_u[a/4], vet_x[a/4]);

  return SUCCESS;
}

void fft_fft1d::swap_endian(uint32_t *a, uint32_t *b)
{
  uint32_t swap = *a;
  *a = *b;
  *b = swap;
}

double fft_fft1d::read_double( double* a )
{
  ac_tlm_req request;
  request.type = READ;
  uint32_t value[2];

  // le primeira parte
  request.addr = (uint32_t)a;
  ac_tlm_rsp first_part = R_port_mem->transport( request );
  value[1] = first_part.data;

  // le segunda parte
  request.addr = (uint32_t)a+4;
  ac_tlm_rsp second_part = R_port_mem->transport( request );
  value[0] = second_part.data;

  return *(double*)&value;
}

void fft_fft1d::write_double( double* a, double d )
{
  ac_tlm_req request;
  request.type = WRITE;
  uint32_t *value = (uint32_t*)&d;

  // escreve primeira parte
  request.addr = (uint32_t)a;
  request.data = value[1];
  R_port_mem->transport( request );

  // escreve segunda parte
  request.addr = (uint32_t)a+4;
  request.data = value[0];
  R_port_mem->transport( request );
}

long fft_fft1d::BitReverse(long M, long k)
{
  long i; 
  long j; 
  long tmp;

  j = 0;
  tmp = k;
  for (i=0; i<M; i++) {
    j = 2*j + (tmp&0x1);
    tmp = tmp>>1;
  }
  return(j);
}

void fft_fft1d::Reverse(long N, long M, double *x)
{
  long j, k;
  double a, b, aux;

  for (k=0; k<N; k++) {
    j = BitReverse(M, k);
    if (j > k) {

      a = read_double(x+2*j);
      b = read_double(x+2*k);
      write_double(x+2*j, b);
      write_double(x+2*k, a);   

      a = read_double(x+2*j+1);
      b = read_double(x+2*k+1);
      write_double(x+(2*j+1), b);
      write_double(x+(2*k+1), a);
    }
  }
}

void fft_fft1d::FFT1DOnce(long direction, long M, long N, double *u, double *x)
{
  long j; 
  long k; 
  long q; 
  long L; 
  long r; 
  long Lstar;
  double *u1; 
  double *x1; 
  double *x2;
  double omega_r; 
  double omega_c; 
  double tau_r; 
  double tau_c; 
  double x_r; 
  double x_c;

  Reverse(N, M, x);

  for (q=1; q<=M; q++) {
    L = 1<<q; r = N/L; Lstar = L/2;
    u1 = (double*)(u+2*(Lstar-1)*8);
    for (k=0; k<r; k++) {
      x1 = (double*)(x+2*(k*L)*8);
      x2 = (double*)(x+2*(k*L+Lstar)*8);
      for (j=0; j<Lstar; j++) {
	      omega_r = read_double(u1+2*j); 
        omega_c = direction * read_double(u1+2*j+1);
	      x_r = read_double(x2+2*j);
        x_c = read_double(x2+2*j+1);
	      tau_r = omega_r*x_r - omega_c*x_c;
	      tau_c = omega_r*x_c + omega_c*x_r;
	      x_r = read_double(x1+2*j); 
        x_c = read_double(x1+2*j+1);
	      write_double(x2+2*j, x_r - tau_r);
	      write_double(x2+2*j+1, x_c - tau_c);
	      write_double(x1+2*j, x_r + tau_r);
	      write_double(x1+2*j+1, x_c + tau_c);
      }
    }
  }
}

