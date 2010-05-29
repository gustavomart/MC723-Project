/* ex: set tabstop=2 expandtab: 
   -*- Mode: C; tab-width: 2; indent-tabs-mode nil -*-
*/
/* `tc-'___arch_name___`.c' -- Assemble code for ___arch_name___
   Copyright 2005, 2006 --- The ArchC Team
 
   This file is automatically retargeted by ArchC binutils generation 
   tool. This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/*
 * written by:
 *   Alexandro Baldassin            
 */


#include "as.h"
#include "config.h"
#include "subsegs.h"
#include "safe-ctype.h"
#include `"tc-'___arch_name___`.h"'
#include `"elf/'___arch_name___`.h"'
#include `"opcode/'___arch_name___`.h"'
#include `"share-'___arch_name___`.h"'

//#define DEBUG_ON

#ifdef DEBUG_ON
#  include <stdio.h>
   extern int indent_level;

#  define dbg_printf(il, args...) {fprintf(stderr, "%*s+ ", il*4, ""); \
                                  fprintf(stderr, args);}
#  define dbg_print_insn(il, insn)  print_opcode_structure(stderr, il*4, insn)
#  define dbg_print_operand_info(il, opid) print_operand_info(stderr, il*4, opid)
#  define dbg_print_fixup(il, fixup) print_internal_fixup(stderr, il, fixup)
#  define dbg_print_expression(il, exp) {indent_level = il; \
                                     print_expr_1(stderr, exp); \
                                     fprintf(stderr, "\n");}
#  define dbg_print_gasfixup(il, fixP) print_fixup(stderr, il, fixP)
#  define dbg_print_symbolS(il, sym) {indent_level = il; \
                                     print_symbol_value_1(stderr, sym); \
                                     fprintf(stderr, "\n");}
#else 
#  define dbg_printf(il, args...)
#  define dbg_print_insn(il, insn) 
#  define dbg_print_operand_info(il, opid)
#  define dbg_print_fixup(il, fixup)
#  define dbg_print_expression(il, exp)
#  define dbg_print_gasfixup(il, fixP)
#  define dbg_print_symbolS(il, sym)
#endif


#ifdef __STDC__
#define INTERNAL_ERROR() as_fatal (_("Internal error: line %d, %s"), __LINE__, __FILE__)
#else
#define INTERNAL_ERROR() as_fatal (_("Internal error"));
#endif

typedef struct _acfixuptype
{
  expressionS ref_expr_fix; /* symbol in which the relocation points to */
  unsigned int operand_id;

  struct _acfixuptype *next;
} acfixuptype;


static void s_cons(int byte_size);
static void archc_show_information(void);
static void pseudo_assemble(void);
static void emit_insn(void);
static int get_expression(expressionS *, char **);
static int get_immediate(expressionS *ep, char **str);
static int get_address(expressionS *, char **);
static int get_userdefoper(unsigned int *val, const char *opclass, char **str);
static int ac_valid_symbol(const char *ac_symbol, char *parser_symbol, 
                           unsigned int *value);
static int ac_parse_operands(char *s_pos, char *args);
static void create_fixup(unsigned oper_id);
static void strtolower(char *str);
static void clean_out_insn(void);
static void free_all_fixups(acfixuptype *fix);
static void print_internal_fixup(FILE *stream, unsigned int il, acfixuptype *fix);
static void print_fixup(FILE *stream, unsigned int il, fixS *fixP);
static void encode_field(unsigned int val_const, unsigned int oper_id);


/*
  variables used by the core
*/
const char comment_chars[] = "#!";  // # - MIPS, ! - SPARC, @ - ARM
const char line_comment_chars[] = "#";
const char line_separator_chars[] = ";";

const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "rRsSfFdDxXpP";
const pseudo_typeS md_pseudo_table[] = 
{ 
  {"half",  s_cons, AC_WORD_SIZE/8 /2},
  {"dword", s_cons, AC_WORD_SIZE/8 *2},
  {"byte",  s_cons, 1},
  {"hword", s_cons, AC_WORD_SIZE/8 /2},
  {"int",   s_cons, AC_WORD_SIZE/8},
  {"long",  s_cons, AC_WORD_SIZE/8},
  {"octa",  s_cons, AC_WORD_SIZE/8 *4},
  {"quad",  s_cons, AC_WORD_SIZE/8 *2},  
  {"short", s_cons, AC_WORD_SIZE/8 /2},
  {"word",  s_cons, AC_WORD_SIZE/8},
  {NULL, 0, 0},
};


