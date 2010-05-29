/**
 * @file      ac_tlm_router.cpp
 * @author    Gustavo Solaira
 *
 * @brief     Implements a ac_tlm router.
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

#include "ac_tlm_router.h"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate lock from ArchC
using user::ac_tlm_router;

/// Constructor
ac_tlm_router::ac_tlm_router( sc_module_name module_name ) :
  sc_module( module_name ),
  target_export1("iport1"),
  target_export2("iport2"),
  target_export3("iport3"),
  target_export4("iport4"),
  target_export5("iport5"),
  target_export6("iport6"),
  target_export7("iport7"),
  target_export8("iport8"),
  R_port_mem("R_port_mem", 5242880U),
  R_port_lock("R_port_lock", 32U)
{
    /// Binds target_export to the router
    target_export1( *this );
    target_export2( *this );
    target_export3( *this );
    target_export4( *this );
    target_export5( *this );
    target_export6( *this );
    target_export7( *this );
    target_export8( *this );

}

/// Destructor
ac_tlm_router::~ac_tlm_router() {
}

/** Route the request to one of the peripherals
  * @param request is the request received
  * @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp ac_tlm_router::route( const ac_tlm_req &request )
{
  if (request.addr < LOCK_BASE)
  {
    // Route to mem
    return R_port_mem->transport( request );
  }
  else if (request.addr < MDOUBLE_BASE)
  {
    // Route to Read&Inc register
    ac_tlm_req req_aux = request;
    req_aux.addr -= LOCK_BASE;
    return R_port_lock->transport( req_aux );
  }
  
}



