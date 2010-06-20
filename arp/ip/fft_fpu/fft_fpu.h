//////////////////////////////////////////////////////////////////////////////

#ifndef AC_TLM_FPU_H_
#define AC_TLM_FPU_H_

//////////////////////////////////////////////////////////////////////////////

// Standard includes
// SystemC includes
#include <systemc>
// ArchC includes
#include "ac_tlm_protocol.H"

//////////////////////////////////////////////////////////////////////////////

// using statements
using tlm::tlm_transport_if;

//////////////////////////////////////////////////////////////////////////////

//#define DEBUG

/// Namespace to isolate memory from ArchC
namespace user
{

/// A TLM memory
class fft_fpu :
  public sc_module,
  public ac_tlm_transport_if // Using ArchC TLM protocol
{
public:
  /// Exposed port with ArchC interface
  sc_export< ac_tlm_transport_if > target_export;
  /// Internal write
  ac_tlm_rsp_status write( const uint32_t & , const uint32_t & );
  /// Internal read
  ac_tlm_rsp_status read( const uint32_t & , uint32_t &);

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
      response.status = read( request.addr , response.data );
      break;
    case WRITE:     // Packet is a WRITE
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
   * @param k Memory size in kilowords.
   *
   */
  fft_fpu( sc_module_name module_name );

  /**
   * Default destructor.
   */
  ~fft_fpu();

private:
  uint32_t *op1_s[2];
  uint32_t *op2_s[2];
  uint32_t *res_s[2];

  uint32_t *op1_m[2];
  uint32_t *op2_m[2];
  uint32_t *res_m[2];
  
  uint32_t *op_sin[2];
  uint32_t *res_sin[2];
  
  uint32_t *op_cos[2];
  uint32_t *res_cos[2];
  
  uint32_t *op1_sub[2];
  uint32_t *op2_sub[2];
  uint32_t *res_sub[2];

  uint32_t *op1_d[2];
  uint32_t *op2_d[2];
  uint32_t *res_d[2];
  
};

};

#endif //AC_TLM_MEM_H_