typedef unsigned char uc;
static const unsigned char charmap[] = {
	(uc)'\000',(uc)'\001',(uc)'\002',(uc)'\003',(uc)'\004',(uc)'\005',(uc)'\006',(uc)'\007',
	(uc)'\010',(uc)'\011',(uc)'\012',(uc)'\013',(uc)'\014',(uc)'\015',(uc)'\016',(uc)'\017',
	(uc)'\020',(uc)'\021',(uc)'\022',(uc)'\023',(uc)'\024',(uc)'\025',(uc)'\026',(uc)'\027',
	(uc)'\030',(uc)'\031',(uc)'\032',(uc)'\033',(uc)'\034',(uc)'\035',(uc)'\036',(uc)'\037',
	(uc)'\040',(uc)'\041',(uc)'\042',(uc)'\043',(uc)'\044',(uc)'\045',(uc)'\046',(uc)'\047',
	(uc)'\050',(uc)'\051',(uc)'\052',(uc)'\053',(uc)'\054',(uc)'\055',(uc)'\056',(uc)'\057',
	(uc)'\060',(uc)'\061',(uc)'\062',(uc)'\063',(uc)'\064',(uc)'\065',(uc)'\066',(uc)'\067',
	(uc)'\070',(uc)'\071',(uc)'\072',(uc)'\073',(uc)'\074',(uc)'\075',(uc)'\076',(uc)'\077',
	(uc)'\100',(uc)'\141',(uc)'\142',(uc)'\143',(uc)'\144',(uc)'\145',(uc)'\146',(uc)'\147',
	(uc)'\150',(uc)'\151',(uc)'\152',(uc)'\153',(uc)'\154',(uc)'\155',(uc)'\156',(uc)'\157',
	(uc)'\160',(uc)'\161',(uc)'\162',(uc)'\163',(uc)'\164',(uc)'\165',(uc)'\166',(uc)'\167',
	(uc)'\170',(uc)'\171',(uc)'\172',(uc)'\133',(uc)'\134',(uc)'\135',(uc)'\136',(uc)'\137',
	(uc)'\140',(uc)'\141',(uc)'\142',(uc)'\143',(uc)'\144',(uc)'\145',(uc)'\146',(uc)'\147',
	(uc)'\150',(uc)'\151',(uc)'\152',(uc)'\153',(uc)'\154',(uc)'\155',(uc)'\156',(uc)'\157',
	(uc)'\160',(uc)'\161',(uc)'\162',(uc)'\163',(uc)'\164',(uc)'\165',(uc)'\166',(uc)'\167',
	(uc)'\170',(uc)'\171',(uc)'\172',(uc)'\173',(uc)'\174',(uc)'\175',(uc)'\176',(uc)'\177',
	(uc)'\200',(uc)'\201',(uc)'\202',(uc)'\203',(uc)'\204',(uc)'\205',(uc)'\206',(uc)'\207',
	(uc)'\210',(uc)'\211',(uc)'\212',(uc)'\213',(uc)'\214',(uc)'\215',(uc)'\216',(uc)'\217',
	(uc)'\220',(uc)'\221',(uc)'\222',(uc)'\223',(uc)'\224',(uc)'\225',(uc)'\226',(uc)'\227',
	(uc)'\230',(uc)'\231',(uc)'\232',(uc)'\233',(uc)'\234',(uc)'\235',(uc)'\236',(uc)'\237',
	(uc)'\240',(uc)'\241',(uc)'\242',(uc)'\243',(uc)'\244',(uc)'\245',(uc)'\246',(uc)'\247',
	(uc)'\250',(uc)'\251',(uc)'\252',(uc)'\253',(uc)'\254',(uc)'\255',(uc)'\256',(uc)'\257',
	(uc)'\260',(uc)'\261',(uc)'\262',(uc)'\263',(uc)'\264',(uc)'\265',(uc)'\266',(uc)'\267',
	(uc)'\270',(uc)'\271',(uc)'\272',(uc)'\273',(uc)'\274',(uc)'\275',(uc)'\276',(uc)'\277',
	(uc)'\300',(uc)'\341',(uc)'\342',(uc)'\343',(uc)'\344',(uc)'\345',(uc)'\346',(uc)'\347',
	(uc)'\350',(uc)'\351',(uc)'\352',(uc)'\353',(uc)'\354',(uc)'\355',(uc)'\356',(uc)'\357',
	(uc)'\360',(uc)'\361',(uc)'\362',(uc)'\363',(uc)'\364',(uc)'\365',(uc)'\366',(uc)'\367',
	(uc)'\370',(uc)'\371',(uc)'\372',(uc)'\333',(uc)'\334',(uc)'\335',(uc)'\336',(uc)'\337',
	(uc)'\340',(uc)'\341',(uc)'\342',(uc)'\343',(uc)'\344',(uc)'\345',(uc)'\346',(uc)'\347',
	(uc)'\350',(uc)'\351',(uc)'\352',(uc)'\353',(uc)'\354',(uc)'\355',(uc)'\356',(uc)'\357',
	(uc)'\360',(uc)'\361',(uc)'\362',(uc)'\363',(uc)'\364',(uc)'\365',(uc)'\366',(uc)'\367',
	(uc)'\370',(uc)'\371',(uc)'\372',(uc)'\373',(uc)'\374',(uc)'\375',(uc)'\376',(uc)'\377',
};

static int insensitive_symbols = 0;
static int sensitive_mno = 0;

/* hash tables: symbol and opcode */
static struct hash_control *sym_hash = NULL;
static struct hash_control *op_hash = NULL;

/* assuming max input = 64 */
static int log_table[] = {  0 /*invalid*/,  0, 1, 1, 
                            2 /* log 4 */,  2, 2, 2, 
                            3 /* log 8 */,  3, 3, 3,
                            3,              3, 3, 3,
                            4 /* log 16 */, 4, 4, 4};


/* every error msg due to a parser error is stored here 
   NULL implies no error has occured  */
static char *insn_error;

/* a flag variable - if set, the parser is in 'imm' mode, that is, getting an
   operand of type imm */
static int in_imm_mode = 0;

/* if an operand is of type expr, it will be saved here */
static expressionS ref_expr;


/* out_insn is the current instruction being assembled */
static struct insn_encoding
{
  unsigned int image;
  unsigned int format;

  unsigned int is_pseudo;
  unsigned int op_num;
  unsigned int op_pos[9*2];

  acfixuptype *fixup;
} out_insn;




/*
  code
*/


