//////////////////////////////////////////////////////////////////////////////
// Standard includes
// SystemC includes
// ArchC includes

#include "fft_fpu.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////////

/// Namespace to isolate memory from ArchC
using user::fft_fpu;

/// Constructor
fft_fpu::fft_fpu( sc_module_name module_name ) :
  sc_module( module_name ),
  target_export("iport")
{
    /// Binds target_export to the memory
    target_export( *this );

}

/// Destructor
fft_fpu::~fft_fpu() {

}

/** Internal Write
  * Note: Always write 32 bits
  * @param a is the address to write
  * @param d id the data being write
  * @returns A TLM response packet with SUCCESS
*/
ac_tlm_rsp_status fft_fpu::write( const uint32_t &a , const uint32_t &d )
{

    
  switch(*((uint32_t *)&a)){
    
    //mult
    case 0:
      *((uint32_t *)&op1_m[1]) = *((uint32_t *)&d);
      break;
    case 4:
      *((uint32_t *)&op1_m[0]) = *((uint32_t *)&d);
      break;
    case 8:
      *((uint32_t *)&op2_m[1]) = *((uint32_t *)&d);
      break;
    case 12:
      *((uint32_t *)&op2_m[0]) = *((uint32_t *)&d);
      break;
    
    //soma
    case 24:
      *((uint32_t *)&op1_s[1]) = *((uint32_t *)&d);
      break;
    case 28:
      *((uint32_t *)&op1_s[0]) = *((uint32_t *)&d);
      break;
    case 32:
      *((uint32_t *)&op2_s[1]) = *((uint32_t *)&d);
      break;
    case 36:
      *((uint32_t *)&op2_s[0]) = *((uint32_t *)&d);
      break;
      
    //sen()
    case 48:
      *((uint32_t *)&op_sin[1]) = *((uint32_t *)&d);
      break;
    case 52:
      *((uint32_t *)&op_sin[0]) = *((uint32_t *)&d);
      break;
      
    //cos()
    case 64:
      *((uint32_t *)&op_cos[1]) = *((uint32_t *)&d);
      break;
    case 68:
      *((uint32_t *)&op_cos[0]) = *((uint32_t *)&d);
      break;
      
    //div
    case 80:
      *((uint32_t *)&op1_d[1]) = *((uint32_t *)&d);
      break;
    case 84:
      *((uint32_t *)&op1_d[0]) = *((uint32_t *)&d);
      break;
    case 88:
      *((uint32_t *)&op2_d[1]) = *((uint32_t *)&d);
      break;
    case 92:
      *((uint32_t *)&op2_d[0]) = *((uint32_t *)&d);
      break;
    
    //sub
    case 104:
      *((uint32_t *)&op1_sub[1]) = *((uint32_t *)&d);
      break;
    case 108:
      *((uint32_t *)&op1_sub[0]) = *((uint32_t *)&d);
      break;
    case 112:
      *((uint32_t *)&op2_sub[1]) = *((uint32_t *)&d);
      break;
    case 116:
      *((uint32_t *)&op2_sub[0]) = *((uint32_t *)&d);
      break;
      

    default:
      return ERROR;
  }
      
  
  return SUCCESS;
}

/** Internal Read
  * Note: Always read 32 bits
  * @param a is the address to read
  * @param d id the data that will be read
  * @returns A TLM response packet with SUCCESS and a modified d
*/
ac_tlm_rsp_status fft_fpu::read( const uint32_t &a , uint32_t &d )
{
  
  //mult
  if( (*((const uint32_t *) &a) == 16) || (*((const uint32_t *) &a) == 20)) {
      
      //operacao
      *((double *) &res_m) = (*((double *) &op1_m)) * (*((double*) &op2_m));
      
	    if(*((const uint32_t *) &a) == 16)
    	  *((uint32_t *)&d) = *((uint32_t *)&res_m[1]);
	
    	if (*((const uint32_t *) &a) == 20)
    	  *((uint32_t *)&d) = *((uint32_t *)&res_m[0]);
  }
      
  //soma
  if( (*((const uint32_t *) &a) == 40) || (*((const uint32_t *) &a) == 44) ){
      
      //operacao
      *((double *) &res_s) = (*((double *) &op1_s)) + (*((double*) &op2_s));
      
	  if(*((const uint32_t *) &a) == 40)
	    *((uint32_t *)&d) = *((uint32_t *)&res_s[1]);
	
  	if(*((const uint32_t *) &a) == 44)
	    *((uint32_t *)&d) = *((uint32_t *)&res_s[0]);
  }
     
  //seno
  if( (*((const uint32_t *) &a) == 56) || (*((const uint32_t *) &a) == 60) ){
      
      //operacao
      *((double *) &res_sin) = sin((*((double *) &op_sin)));       
      
	  if(*((const uint32_t *) &a) == 56)
	    *((uint32_t *)&d) = *((uint32_t *)&res_sin[1]);
	
  	if(*((const uint32_t *) &a) == 60)
  	  *((uint32_t *)&d) = *((uint32_t *)&res_sin[0]);
  }

  //cosseno
  if( (*((const uint32_t *) &a) == 72) || (*((const uint32_t *) &a) == 76) ){
      
      //operacao
      *((double *) &res_cos) = cos( (*((double *) &op_cos)) );  
      
	  if(*((const uint32_t *) &a) == 72)
	    *((uint32_t *)&d) = *((uint32_t *)&res_cos[1]);
	
	  if(*((const uint32_t *) &a) == 76)
	    *((uint32_t *)&d) = *((uint32_t *)&res_cos[0]);
  }

  //div
  if( (*((const uint32_t *) &a) == 96) || (*((const uint32_t *) &a) == 100) ){
      
      //operacao
      *((double *) &res_d) = (*((double *) &op1_d)) / (*((double*) &op2_d));
      
	  if(*((const uint32_t *) &a) == 96)
	    *((uint32_t *)&d) = *((uint32_t *)&res_d[1]);
	
	  if (*((const uint32_t *) &a) == 100)
	    *((uint32_t *)&d) = *((uint32_t *)&res_d[0]);
  }

  //sub
  if( (*((const uint32_t *) &a) == 120) || (*((const uint32_t *) &a) == 124) ){
      
      //operacao
      *((double *) &res_sub) = ( (*((double *) &op1_sub)) - (*((double*) &op2_sub)) );
      
	  if(*((const uint32_t *) &a) == 120)
	    *((uint32_t *)&d) = *((uint32_t *)&res_sub[1]);
	
	  if(*((const uint32_t *) &a) == 124)
	    *((uint32_t *)&d) = *((uint32_t *)&res_sub[0]);
  }
  
  return SUCCESS;
}

