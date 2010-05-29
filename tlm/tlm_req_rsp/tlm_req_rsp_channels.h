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


#ifndef TLM_REQ_RSP_CHANNELS
#define TLM_REQ_RSP_CHANNELS

#include "tlm_adapters/tlm_adapters.h"
#include "tlm_fifo/tlm_fifo.h"
#include "tlm_req_rsp/tlm_put_get_imp.h"

namespace tlm
{

template < typename REQ , typename RSP >
class tlm_req_rsp_channel : public sc_module
{
public:
  // uni-directional slave interface

  sc_export< tlm_fifo_get_if< REQ > > get_request_export;
  sc_export< tlm_fifo_put_if< RSP > > put_response_export;

  // uni-directional master interface

  sc_export< tlm_fifo_put_if< REQ > > put_request_export;
  sc_export< tlm_fifo_get_if< RSP > > get_response_export;

  // master / slave interfaces

  sc_export< tlm_master_if< REQ , RSP > > master_export;
  sc_export< tlm_slave_if< REQ , RSP > > slave_export;


  tlm_req_rsp_channel( int req_size = 1 , int rsp_size = 1 ) :
    sc_module( sc_module_name( sc_gen_unique_name("tlm_req_rsp_channel") ) ) ,
    master( request_fifo , response_fifo ) , 
    slave( request_fifo , response_fifo ) ,
    request_fifo( req_size ) ,
    response_fifo( rsp_size ) {

    bind_exports();
    
  }

  tlm_req_rsp_channel( sc_module_name module_name ,
		       int req_size = 1 , int rsp_size = 1 ) :
    sc_module( module_name  ) ,
    master( request_fifo , response_fifo ) , 
    slave( request_fifo , response_fifo ) ,
    request_fifo( req_size ) ,
    response_fifo( rsp_size ) {

    bind_exports();
    
  }

private:
  void bind_exports() {

    put_request_export( request_fifo );
    get_request_export( request_fifo );
    
    put_response_export( response_fifo );
    get_response_export( response_fifo );

    master_export( master );
    slave_export( slave );

  }

protected:

  tlm_master_imp< REQ , RSP > master;
  tlm_slave_imp< REQ , RSP > slave;

  tlm_fifo<REQ> request_fifo;
  tlm_fifo<RSP> response_fifo;
};

template < typename REQ , typename RSP >
class tlm_transport_channel : public sc_module
{
public:

  // master transport interface

  sc_export< tlm_transport_if< REQ , RSP > > target_export;

  // slave interfaces

  sc_export< tlm_fifo_get_if< REQ > > get_request_export;
  sc_export< tlm_fifo_put_if< RSP > > put_response_export;

  sc_export< tlm_slave_if< REQ , RSP > > slave_export;

  tlm_transport_channel() :
    sc_module( sc_module_name( sc_gen_unique_name("transport_channel" ) ) ) ,
    target_export("target_export") ,
    req_rsp( "req_rsp" , 1 , 1 ) ,
    t2m("ts2m")
  {
    do_binding();
  }

  tlm_transport_channel( sc_module_name nm ) :
    sc_module( nm ) ,
    target_export("target_export") ,
    req_rsp( "req_rsp" , 1 , 1 ) ,
    t2m("tsm" )
  {
    do_binding();
  }

private:
  void do_binding() {

    target_export( t2m.target_export );

    t2m.master_port( req_rsp.master_export );

    get_request_export( req_rsp.get_request_export );
    put_response_export( req_rsp.put_response_export );
    slave_export( req_rsp.slave_export );

  }

  tlm_req_rsp_channel< REQ , RSP > req_rsp;
  tlm_transport_to_master< REQ , RSP > t2m;

};

};
#endif