void 
md_begin()
{
  int i;
  const char *retval;

  op_hash = hash_new();
  for (i = 0; i < num_opcodes;) {
    const char *t_mnemonic = opcodes[i].mnemonic;

    retval = hash_insert(op_hash, t_mnemonic, (PTR) &opcodes[i]);

    if (retval != NULL) {
      fprintf (stderr, _("internal error: can't hash '%s': %s\n"),
         opcodes[i].mnemonic, retval);
      as_fatal (_("Broken assembler.  No assembly attempted."));
    }

    while ((++i < num_opcodes) && !strcmp(t_mnemonic, opcodes[i].mnemonic));

  }

  /* TODO:
   *  . It not allowed to define the same symbol name in more than one
   *  ac_asm_map section -> remove this restriction
   *  . Desalloc the memory allocated in case of 'insensitive'
   */
  sym_hash = hash_new();
  for (i=0; i < num_symbols; i++) {
    const char *t_symbol = udsymbols[i].symbol;
    char *symtoadd = NULL;

    if (insensitive_symbols) {
      symtoadd  = (char *) malloc(strlen(t_symbol)+1);
      strcpy(symtoadd, t_symbol);
      strtolower(symtoadd);
    }
    else
      symtoadd = (char *)t_symbol;

    retval = hash_insert(sym_hash, (const char *)symtoadd, (PTR) &udsymbols[i]);

    if (retval != NULL) {
      fprintf (stderr, _("internal error: can't hash `%s': %s\n"),
         udsymbols[i].symbol, retval);
      as_fatal (_("Broken assembler.  No assembly attempted."));
    }
  }

  record_alignment(text_section, log_table[AC_WORD_SIZE/8]);

}


#define KEEP_GOING 1
#define STOP_INSN  0

void 
md_assemble(char *str)
{
  dbg_printf(0, "Assembling instruction \"%s\"\n", str);

  static int has_pseudo = 0;

  /* insn we need to encode at this call */
  acasm_opcode *insn = NULL;

  /* we won't change str, so it always point to the start of the insn string.
     's_pos' will be used to parse it instead and points to the position in 
     'str' we are parsing at the moment */
  char *s_pos = str;

  /* pointer to the mnemonic string - 0-ended */
  char *p_mnemonic = sensitive_mno ? original_case_string : str;

  /* if it is a pseudo instruction we always use 'str' because
     'original_case_string' is not filled by pseudo_assemble */
  if (has_pseudo)
    p_mnemonic = str;

  /* a char used to store the value of the char in a string whenever we need 
     to break the original 'str' into substrings adding '\0', so we can
     restore the 'str' to full original string later */
  char save_c = 0;

  /* buffer used to store ArchC argument string and a pointer to scan it */
  char buffer[200];

  /* advance till first space (this indicates we should have got a mnemonic) 
     or end of string (maybe a mnemonic with no operands) */
  while (*s_pos != '\0' && !ISSPACE (*s_pos)) s_pos++;

  /* save the char at s_pos (it might get lost) */
  save_c = *s_pos;

  /* If we stopped on whitespace then replace the whitespace with null for
     the call to hash_find. Note that if we didn't stop on whitespace then
     *s_pos is already '\0': it doesn't hurt, does it? ;)  */
  *s_pos = '\0';

  /* try finding the opcode in the hash table */
  insn = (acasm_opcode *) hash_find (op_hash, p_mnemonic);

  /* if the instruction was not found then abort this insn encoding (it won't
     abort the assembler though, just move to the next one) */
  if (insn == NULL) {
    as_bad (_("unrecognized mnemonic: %s"), p_mnemonic);
    return;
  }

  /* restore original string */
  *s_pos = save_c;

  /* makes s_pos points to the start of the argument string (if there is one) */
  while (ISSPACE(*s_pos)) s_pos++;

  /* main encoding loop */
  for (;;) {
    dbg_printf(1, "opcode = ");
    dbg_print_insn(0, insn);

    strcpy(buffer, insn->args);

    clean_out_insn();

    out_insn.image = insn->image;
    out_insn.format = insn->format_id;
    out_insn.is_pseudo = insn->pseudo_idx;
    insn_error = NULL;

    if (insensitive_symbols) {
      strtolower(s_pos);
      strtolower(buffer);
    }

    dbg_printf(1, "Parsing \"%s\" with \"%s\"\n", s_pos, buffer);  
    
    int go_next = ac_parse_operands(s_pos, buffer); /* parse the operands */

    if (go_next == STOP_INSN)
      break;
    
    /* try the next insn (overload) if the current failed */
    if (insn_error && (insn+1 < &opcodes[num_opcodes]) && 
        (!strcmp(insn->mnemonic, insn[1].mnemonic))) {
      insn++;
      dbg_printf(1, "\n");
      dbg_printf(1, "Trying another opcode. Error: %s\n", insn_error);
      dbg_printf(1, "\n");
    }
    else break;
  } 

  if (insn_error) {
    as_bad(_("%s in insn: '%s'"), insn_error, p_mnemonic);
    return;
  }

  if (insn->pseudo_idx) {
    if (has_pseudo != 0) {
      as_bad(_("Pseudo-insn %s called by another pseudo-insn. Not assembled.'\n"), p_mnemonic);
      return;    
    }
    has_pseudo = 1;
    pseudo_assemble(); /* main code to handle pseudo instruction */
    has_pseudo = 0;
  }
  else 
    emit_insn();  /* save the instruction encoded in the the current frag */
}



