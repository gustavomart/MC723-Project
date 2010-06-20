/**
 * @file      ac_tlm_router.h
 * @author    Gustavo Solaira
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

#ifndef AC_TLM_ROUTER_H_
#define AC_TLM_ROUTER_H_

//////////////////////////////////////////////////////////////////////////////

// Standard includes
// SystemC includes
#include <systemc>
// ArchC includes
#include "ac_tlm_protocol.H"
#include "ac_tlm_port.H"

#define MEM_BASE 0x000000
#define LOCK_BASE 0x500000
#define FPU_BASE 0x600000
#define FFT1D_BASE 0x700000
#define TRANSPOSE_BASE 0x800000

//////////////////////////////////////////////////////////////////////////////

// using statements
using tlm::tlm_transport_if;

//////////////////////////////////////////////////////////////////////////////

//#define DEBUG

/// Namespace to isolate memory from ArchC
namespace user
{

/// A TLM router
class ac_tlm_router :
  public sc_module,
  public ac_tlm_transport_if
{
public:
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export1;
  sc_export< ac_tlm_transport_if > target_export2;
  sc_export< ac_tlm_transport_if > target_export3;
  sc_export< ac_tlm_transport_if > target_export4;

  ac_tlm_port R_port_mem;
  ac_tlm_port R_port_lock;
  ac_tlm_port R_port_fft1d;
  ac_tlm_port R_port_fpu;

  ac_tlm_rsp route( const ac_tlm_req &request );

  /**
   * Implementation of TLM transport method that
   * handle packets of the protocol doing apropriate actions.
   * This method must be implemented (required by SystemC TLM).
   * @param request is a received request packet
   * @return A response packet to be send
  */
  ac_tlm_rsp transport( const ac_tlm_req &request ) {

    ac_tlm_rsp response;

    #ifdef DEBUG  // Turn it on to print transport level messages
    cout << "Transport at 0x" << hex << request.addr << " value ";
    cout << response.data << endl;
    #endif
    
    return route( request );
  }


  /**
   * Default constructor.
   *
   */
  ac_tlm_router( sc_module_name module_name );

  /**
   * Default destructor.
   */
  ~ac_tlm_router();

};

};

#endif //AC_TLM_ROUTER_H_
