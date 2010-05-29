/**
 * @file      ac_tlm_lock.cpp
 * @author    Gustavo Solaira
 *
 * @brief     Implements a ac_tlm lock.
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

#include "ac_tlm_lock.h"

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate lock from ArchC
using user::ac_tlm_lock;

/// Constructor
ac_tlm_lock::ac_tlm_lock( sc_module_name module_name , int n ) :
  sc_module( module_name ),
  target_export("iport")
{
    /// Binds target_export to the lock
    target_export( *this );

    /// Initialize lock vector
    lock_mem = new uint32_t[ n ];
    for(n=n-1;n>0;n--) lock_mem[n]=0;

}

/// Destructor
ac_tlm_lock::~ac_tlm_lock() {

  delete [] lock_mem;
}

/** Internal Write
  * Note: Always write 32 bits
  * @param a is the address to write
  * @param d id the data being write
  * @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status ac_tlm_lock::writel( const uint32_t &a , const uint32_t &d )
{
  // write value (decrement base memory)
  *((uint32_t *) &lock_mem[a]) = *((uint32_t *) &d);

  return SUCCESS;
}

/** Internal Read
  * Note: Always read 32 bits
  * @param a is the address to read
  * @param d id the data that will be read
  * @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status ac_tlm_lock::readl( const uint32_t &a , uint32_t &d )
{
  // return stored value (decrement base memory)
  *((uint32_t *) &d) = *((uint32_t *) &lock_mem[a]);
  // changes value to one
  lock_mem[a] = 1;

  return SUCCESS;
}