/*
  Operands Parser. Actually, it does a semantic job as well.

  args -> points to the beginning of the argument list (from opcode table) 

  s_pos -> points to the beginning of the insn operand string

  The 'args' string comes from the opcode table generated automatically from an
  ArchC model and tells us how we should expect the syntax of the operands of
  an instruction.
*/  
static 
int ac_parse_operands(char *s_pos, char *args)
{
  /* an ArchC symbol value (gas symbols use expressionS) */
  unsigned int symbol_value; 

  /* a buffer to hold the current conversion specifier found in args */
  char op_buf[30]; 
  char *ob = op_buf;

  insn_error = NULL;
  ref_expr.X_op = O_absent;

  /* while argument list is not empty, try parsing the operands */
  while (*args != '\0') { 

    /* a whitespace in the args string means that we need at least one
       whitespace in the s_pos string */
    if (ISSPACE(*args)){
      if (!ISSPACE(*s_pos)) {
        insn_error = "invalid instruction syntax";
        return KEEP_GOING;
      }
      args++;
      s_pos++;
      /* note that it's not possible to have a space as the last char of an
         'args' string, so we dont need to check for '\0' */
    }

    /* eats any space from s_pos so that multiples whitespaces can be handled correctly between
     syntatic elements - the gas pre-processor does this, but we must be sure xD */
    while (ISSPACE(*s_pos)) s_pos++; 

    /* if we've reached the end of insn operand string and not 'args', then
       some operands are missing like: "add" when we were expecting something
       like "add $03,$04,$05" */
    if (*s_pos == '\0') { 
      insn_error = "invalid instruction syntax";
      return KEEP_GOING;
    }

    /*
      '%' is the start of a well-formed sentence. It goes like this:

      '%'<conversion specifier>':'<insn field ID>':<reloc_number>' 
    */
    if (*args == '%') {    
      args++;

      /* save the start of the operand string in case a pseudo is called later */
      out_insn.op_pos[out_insn.op_num++] = (int)s_pos;

      /* ob points to the conversion specifier buffer - null terminated */
      ob = op_buf;
      while (*args != ':') { 
        *ob = *args;
        ob++; args++;
      }
      *ob = '\0';

      args++;                  /* next literal character/operand */

      unsigned int operand_id = atoi(op_buf);

      dbg_printf(2, "Using operand[%d] to parse \"%s\"\n", operand_id, s_pos);
      dbg_printf(3, "operand[%d] = ", operand_id);
      dbg_print_operand_info(0, operand_id);


      if (operand_id >= num_oper_id)
        INTERNAL_ERROR();

    
      switch (operands[operand_id].type) {
 
      /* EXP */ 
        case op_exp:

          if (!get_expression(&ref_expr, &s_pos)) 
            return KEEP_GOING;

          switch (ref_expr.X_op) {
            case O_symbol:  /* X_add_symbol + X_add_number */
              create_fixup(operand_id);
              break;
                
            case O_constant: /* X_add_number */
              encode_field(ref_expr.X_add_number, operand_id);
              break;

            case O_uminus:          /* (- X_add_symbol) + X_add_number.  */
            case O_bit_not:         /* (~ X_add_symbol) + X_add_number.  */
            case O_logical_not:     /* (! X_add_symbol) + X_add_number.  */
            case O_multiply:        /* (X_add_symbol * X_op_symbol) + X_add_number.  */
            case O_divide:          /* (X_add_symbol / X_op_symbol) + X_add_number.  */
            case O_modulus:         /* (X_add_symbol % X_op_symbol) + X_add_number.  */
            case O_left_shift:      /* (X_add_symbol << X_op_symbol) + X_add_number. */
            case O_right_shift:     /* (X_add_symbol >> X_op_symbol) + X_add_number. */
            case O_bit_inclusive_or:/* (X_add_symbol | X_op_symbol) + X_add_number.  */
            case O_bit_or_not:      /* (X_add_symbol |~ X_op_symbol) + X_add_number. */
            case O_bit_exclusive_or:/* (X_add_symbol ^ X_op_symbol) + X_add_number.  */
            case O_bit_and:         /* (X_add_symbol & X_op_symbol) + X_add_number.  */
              insn_error = "operation not supported with relocatable symbol";
              return STOP_INSN;
              break;

           default:
              insn_error = "invalid expression";
              return KEEP_GOING;
          }
          break; 


        case op_imm:

          in_imm_mode = 1;
          if (!get_immediate(&ref_expr, &s_pos)) {
            in_imm_mode = 0;
            return KEEP_GOING;
          }
          in_imm_mode = 0;

          dbg_printf(3, "Immediate value: %ld\n", ref_expr.X_add_number);

          encode_field(ref_expr.X_add_number, operand_id);

          break;

        case op_addr:

          if (!get_address(&ref_expr, &s_pos)) return KEEP_GOING;

          if (ref_expr.X_op != O_symbol) {    // X_add_symbol + X_add_number
            insn_error = "invalid operand, expected a symbolic address";
            return KEEP_GOING;
          }

          create_fixup(operand_id);

          break;

        case op_userdef:

          if (!get_userdefoper(&symbol_value, operands[operand_id].name, &s_pos)) 
            return KEEP_GOING;


        /* NEVER!!! discomment the line below O.O */
        /* if (out_insn.is_pseudo) return; */

        /* if we have reached here, we sucessfully got a valid symbol's value; so we encode it */
          encode_field(symbol_value, operand_id);

          break;
 
        default:
          INTERNAL_ERROR();

      }

      out_insn.op_pos[out_insn.op_num++] = (int)s_pos;

    } /* end of if (*args == '%') */
    else {  
      /* args string must equal s_pos string in case its not a '%' */    

      /* scape */
      if (*args == '\\') {
        args++;   
      }

      if (*s_pos != *args) {  
        insn_error = "invalid instruction syntax";
        return KEEP_GOING;
      }
      args++;
      s_pos++;

    }
    
  } /* end while */

  if (*s_pos != '\0') {  /* have add $3,$2,$2[more] - expected add $3,$2,$2 */
    insn_error = "invalid instruction syntax";
    return KEEP_GOING;
  }

  return KEEP_GOING;
}



