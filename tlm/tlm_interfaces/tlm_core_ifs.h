
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
// Note to the LRM writer : This is the core of the TLM standard
//


#ifndef TLM_CORE_IFS_HEADER
#define TLM_CORE_IFS_HEADER

#include <systemc.h>

#include "tlm_interfaces/tlm_tag.h"

namespace tlm 
{

// bidirectional blocking interfaces

template < typename REQ , typename RSP >
class tlm_transport_if : public virtual sc_interface
{
public:
  virtual RSP transport( const REQ & ) = 0;

};

// uni-directional blocking interfaces

template < typename T >
class tlm_blocking_get_if : public virtual sc_interface
{
public:
  virtual T get( tlm_tag<T> *t = 0 ) = 0;
  virtual void get( T &t ) { t = get(); }

};

template < typename T >
class tlm_blocking_put_if : public virtual sc_interface
{
public:
  virtual void put( const T &t ) = 0;
};

// uni-directional non blocking interfaces

template < typename T >
class tlm_nonblocking_get_if : public virtual sc_interface
{
public:
  virtual bool nb_get( T &t ) = 0;
  virtual bool nb_can_get( tlm_tag<T> *t = 0 ) const = 0;
  virtual const sc_event &ok_to_get( tlm_tag<T> *t = 0 ) const = 0;
};

template < typename T >
class tlm_nonblocking_put_if : public virtual sc_interface
{
public:
  virtual bool nb_put( const T &t ) = 0;
  virtual bool nb_can_put( tlm_tag<T> *t = 0 ) const = 0;
  virtual const sc_event &ok_to_put( tlm_tag<T> *t = 0 ) const = 0;
};

// combined uni-directional blocking and non blocking 

template < typename T >
class tlm_get_if :
  public virtual tlm_blocking_get_if< T > ,
  public virtual tlm_nonblocking_get_if< T > {};

template < typename T >
class tlm_put_if :
  public virtual tlm_blocking_put_if< T > ,
  public virtual tlm_nonblocking_put_if< T > {};

};
#endif
