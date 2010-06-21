/**
 * @file      fft_transpose.h
 * @author    Felipe Mesquita
 *
 *
 * @brief     Defines a fft matrix transposer.
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

#ifndef AC_TLM_transpose_H_
#define AC_TLM_transpose_H_

//////////////////////////////////////////////////////////////////////////////

// Standard includes
// SystemC includes
#include <systemc>
// ArchC includes
#include "ac_tlm_protocol.H"
#include "ac_tlm_port.H"

//////////////////////////////////////////////////////////////////////////////

// using statements
using tlm::tlm_transport_if;

//////////////////////////////////////////////////////////////////////////////

//#define DEBUG

/// Namespace to isolate adouble from ArchC
namespace user
{

/// A TLM lock register
class fft_transpose :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
public:
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export;
  /// Internal write
  ac_tlm_rsp_status write(const uint32_t &a , const uint32_t &d);
  ac_tlm_rsp_status read(const uint32_t &a , const uint32_t &d);

  ac_tlm_port R_port_mem;

  /**
   * Implementation of TLM transport method that
   * handle packets of the protocol doing apropriate actions.
   * This method must be implemented (required by SystemC TLM).
   * @param request is a received request packet
   * @return A response packet to be send
  */
  ac_tlm_rsp transport( const ac_tlm_req &request ) {

    ac_tlm_rsp response;

    switch( request.type ) {
    case READ :     // Packet is a READ one
      #ifdef DEBUG  // Turn it on to print transport level messages
    cout << "Transport READ at 0x" << hex << request.addr << " value ";
    cout << response.data << endl;
      #endif
      response.status = read( request.addr , response.data );
      break;
    case WRITE:     // Packet is a WRITE
      #ifdef DEBUG
    cout << "Transport WRITE at 0x" << hex << request.addr << " value ";
    cout << request.data << endl;
      #endif
      response.status = write( request.addr , request.data );
      break;
    default :
      response.status = ERROR;
      break;
    }

    return response;

  }


  /**
   * Default constructor.
   *
   */
  fft_transpose( sc_module_name module_name );

  /**
   * Default destructor.
   */
  ~fft_transpose();

private:
  // matrix transpose result
//   double *matrix_transpose;
  long _MyNum; 
  long _n1[4]; 
  double* _src[4]; 
  double* _dest[4]; 
  long _MyFirst[4]; 
  long _MyLast[4]; 
  long _pad_length[4];
  long _P[4];
  long _num_cache_lines[4];

  double read_double( double* a );
  void write_double( double* a, double d );
  void Transpose(long n1, double *src, double *dest, long MyNum, long MyFirst, long MyLast, long pad_length, long num_cache_lines, long P);

};

};

#endif //AC_TLM_transpose_H_