static void pseudo_assemble() {

  char op_string[9][50];
  unsigned int i;
  int j;

  /* extract the operand strings */
  for (i=0; i<out_insn.op_num/2; i++) {
     
    int str_count = out_insn.op_pos[i*2+1] - out_insn.op_pos[i*2];
    if (str_count < 0 || str_count >= 50) 
      INTERNAL_ERROR(); /* out buffer is too small */

    for (j=0; j<str_count; j++) 
      op_string[i][j] = *(char *)(out_insn.op_pos[i*2]+j);

    op_string[i][str_count] = '\0';

  }

  /* from now on the code must be reentrant (recursive calls to md_assemble) */
  int num_operands = (unsigned int) out_insn.op_num/2;
  int pseudo_ind = out_insn.is_pseudo;
  if (pseudo_ind > num_pseudo_instrs) INTERNAL_ERROR();

  char new_insn[50];

  while (pseudo_instrs[pseudo_ind] != NULL) {
    const char *pseudoP = pseudo_instrs[pseudo_ind];
    char *n_ind = new_insn;

    while (*pseudoP != '\0') {
      
      if (*pseudoP == '\\') 
         pseudoP++;
      else if (*pseudoP == '%') {
       pseudoP++;
       if ((*pseudoP < '0') || (*pseudoP > '9') || (*pseudoP-'0' >= num_operands)) 
         INTERNAL_ERROR();

       strcpy(n_ind, op_string[*pseudoP-'0']);
       n_ind += strlen(op_string[*pseudoP-'0']);

       pseudoP++;
       continue;
      }

      *n_ind = *pseudoP;
      n_ind++;
      pseudoP++;

    }
    *n_ind = '\0';
      
    md_assemble(new_insn);

    pseudo_ind++;
  }
}


static void emit_insn() 
{
  /* frag address where we emit the insn encoding (call frag_more() to get the address) */
    char *frag_address;
  
  dbg_printf(1, "Emitting image 0x%08lX (%ld bits) at address 0x%08lX\n", 
                out_insn.image, 
                get_insn_size(out_insn.format), 
                frag_now_fix());

  frag_address = frag_more(get_insn_size(out_insn.format)/8);

  md_number_to_chars(frag_address, out_insn.image, get_insn_size(out_insn.format)/8);

  dbg_printf(2, "Frag info: addr = %ld, fix = %ld, var = %ld\n", 
                  frag_now->fr_address,
                  frag_now->fr_fix,
                  frag_now->fr_var);

  while (out_insn.fixup != NULL) {

    fixS *fixP;

    ref_expr = out_insn.fixup->ref_expr_fix;

    if (ref_expr.X_op != O_symbol)
      INTERNAL_ERROR();
    
    fixP = fix_new_exp (frag_now, frag_address - frag_now->fr_literal /* where */, get_insn_size(out_insn.format)/8 /* size */,
    &ref_expr, 0 /* pcrel -> always FALSE */, operands[out_insn.fixup->operand_id].reloc_id);
    
    if (!fixP)
      INTERNAL_ERROR();

    fixP->tc_fix_data = out_insn.fixup->operand_id;

    /* don't let GAS core mess with our fixup (adjust_reloc_syms) */
    fixP->fx_done = TRUE;

    acfixuptype *p = out_insn.fixup;
    out_insn.fixup = out_insn.fixup->next;
    free(p); 

    dbg_printf(1, "gas fixS created:\n");
    dbg_print_gasfixup(2, fixP);
  }

}



/*

  Function used to write 'val' in 'buf' using 'n' bytes with a correct machine endian.
  Usually it's called to emit instructions to a frag.

*/
void
md_number_to_chars(char *buf, valueT val, int n)
{
#ifdef AC_BIG_ENDIAN
    number_to_chars_bigendian (buf, val, n);  
#else
    number_to_chars_littleendian (buf, val, n);
#endif
}



/*

  Called by fixup_segment() (write.c) to apply a fixup to a frag.

  fixP -> a pointer to the fixup
  valP -> a pointer to the value to apply (with symbol value added)
  seg  -> segment which the fix is attached to

*/
void
md_apply_fix3 (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{

  INTERNAL_ERROR(); // should never get called
  
}


int
___arch_name___`_validate_fix'(struct fix *fixP ATTRIBUTE_UNUSED, asection *seg ATTRIBUTE_UNUSED)
{
  dbg_printf(0, "Validating fix\n");
  dbg_print_gasfixup(1, fixP);

  if (fixP->fx_addsy == NULL)
    INTERNAL_ERROR();

  resolve_symbol_value(fixP->fx_addsy);
  symbol_mark_used_in_reloc(fixP->fx_addsy);

  if (fixP->fx_subsy != NULL) {
    resolve_symbol_value(fixP->fx_subsy);

    fixP->fx_offset -= S_GET_VALUE(fixP->fx_subsy);

    switch (fixP->fx_r_type) {
      case `R_'___arch_name___`_8:'
        fixP->fx_r_type = `R_'___arch_name___`_REL8;' 
        break;
      case `R_'___arch_name___`_16:'
        fixP->fx_r_type = `R_'___arch_name___`_REL16;' 
        break;
      case `R_'___arch_name___`_32:'
        fixP->fx_r_type = `R_'___arch_name___`_REL32;' 
        break;

      default:
        INTERNAL_ERROR();
    }
  }

  fixP->fx_done = FALSE;

  return 0;
}


