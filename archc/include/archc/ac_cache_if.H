/**
 * @file      ac_cache_if.H
 * @author    The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:33:20 -0300
 *
 * @brief     
 *
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _AC_CACHE_IF_H
#define _AC_CACHE_IF_H

#include "ac_parms.H"

class ac_cache_if {
public:
  virtual void request_block(ac_cache_if* client, unsigned address, unsigned size_bytes) = 0;
  virtual void request_write_byte(ac_cache_if* client, unsigned address, unsigned char datum) = 0;
  virtual void request_write_half(ac_cache_if* client, unsigned address, unsigned short datum) = 0;
  virtual void request_write(ac_cache_if* client, unsigned address, ac_word datum) = 0;
  virtual void request_write_block(ac_cache_if* client, unsigned address, char* datum, unsigned size_bytes) = 0;

  virtual void response_block(char* block) = 0;
  virtual void response_write_byte() = 0;
  virtual void response_write_half() = 0;
  virtual void response_write() = 0;
  virtual void response_write_block() = 0;

  virtual void bindToNext(ac_cache_if& next) = 0;
  virtual void bindToPrevious(ac_cache_if& previous) = 0;
  

};

void bind(ac_cache_if& previous, ac_cache_if& next);

#endif
