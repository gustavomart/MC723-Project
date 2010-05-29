
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


#ifndef TLM_HEADER
#define TLM_HEADER

// The TLM interfaces

#include "tlm_interfaces/tlm_core_ifs.h"
#include "tlm_interfaces/tlm_peek_ifs.h"
#include "tlm_interfaces/tlm_master_slave_ifs.h"

// The channels : tlm_fifo, tlm_transport_channel and tlm_req_rsp_channel

#include "tlm_req_rsp/tlm_req_rsp_channels.h"

// Some non blocking ports to provide static sensitivity

#include "tlm_event_finder/tlm_nonblocking_port.h"

#endif