void
___arch_name___`_adjust_reloc_count'(struct fix *fixP, long seg_reloc_count)
{
  return;

  /* NOTE: seg_reloc_count is actually not used in BFD_ASSEMBLER mode */
  for (; fixP; fixP = fixP->fx_next) {

    if (fixP->fx_done) 
      continue;

    ++seg_reloc_count;

    if (fixP->fx_addsy == NULL)
      fixP->fx_addsy = abs_section_sym;

    symbol_mark_used_in_reloc (fixP->fx_addsy);

    if (fixP->fx_subsy != NULL)
      symbol_mark_used_in_reloc (fixP->fx_subsy);
  }
}



/*

  Called by write_relocs() (write.c) for each fixup so that a BFD arelent structure can be build
and returned do be applied through 'bfd_install_relocation()' which in turn will call a backend
routine to apply the fix. As we are not dealing with relocations atm, we just return NULL so that
no bfd_install_relocation and similar functions will be called. 
*/
arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixp)
{
  arelent *rel;
  bfd_reloc_code_real_type r_type;

  rel = (arelent *) xmalloc (sizeof (arelent));
  rel->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *rel->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  rel->address = fixp->fx_frag->fr_address + fixp->fx_where;

  r_type = fixp->fx_r_type;
  //rel->addend = fixp->fx_addnumber;
  rel->addend = fixp->fx_offset;
//  rel->addend = fixp->tc_fix_data.addend;
  rel->howto = bfd_reloc_type_lookup (stdoutput, r_type);

  if (rel->howto == NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
        _("Cannot represent relocation type %s"),
        bfd_get_reloc_code_name (r_type));
      /* Set howto to a garbage value so that we can keep going.  */
      rel->howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_32);
      assert (rel->howto != NULL);
    }

  return rel;
}

char *
___arch_name___`_canonicalize_symbol_name'(char *c)
{
  if (insensitive_symbols) {
    strtolower(c);
  }

  return c;  
}

void
___arch_name___`_handle_align' (fragS *fragp  ATTRIBUTE_UNUSED)
{
/* We need to define this function so that in write.c, in routine 
 subsegs_finish, the variable alignment get the right size and the last frag 
 can be align.  
   I think there is another way to handle the alignment stuff without defining
this function (which is not mandatory). We just need to make md_section_align check 
the bfd alignment, and return the next aligned address. I've not tested that tho ;)
 */
}

void
___arch_name___`_symbol_new_hook' (symbolS *sym)
{
  dbg_printf(0, "Symbol Created!!!\n%*s", 4, "");
  dbg_print_symbolS(1, sym);
}





/*
   Called (by emit_expr()) when symbols might get their values taken like:
   .data
   .word label2
*/
extern void 
___arch_name___`_cons_fix_new'(struct frag *frag, int where, unsigned int nbytes, struct expressionS *exp)
{
  fixS *fixP = NULL;

  switch (nbytes) {
    case 1:
      fixP = fix_new_exp (frag, where, (int) nbytes, exp, 0, `R_'___arch_name___`_8');
      break;
    case 2:
      fixP = fix_new_exp (frag, where, (int) nbytes, exp, 0, `R_'___arch_name___`_16');
      break;
    case 4:
      fixP = fix_new_exp (frag, where, (int) nbytes, exp, 0, `R_'___arch_name___`_32');
      break;
    default:
      INTERNAL_ERROR();
  }

 
  fixP->fx_done = FALSE;


  if (!fixP) INTERNAL_ERROR();
}
 
/* ---------------------------------------------------------------------------------------
 Static functions
*/
 
/*
  We created this to auto-align data section whenever a data size > 1 is found
  (The MIPS version uses this method, so I decided to use also)
*/
static void
s_cons (int byte_size)
{
  if (byte_size > 1) {
    frag_align(log_table[byte_size], 0, 0);
    record_alignment(now_seg, log_table[byte_size]);
  }

  cons(byte_size);
}
 

static int
get_expression (expressionS *ep, char **str)
{
  char *save_in;

  save_in = input_line_pointer;

  input_line_pointer = *str;
  expression (ep);

#ifdef DEBUG_ON
  char save_char = *input_line_pointer;
  *input_line_pointer = '\0';
#endif
  dbg_printf(3, "Expression string: \"%s\"\n%*s", *str, 4*4, "");
  dbg_print_expression(4, ep);
#ifdef DEBUG_ON
  *input_line_pointer = save_char;
#endif

  *str = input_line_pointer;

  input_line_pointer = save_in;
  
  if (insn_error) return 0;
  return 1;
}


static int
get_immediate (expressionS *ep, char **str)
{
  char *save_in;

  save_in = input_line_pointer;

  input_line_pointer = *str;

  ep->X_op = O_constant;
  ep->X_add_number = get_single_number();

#ifdef DEBUG_ON
  char save_char = *input_line_pointer;
  *input_line_pointer = '\0';
#endif
  dbg_printf(3, "Immediate string: \"%s\"\n", *str);
#ifdef DEBUG_ON
  *input_line_pointer = save_char;
#endif

  *str = input_line_pointer;

  input_line_pointer = save_in;

  if (insn_error) return 0;
  return 1;
}

static int
get_address (expressionS *ep, char **str)
{
  char *save_in;
 
  save_in = input_line_pointer;
 
  input_line_pointer = *str;
  expression (ep);
 
#ifdef DEBUG_ON
  char save_char = *input_line_pointer;
  *input_line_pointer = '\0';
#endif
  dbg_printf(3, "Address string: \"%s\"\n%*s", *str, 4*4, "");
  dbg_print_expression(4, ep);
#ifdef DEBUG_ON
  *input_line_pointer = save_char;
#endif

  *str = input_line_pointer;

  input_line_pointer = save_in;
  
  if (insn_error) return 0;

  return 1;
}


