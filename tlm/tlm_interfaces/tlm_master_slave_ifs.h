

/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2004 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

//
// Note to the LRM writer : These interfaces are channel specific interfaces
// useful in the context of tlm_req_rsp_channel and tlm_transport_channel.
//
// They inherit their beta status from tlm_get_peek_if.
//

#ifndef TLM_MASTER_SLAVE_IFS_HEADER
#define TLM_MASTER_SLAVE_IFS_HEADER

// 
// req/rsp combined interfaces
//

#include "tlm_interfaces/tlm_peek_ifs.h"

namespace tlm
{

template < typename REQ , typename RSP >
class tlm_master_if : 
  public virtual tlm_put_if< REQ > ,
  public virtual tlm_get_peek_if< RSP > {};

template < typename REQ , typename RSP >
class tlm_slave_if : 
  public virtual tlm_put_if< RSP > ,
  public virtual tlm_get_peek_if< REQ > {};

};
#endif