static int 
get_userdefoper(unsigned int *val, const char *opclass, char **str)
{
  char *s_pos = *str;

  if (*s_pos == '\0') { 
    insn_error = "operand missing";
    return 0;
  }
 
  /* get the operand string (sync with ac_asm_map identifiers) */
  if ( (*s_pos >= 'a' && *s_pos <= 'z') ||
       (*s_pos >= 'A' && *s_pos <= 'Z') ||
       (*s_pos >= '0' && *s_pos <= '9') ||
       (*s_pos == '%' || *s_pos == '!' ||
        *s_pos == '@' || *s_pos == '#' ||
        *s_pos == '$' || *s_pos == '&' ||
        *s_pos == '*' || *s_pos == '-' ||
        *s_pos == '+' || *s_pos == '=' ||
        *s_pos == '|' || *s_pos == ':' ||
        *s_pos == '<' || *s_pos == '>' ||
        *s_pos == '^' || *s_pos == '~' ||
        *s_pos == '?' || *s_pos == '/') ||
        *s_pos == ',' || *s_pos == '_')
    s_pos++;
  else {
    insn_error = "unrecognized operand symbol";
    return 0;
  }
  
  while ( (*s_pos >= 'a' && *s_pos <= 'z') ||
          (*s_pos >= 'A' && *s_pos <= 'Z') ||
          (*s_pos >= '0' && *s_pos <= '9') ||
           *s_pos == '.' || *s_pos == '_' )
    s_pos++;

  char save_c = *s_pos;
  *s_pos = '\0';
 
  if (!ac_valid_symbol(opclass, *str, val)) {
    insn_error = "unrecognized operand symbol";
    *s_pos = save_c;
    return 0;
  }

  dbg_printf(3, "\"%s = %u\"\n", *str, *val);
 
  *s_pos = save_c;
  *str = s_pos;

  return 1;
}



/* 
  if ac_symbol == "" then marker is not checked
*/
static int ac_valid_symbol(const char *ac_symbol, char *parser_symbol, unsigned int *value)
{
  acasm_symbol *msymb = NULL;
  
  msymb = (acasm_symbol *) hash_find (sym_hash, parser_symbol);

  // TODO: handle the case when there are 2 or more symbols with same names (but different markers)

  // symbol not found
  if (msymb == NULL) return 0;
  
  // marker not valid for this symbol
  if (ac_symbol != "")    
    if (strcmp(msymb->cspec, ac_symbol)) return 0;

  *value = msymb->value;

  return 1;
}




//-------------------------------------------------------------------------------------
// Necessary stuff
//-----------------------



/*
--------------------------------------------------------------------------------------
 Command-line parsing stuff


  'void parse_args(int *, char ***)' in <as.c> is the main routine called to parse command line options for the main GAS
 program. The machine dependent (md) part might extend the short and long options of the main GAS ones by means of these 
 variables: 

 (obrigatory variables)
    const char *md_shortopts  (short options)
    struct option md_longopts (long options)
    size_t md_longopts_size  (this must be set to the long options' size)

   Two other routines *MUST* be set:

 (obrigatory)
   int md_parse_option(int c, char *arg)
      'c' may be a character (in case of a short option) or a value in the 'val' field of a long option structure. If 'arg'
      is not NULL, then it holds a string related somehow to 'c' (an argument to 'c'). Strictly speaking, 'c' is the value
      returned by a call to the getopt_long_only(...) by parse_args.
      Return 0 to indicate the option 'c' was not recognized or !0 otherwise.

 (obrigatory)
   md_show_usage (FILE *stream)
      This is called by the main GAS show_usage routine and should display this assembler machine dependent options in 
      'stream'

   Still, there might be defined one more routine to do special parsing handling or md setting right after the options 
      parsing (its called at the end of 'parse_args' from <as.c>):

 (optional)
   void md_after_parse_args ()
--------------------------------------------------------------------------------------
*/
const char *md_shortopts = "is";

struct option md_longopts[] =
{
  {"insensitive-syms", no_argument, NULL, OPTION_MD_BASE},
  {"sensitive-mno", no_argument, NULL, OPTION_MD_BASE+1},
  {"archc", no_argument, NULL, OPTION_MD_BASE+2} 
};
size_t md_longopts_size = sizeof (md_longopts);


static void
archc_show_information()
{
  fprintf (stderr, _("GNU assembler automatically generated by acbinutils 2.0.\n\
Architecture name: ___arch_name___.\n"));
}


int
md_parse_option (int c, char *arg ATTRIBUTE_UNUSED)
{
  switch (c) 
  {
    case OPTION_MD_BASE+0:
    case 'i':
      insensitive_symbols = 1;
      break;
    
    case OPTION_MD_BASE+1:
    case 's':
      sensitive_mno = 1;
      break;
     
    case OPTION_MD_BASE+2: /* display archc version information; */  
      archc_show_information();
      exit (EXIT_SUCCESS); 
      break;
     
    default:
      return 0;
  }

  return 1;
}


void
md_show_usage (FILE *stream)
{
  fprintf (stream, "md options:\n\n");

  fprintf (stream, _("\
  -i,--insensitive-syms   use case-insensitive symbols\n"));

  fprintf (stream, _("\
  -s,--sensitive-mno      use case-sensitive mnemonic strings\n"));
  
  fprintf (stream, _("\
  --archc                 display ArchC information\n"));
}
/*---------------------------------------------------------------------------*/


char *
md_atof (type, litP, sizeP)
     int type ATTRIBUTE_UNUSED;
     char *litP ATTRIBUTE_UNUSED;
     int *sizeP ATTRIBUTE_UNUSED;
{
  return NULL;
}


/* Convert a machine dependent frag.  */
void
md_convert_frag (abfd, asec, fragp)
     bfd *abfd ATTRIBUTE_UNUSED;
     segT asec ATTRIBUTE_UNUSED;
     fragS *fragp ATTRIBUTE_UNUSED;
{
  return;
}


valueT
md_section_align (seg, addr)
     asection *seg ATTRIBUTE_UNUSED;
     valueT addr;
{
  return addr;
}


int
md_estimate_size_before_relax (fragp, segtype)
     fragS *fragp ATTRIBUTE_UNUSED;
     asection *segtype ATTRIBUTE_UNUSED;
{
  return 0;
}


long
md_pcrel_from (fixP)
     fixS *fixP;
{
  /* should not be called */
  INTERNAL_ERROR();

//  if (fixP->fx_addsy == NULL)
//    INTERNAL_ERROR();

//  return fixP->fx_where + fixP->fx_frag->fr_address + fixP->tc_fix_data.pcrel_add;
}

symbolS *md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}

void md_operand (expressionS *expressionP)
{
  while (*input_line_pointer != '\0') input_line_pointer++;

  insn_error = "bad expression";
  expressionP->X_op = O_constant;
}


int ___arch_name___`_parse_name'(char *name, expressionS *expP, char *c ATTRIBUTE_UNUSED) {

  unsigned int dummy;

  if (in_imm_mode) { /* no name allowed when getting an 'imm' operand */
    insn_error = "invalid operand, expected a number";

    expP->X_op = O_absent;       /* some machines crash without these lines */
    expP->X_add_symbol = NULL; 
    return 1;
  }

  if (ac_valid_symbol("", name, &dummy)) { /* symbol found */
    expP->X_op = O_absent;
    insn_error = "invalid symbol in expression";
    return 1;
  }

  return 0;
}

void ___arch_name___`_frob_label'(symbolS *sym) {

  unsigned int dummy;

  dbg_printf(0, "Label Created!!!\n%*s", 4, "");
  dbg_print_symbolS(1, sym);

  if (ac_valid_symbol("",  (char *)S_GET_NAME(sym), &dummy))  /* symbol found as label -.- */
    as_bad(_("invalid label name: '%s'"), S_GET_NAME(sym));
  
}



static void 
create_fixup(unsigned int oper_id) 
{
  if (out_insn.is_pseudo) return;

  acfixuptype *fixups;
  
  if (out_insn.fixup == NULL)
  {
    out_insn.fixup = (acfixuptype *) malloc(sizeof(acfixuptype));
    out_insn.fixup->next = NULL;  
    fixups = out_insn.fixup;
  }
  else {
    fixups = out_insn.fixup;
    while (fixups->next != NULL) fixups = fixups->next;
    fixups->next = (acfixuptype *) malloc(sizeof(acfixuptype));
    fixups = fixups->next;
    fixups->next = NULL;
  }

  fixups->ref_expr_fix = ref_expr;
  fixups->operand_id = oper_id;
  
  dbg_printf(3,"Fixup = ");
  dbg_print_fixup(0, fixups);
}


static void 
encode_field(unsigned int val_const, unsigned int oper_id)
{
  mod_parms mp;
  char secname[] = "no symbol";

  if (out_insn.is_pseudo) return;

  mp.input   = val_const;
  mp.address = frag_now_fix();
  mp.section = secname; 

  dbg_printf(3, "image 0x%08lX + %u ==> ", out_insn.image, val_const);
  encode_cons_field(&out_insn.image, &mp, oper_id);
  dbg_printf(0, "0x%08lX\n", out_insn.image);

  if (mp.error) 
    as_bad(_("invalid operand value: '%u'"), val_const);

}



static void strtolower(char *str)
{
  while (*str != '\0') {
    *str = (uc)charmap[(uc)*str];
    str++;
  }
}

static void clean_out_insn()
{
  out_insn.image = 0;
  out_insn.format = 0;
  out_insn.is_pseudo = 0;
  out_insn.op_num = 0;
  out_insn.op_pos[9*2] = 0;
  free_all_fixups(out_insn.fixup);
  out_insn.fixup = NULL;
}

static void free_all_fixups(acfixuptype *fix)
{
  acfixuptype *current_fixP = fix;
  acfixuptype *last_fixP;

  while (current_fixP != NULL) {
    last_fixP = current_fixP;
    current_fixP = current_fixP->next; 

    free (last_fixP);
  }
}

static void print_internal_fixup(FILE *stream, unsigned int il, acfixuptype *fix)
{
//  expressions ref_expr_fix; /* symbol in which the relocation points to */
  fprintf(stream, "%*s<(Addend) %d, (fields) 0x%X, (mod) %u, (reloc) %u>\n", 
                    il*4, "",
                    operands[fix->operand_id].mod_addend,
                    operands[fix->operand_id].fields,
                    operands[fix->operand_id].mod_type,
                    operands[fix->operand_id].reloc_id);
}


static void print_fixup(FILE *stream, unsigned int il, fixS *fixP)
{
  fprintf(stream, "%*s%lu - [%s,%u] - pcrel %d, done %d, rel %u\n", 
                  il*4, "",
                  (unsigned long) fixP,
                  fixP->fx_file, fixP->fx_line,
                  fixP->fx_pcrel,
                  fixP->fx_done,
                  (unsigned int) fixP->fx_r_type);

  fprintf(stream, "%*ssize %u, where %ld, offset %ld, dot %lu, addnumber %ld\n", 
                  il*4, "",
                  fixP->fx_size, 
                  fixP->fx_where,
                  fixP->fx_offset,
                  fixP->fx_dot_value,
                  fixP->fx_addnumber);


  extern int indent_level;
  indent_level = il+1;
 
  if (fixP->fx_addsy) {
    fprintf(stream, "%*s", (il+1)*4, "");
    print_symbol_value_1(stderr, fixP->fx_addsy); 
    fprintf(stderr, "\n");
  }

  if (fixP->fx_subsy) {
    fprintf(stream, "%*s", (il+1)*4, "");
    print_symbol_value_1(stderr, fixP->fx_subsy); 
    fprintf(stderr, "\n");
  }

  fprintf(stream, "%*smd: operand type %u\n", 
                il*4, "",
                fixP->tc_fix_data);
}


