/*/////////////////////////////////////////////////////////////////////////////////////
// project : sFFe ( SegFault (or Segmentation Fault :) ) formula evalutaor )
// author  : Mateusz Malczak ( mateusz@malczak.info )
// wpage   : malczak.info
/////////////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#ifdef __APPLE__
  #include <ctype.h>
#elif __linux
  #include <ctype.h>
#endif

#include <string.h>

#ifdef SFFE_DEVEL
  #include <time.h>
#endif

#include "sffe.h"

#ifdef SFFE_DOUBLE
  #include "sffe_real.h"
#endif

#ifdef SFFE_DOUBLE
  #define sfset(arg,val) \
    (arg)->value = (SFNumber*)malloc(sizeof(SFNumber));\
      if ( (arg)->value ) \
      {\
        (arg)->type = sfvar_type_managed_ptr;\
        *((arg)->value) = (val);\
      }
#endif

/** debug functions
*/

#ifdef SFFE_DEVEL

void sffe_print_error(enum sffe_error err, char *ptr)
{
  /* in debug mode report errors on stdout */
  printf("Parser error : ");
  switch (err)
  {
    case SFFE_ERROR_MEM_ERROR:
      printf(" MEMORY ERROR!!");
      break;
    case SFFE_ERROR_UNBALANCED_BRACKES:
      printf(" UNBALANCED BRACKETS!! : %s\n", ptr);
      break;
    case SFFE_ERROR_INVALID_FUNCTION:
      printf(" UNKNOWN FUNCTION!! : %s\n", ptr);
      break;
    case SFFE_ERROR_INAVLID_NUMBER:
      printf(" NUMBER FORMAT!! : %s\n", ptr);
      break;
    case SFFE_ERROR_UNKNOWN_CONST:
      printf(" UNKOWN CONST or VAR NAME!! : %s\n", ptr);
      break;
    case SFFE_ERROR_OPERATOR:
      printf(" UNKNOWN OPERATOR!! : %s\n", ptr);
      break;
    case SFFE_ERROR_STACK_ERROR:
      printf(" INTERNAL STACK CORRUPTED!! : %s\n", ptr);
      break;
    case SFFE_ERROR_PAR_CNT_ERROR:
      printf(" FUNCTION PARAMETERS ERROR!! : %s\n", ptr);
      break;
    case SFFE_ERROR_NO_FUNCTIONS:
      printf("Formula error ! ARE YOU KIDDING ME ?!? : %s", ptr);
      break;
    default : break;
  };
}
#endif

/** utils
*/

void sffe_setup_error(sffe *parser, sffe_error err, char *ptr)
{
  /* try to store error message */
  if (parser->errormsg)
  {
    switch (err) {
      case SFFE_ERROR_MEM_ERROR:
        sprintf(parser->errormsg, "Formula error ! MEMORY ERROR!!");
        break;
      case SFFE_ERROR_UNBALANCED_BRACKES:
        sprintf(parser->errormsg,
                "Formula error ! UNBALANCED BRACKETS!! : %s", ptr);
        break;
      case SFFE_ERROR_INVALID_FUNCTION:
        sprintf(parser->errormsg,
                "Formula error ! UNKNOWN FUNCTION!! : %s", ptr);
        break;
      case SFFE_ERROR_INAVLID_NUMBER:
        sprintf(parser->errormsg,
                "Formula error ! NUMBER FORMAT!! : %s", ptr);
        break;
      case SFFE_ERROR_UNKNOWN_CONST:
        sprintf(parser->errormsg,
                "Formula error ! UNKOWN CONST or VAR NAME!! : %s", ptr);
        break;
      case SFFE_ERROR_OPERATOR:
        sprintf(parser->errormsg,
                "Formula error ! UNKNOWN OPERATOR!! : %s", ptr);
        break;
      case SFFE_ERROR_STACK_ERROR:
        sprintf(parser->errormsg,
                "Formula error ! INTERNAL STACK CORRUPTED!! : %s", ptr);
        break;
      case SFFE_ERROR_PAR_CNT_ERROR:
        sprintf(parser->errormsg,
                "Formula error ! FUNCTION PARAMETERS ERROR!! : %s", ptr);
        break;
      case SFFE_ERROR_NO_FUNCTIONS:
        sprintf(parser->errormsg,
                "Formula error ! ARE YOU KIDDING ME ?!? : %s", ptr);
        break;
      default: break;
    };
  }
}

void sf_strdup(char **out, const char * in)
{
  size_t name_len = strlen(in);
  char *dup = malloc(name_len + 1);
  if(dup)
  {
    for (size_t i = 0; i < name_len; i += 1)
    {
      dup[i] = (char) toupper((int) in[i]);
    }
    dup[name_len] = '\0';
  }
  *out = dup;
}

unsigned char sf_priority(char * chr){
  switch (*chr) {
    case 'f':
      return 0x60;
    case '^':
      return 0x40;
    case '/':
    case '*':
      return 0x20;
    case '+':
    case '-':
      return 0x00;
    default:
      return 0x80;
  }
}

/************************* custom function */
/* all used in this section variables are defined depanding on complex number implementation */
sffunction *sffe_function(char *fn, size_t len)
{
  /* sffnctscount - defined in sffe_cmplx_* file */
  for (unsigned char idx = 5; idx < sffnctscount; idx += 1)
  {
    if (!strncmp(fn, sfcmplxfunc[idx].name, len))
    {
      return (sffunction *) (sfcmplxfunc + idx);
    }
  }
  return NULL;
}

sffunction *sffe_operator(char op)
{
  switch (op) {
    case '^':
      return (sffunction *) sfcmplxfunc;
    case '+':
      return (sffunction *) sfcmplxfunc + 1;
    case '-':
      return (sffunction *) sfcmplxfunc + 2;
    case '*':
      return (sffunction *) sfcmplxfunc + 3;
    case '/':
      return (sffunction *) sfcmplxfunc + 4;
    default:
      break;
  }
  return NULL;
}

void *sffe_const(char *fn, size_t len, void *ptr)
{
  for (unsigned char idx = 0; idx < sfvarscount; idx += 1)
  {
    if (!strncmp(fn, sfcnames[idx], len))
    {
      sfcvals[idx] ((SFNumber *) ptr);
      return ptr;
    };
  }
  return NULL;
}

/************************* custom function */


sffe *sffe_alloc(void)
{
  sffe *rp = (sffe *) malloc(sizeof(sffe));
  if (!rp)
  {
    return NULL;
  }
  
  memset(rp, 0, sizeof(sffe));
  return rp;
}

void sffe_clear(sffe ** parser)
{
  sffe *p = *parser;
  unsigned int i = 0;
  for (; i < p->argCount; i++)
  {
    if(p->args[i].type == sfvar_type_managed_ptr)
    {
      free(p->args[i].value);
    }
  }
  
  if (p->args)
  {
    free(p->args);
  }
  
  if (p->expression)
  {
    free((char*)p->expression);
  }
  
  if (p->oprs)
  {
    free(p->oprs);
  }
  
  p->expression = NULL;
  p->args = NULL;
  p->argCount = 0;
  p->oprs = NULL;
  p->oprCount = 0;
}

void sffe_free(sffe ** parser)
{
  sffe *p = *parser;
  
  sffe_clear(parser);
  
  if (p->userf) {
    free(p->userf);
  }
  
  if (p->varCount)
  {
    unsigned int i = 0;
    for (; i < p->varCount; i++)
    {
      if(p->variables[i].type == sfvar_type_managed_ptr)
      {
        free(p->variables[i].value);
      }
    };
    
    free(p->variables);
  }
  
  free(*parser);
  parser = NULL;
}

SFNumber sffe_eval(sffe * const parser)
{
  register sfopr *optr = parser->oprs;
  register sfopr *optro = optr;
  register sfopr *optrl = optr + parser->oprCount;
  for (; optr != optrl; optr += 1, optro += 1)
  {
    optro->arg->parg = optro->arg - 1;
    register sfarg* arg = optr->arg;
#ifdef SFFE_DIRECT_FPTR
    arg->parg = optr->fnc( arg, NULL )->parg;
#else
    arg->parg = optr->fnc->fptr( arg, optr->fnc->payload )->parg;
#endif
  };
  return *(parser->result);
}

sfvariable* sffe_var(sffe *const parser, const char* name)
{
  if(parser->varCount)
  {
    sfvariable *var = parser->variables;
    sfvariable *lvar = parser->variables + parser->varCount;
    while (var < lvar)
    {
      if(!strcasecmp(var->name, name))
      {
        return var;
      }
      var += 1;
    }
  }
  
  return NULL;
}

sfvariable* sffe_regvar(sffe ** parser, SFNumber * vptrs, const char* name)
{
  sffe *parser_ = *parser;
  sfvariable *var = sffe_var(parser_, name);
  if(var)
  {
    return var;
  }
  
  int vars_cnt = parser_->varCount + 1;
  parser_->variables = realloc(parser_->variables, vars_cnt * sizeof(sfvariable));
  if(!parser_->variables)
  {
    return NULL;
  }
  
  var = parser_->variables + parser_->varCount;
  if(vptrs == NULL)
  {
    vptrs = (SFNumber*) malloc(sizeof(SFNumber));
    memset(vptrs, 0, sizeof(SFNumber));
    var->type = sfvar_type_managed_ptr;
  } else {
    var->type = sfvar_type_ptr;
  }
  var->value = vptrs;
  sf_strdup(&var->name, name);
  
  parser_->varCount += 1;
  return var;
}

void sffe_regvars(sffe ** parser, unsigned int cN, SFNumber ** vptrs, char* const* names)
{
  while(cN>0)
  {
    cN -= 1;
    sffe_regvar(parser, (vptrs ? vptrs[cN] : NULL), names[cN]);
  }
}

SFNumber* sffe_setvar(sffe ** parser, SFNumber vptrs, const char* name)
{
  SFNumber *value;
  sffe *parser_ = *parser;
  sfvariable *var = sffe_var(parser_, name);
  if(!var)
  {
    var = sffe_regvar(parser, NULL, name);
  }
  
  value = var->value;
  memcpy(value, &vptrs, sizeof(SFNumber));
  return value;
}

void *sffe_regfunc(sffe ** parser, const char *vname, unsigned int parcnt, sffptr funptr, void *payload)
{
  sffe *parser_ = *parser;
  sffunction *sff;
  unsigned short i;
  
  parser_->userf = (sffunction *) realloc(parser_->userf, (parser_->userfCount + 1) * sizeof(sffunction));
  if (!parser_->userf)
  {
    return NULL;
  }
  
  sff = parser_->userf + parser_->userfCount;
  
  sf_strdup(&sff->name, vname);
  
  for (i = 0; i < strlen(vname); i += 1)
  {
    sff->name[i] = (char) toupper((int) vname[i]);
  }
  
  sff->parcnt = parcnt;
  sff->fptr = funptr;
  
  parser_->userfCount += 1;
  return (void *) sff;
}

SFNumber* sffe_variable(sffe * const p, char *fname, size_t len)
{
  char fn[len+1];
  fn[len] = '\0';
  memcpy(fn, fname, len);
  sfvariable *var = sffe_var(p, fn);
  if(var)
  {
    return var->value;
  }
  return NULL;
}

sffunction *userfunction(const sffe * const p, char *fname, size_t len)
{
  unsigned char idx;
  for (idx = 0; idx < p->userfCount; idx += 1)
  {
    const char * name = p->userf[idx].name;
    if (strlen(name) == len && (!strncmp(fname, name, len)))
    {
		    return (sffunction *) (p->userf + idx);
    }
  }
  return NULL;
}

char sffe_donum(char **str)
{
  /* parse number in format [-+]ddd[.dddd[e[+-]ddd]]  */
  unsigned char flag = 0;	/*bit 1 - dot, bit 2 - dec, bits 3,4 - state, bits 5..8 - error */
  if (**str == '-')
  {
    flag = 0x80;
    *str += 1;
  }
  
  if (**str == '+')
  {
    *str += 1;
  }
  
  while (!((flag >> 4) & 0x07))
  {
    switch ((flag & 0x0f) >> 2)
    {
      case 0:		/*0..9 */
        while (isdigit(**str))
        {
          *str += 1;
        }
        
        switch (**str) {	/*only '.' or 'E' allowed */
          case '.':
            flag = (flag & 0xf3) | 4;
            break;
          case 'E':
            flag = (flag & 0xf3) | 8;
            break;
          default:
            flag = 0x10;
        };
        break;
      case 1:		/*.  */
        if (flag & 0x03)
        {
          flag = 0x20;
        } else {
          *str += 1;	/*no 2nd dot, no dot after E  */
        }
        
        flag = (flag & 0xf2) | 0x01;
        break;
      case 2:		/*e  */
        if (flag & 0x02)
        {
          flag = 0x30;
        } else {
          *str += 1;	/*no 2nd E */
        }
        
        if (!isdigit(**str)) /*after E noly [+-] allowed */
        {
          if (**str != '-' && **str != '+')
          {
            flag = 0x40;
          } else {
            *str += 1;
          }
        }
        
        flag = (flag & 0xf1) | 0x02;
        break;
    };
  };
  
  if (flag & 0x80)
  {
    flag ^= 0x80;
  }
  
  return flag >> 4;
}

char sffe_doname(char **str)
{
  do {
    *str += 1;
  } while (isalnum(**str) || **str == '_');
  
  if (strchr("+-*/^~!@#$%&<>?\\:\"|", (int) **str))
  {
    return 1;		/*punctator  */
  }
  
  if (**str == '(')
  {
    return 2;		/* ( - funkcja  */
  }
  
  if (**str == '.')
  {
    return 3;		/*error :( this means something like X. COS. PI. */
  }
  
  return 1;
}

sffe_error sffe_parse(sffe ** parser, const char *expression)
{
#define TRY { jmp_buf ex_buf__; int ex_state__ = setjmp(ex_buf__); if(ex_state__ == 0) {
#define FINALLY } else { int error = ex_state__;
#define ETRY } }
#define THROW(x) longjmp(ex_buf__, (int)x)
  sffe_error result = SFFE_ERROR_OK;
  sffe *_parser;
  
  /**************var area */
  struct _operator { // @todo replace with stack/list
#ifdef SFFE_DEVEL
    char c;             /* used in debug build to store operator character */
#endif
    unsigned char type;	/* store priority of the operator 'f' */
    unsigned char args; /* number of parameters */
#ifdef SFFE_DIRECT_FPTR
    sffptr fnc;
#else
    sffunction*	fnc;
#endif
  };
  
  struct __expression {
    struct _operator *stck; /* operators on stack */
    unsigned int size;      /* number of items on stack */
    struct __expression *prev;   /* previous stack */
  } *_tmp_exp = NULL, *_expression = NULL;
  
  sffunction **_functions = NULL; /* hold all functions used in expression in left - to - right order */
  sffunction **_function = NULL; /* currently expected function from 'fnctbl' */
  sfarg *_argument = NULL, *_arg_itr = NULL;
  
  char *tokens = NULL;		/*tokenized form : (f(n)+f(n))*f(n)-n (f-func, n-num,const) */
  
  char *ech;
  char *ch1, *ch2;
  
  unsigned int ui1 = 0;
  unsigned char token = '\0';
  
  /*
  struct __token {
    unsigned char symbol;
    unsigned int position;
    struct __token *next;
    struct __token *prev;
    union {
      sfarg *argument;
      sffunction *function;
    } data;
  }
  */
  
//  enum sffe_error err;
  
  /**************used defines */
  
#define append_operand() \
{\
  _parser->args = (sfarg *) realloc(_parser->args, (_parser->argCount + 1) * sizeof(sfarg));\
  if (!_parser->args)\
  {\
    THROW(SFFE_ERROR_MEM_ERROR);\
  }\
  _argument = _parser->args + (_parser->argCount++);\
}

#define append_function(fnc) \
{\
  _functions = (sffunction **) realloc(_functions, (++_parser->oprCount) * sizeof(sffunction *));\
  if (!_functions)\
  {\
    THROW(SFFE_ERROR_MEM_ERROR);\
  }\
  _functions[_parser->oprCount - 1] = fnc;\
}

#define append_token(chr)\
{\
  tokens = (char*)realloc(tokens,ui1+2);\
  if (!tokens)\
  {\
    THROW(SFFE_ERROR_MEM_ERROR);\
  }\
  tokens[ui1++] = chr;\
  token = chr;\
  tokens[ui1] = '\0';\
}
  
#define insert_fnc_slot()\
{\
  for ( _arg_itr = _parser->args + _parser->argCount - 1; _arg_itr > _argument; _arg_itr -= 1 )\
  {\
    *_arg_itr = *(_arg_itr-1);\
  };\
  sfset(_arg_itr,-1.0);\
}
  
#define pop_expression(expr)\
{\
  expr->size-=1;\
  insert_fnc_slot();\
  _parser->oprs[ui1].arg = (sfarg*)_argument;\
  _parser->oprs[ui1].fnc = expr->stck[_expression->size].fnc;\
  ui1 += 1;\
  _argument += 1;\
};
  
#define max(a,b) ((a>b) ? a : b)
  
#ifdef SFFE_DEVEL
  clock_t begin = clock();
  printf("parse - BEGIN\n");
#endif
  
  /**************** CODE */
  _functions = NULL;
  ech = (char*)expression;
  tokens = (char *) malloc(1);

  //parser
  _parser = *parser;
  
  /* clear all internal structures */
  if (_parser->expression)
  {
    sffe_clear(parser);
  }
  
  
  TRY
  {
    
    _parser->oprCount = 0;
    _parser->argCount = 0;
    
    ech =(char *) malloc(strlen(expression) + 1);
    sf_strdup(&ech, expression);
    _parser->expression = ech;
    
    
#ifdef SFFE_DEVEL
    printf("\n|-----------------------------------------\n+ > %s[%d] - parsing\n|-----------------------------------------\n",__FILE__, __LINE__);
    printf("| input (len.=%tu): |%s|\n", strlen(_parser->expression),_parser->expression);
#endif
    
    /*! PHASE 1 !!!!!!!!! remove spaces, count brackets, change decimal separators ',' to '.', remove multiple operators eg. ++--++1 -> 1, -+++2 -> -2 */
    ch1 = NULL;
    ui1 = 0;			/*brackets */
    ch2 = ech;
    
    /* skip leading spaces */
    while (isspace(*ech)) {
      ech += 1;
    }
    
    /*handle brackets and change ','->'.', '['->'(', ']'->')' */
    while (*ech) {
      switch (*ech) {
        case '[':
          *ech = '(';
        case '(':
          ui1 += 1;
          break;
        case ']':
          *ech = ')';
        case ')':
          ui1 -= 1;
          break;
        case ',':
          *ech = '.';
          break;
      };
      
      *ch2 = (char) toupper((int) *ech);
      
      /*fix multiple arithm operators */
      if (ch1 && strchr("+-/*^", (int) *ech) && strchr("+-/*^", (int) *ch1))
      {
        if (*ch1 == '-' && *ech == '-') {
          *ch1 = '+';
        } else if (*ch1 == '-' && *ech == '+') {
          *ch1 = '-';
        } else if (*ch1 == '+' && *ech == '-') {
          *ch1 = '-';
        } else if (*ch1 == *ech) {
          *ch1 = *ech;
        } else if (*ech == '-') {
          ch1 = ++ch2;
        } else if (*ch1 != *ech) {
          THROW(SFFE_ERROR_OPERATOR);
        };
      } else {
        ch1 = ch2;
        ch2 += 1;
      };
      
      /*skip spaces */
      do {
        ech += 1;
      } while (isspace(*ech));
    };
    
    *ch2 = '\0';
    
    _parser->expression = (char *) realloc((char*)_parser->expression, strlen(_parser->expression) + 1);
    
    if (ui1) {
      THROW(SFFE_ERROR_UNBALANCED_BRACKES);
    }
    
#ifdef SFFE_DEVEL
    printf("| check (len.=%tu): |%s|\n", strlen(_parser->expression),_parser->expression);
#endif
    
    /*! PHASE 2 !!!!!!!! tokenize expression, lexical analysis (needs optimizations) */
    sffunction *token_function = NULL;
    *tokens = '\0';
    ch2 = NULL;
    ui1 = 0; /* tokens count */
    ch1 = NULL;			/*string starting position */
    ech = (char*)_parser->expression;
    token = '(';			/* in case of leading '-' */
    
    while (*ech)
    {
      ch1 = ech;
      
      if (isalpha(*ech))
      {
        switch (sffe_doname(&ech))
        {
          case 1:		/* const or variable */
            append_operand();
            _argument->type = sfvar_type_ptr;
            _argument->value = (SFNumber *) sffe_variable(_parser, ch1, (size_t) (ech - ch1));
            
            if (!_argument->value)
            {
              sfset(_argument, 10.0); //? temporary const value
              if (_argument->value)
              {
                if (!sffe_const(ch1, (size_t) (ech - ch1), _argument->value))
                {
                  THROW(SFFE_ERROR_UNKNOWN_CONST);
                }
              } else {
                THROW(SFFE_ERROR_MEM_ERROR);
              }
            }
            
            token = 'n';
            break;
            
          case 2:		/* function */
            if (_parser->userfCount)
            {
              /*is it user defined function */
              token_function = (sffunction *) (void *) userfunction(_parser, ch1, (size_t) (ech - ch1));
            }
            
            if (!token_function)
            {
              /*if not, is it build in function */
              token_function = (sffunction *) (void *) sffe_function(ch1, (size_t)(ech - ch1));
            }
            
            /* if not -> ERROR */
            if (!token_function)
            {
              THROW(SFFE_ERROR_INVALID_FUNCTION);
            }
            
            token = 'f';
            break;
            
          case 3:		/* what ? */
            THROW(SFFE_ERROR_OPERATOR);
        };
      } else
      /* numbers (this part can be optimized) */
      /* is it a real number */
        if (isdigit(*ech) || (strchr("/*^(", (int) token) && strchr("+-", *ech)))
        {
          ch1 = ech;		/* st = 1;  */
          
          if (sffe_donum(&ech) > 1)
          {
            THROW(SFFE_ERROR_INAVLID_NUMBER);
          }
          
          append_operand();
          /* '-n'/'+n', which was parsed as 0*n for vars */
          if ((ech - ch1) == 1 && (*ch1 == '-'))
          {
            sfset(_argument, -1)
          } else {
            sfset(_argument, atof(ch1));
          }
          
          /*epx */
          token = 'n';
        } else
        /* if not, we have operator */
        {
          token_function = sffe_operator(*ech);
          
          ch1 = ech;
          token = *ech;
          ech += 1;
        };
      
      
      /*
        BUG:
        * unhandled case is eq (-PI/-x) -> this is parsed as -PI/-1 * x => (-PI/-1) * x => expected: -PI / ( -1 * x) OR -PI / -_prefix(x)
       */
      /* no error and already has any opcodes - check for skipped multiplication. Handle nf, n(, )(, )f, )n, fn */
      if (ui1 > 0)
      {
        if (token == 'f' || token == 'n' || token == '(') // current token
        {
          unsigned char last_token = tokens[ui1-1];
          if (last_token == 'n' || last_token == ')') // last-1 token
          {
            // inject multiplication
            unsigned char tmp = token;
            append_function(sffe_operator('*'))
            append_token('*');
            token = tmp;
          };
        }
      }
      
      // if any function -> append it !
      if(token_function)
      {
        append_function(token_function)
      }
      token_function = NULL;
      
      // store token
      append_token(token);
    };
    
    ech = tokens;
    
#ifdef SFFE_DEVEL
    printf
    ("| compiled expr.: |%s|\n| operations: %d\n| numbers,vars: %d\n| stack not.: ",
     tokens, _parser->oprCount, _parser->argCount);
#endif
    
    /*! PRE PHASE 3 !!!!! no operations in expression = single numeric value */
    if (!_parser->oprCount && _parser->argCount == 1) {
      _parser->oprs = (sfopr *) malloc(_parser->argCount * sizeof(sfopr));
      _parser->oprs[0].arg = (sfarg *) _parser->args;
      _parser->oprs[0].fnc = NULL;
      _parser->result = (SFNumber *) _parser->args->value;
    } else
    {
      /*! PHASE 3 !!!!! create sffe 'stack' notation ]:-> */
      /* lots of memory operations are done here but no memory leaks should occur */
      /* add value slots for uses operators/functions */
      ui1 = _parser->argCount + _parser->oprCount;
      _parser->args = (sfarg *) realloc(_parser->args, ui1 * sizeof(sfarg));
      memset(_parser->args + _parser->argCount, 0, _parser->oprCount * sizeof(sfarg));
      _parser->argCount = ui1;
      _argument = _parser->args;
      _parser->oprs = (sfopr *) malloc(_parser->oprCount * sizeof(sfopr));
      ch1 = NULL;		/* number */
      
      /* stacks ( stores operations and controls parameters count inside of brackts blocks ) */
      _expression = (struct __expression *) malloc(sizeof(struct __expression));
      _expression->prev = NULL;
      _expression->size = 0;	/* 0-stack is empty, but ready to write (one slot allocated), >0-number of element on stack */
      _expression->stck = (struct _operator *) malloc(sizeof(struct _operator));
      memset(_expression->stck, 0, sizeof(struct _operator));
      
      ui1 = 0;		/* used in defines */
      _function = _functions;
      
      while (*ech)
      {
        switch (*ech)
        {
            
            /*  O */
          case '+':
          case '-':
          case '*':
          case '/':
          case '^':{
            if (ch1)
            {
#ifdef SFFE_DEVEL
              printf("%c", *ch1);
#endif
              _argument += 1;
            };
            
            
            unsigned char type = sf_priority(ech);
            /* there is an operator on stack */
            if (_expression->size)
            {
              /* remove all operators with higher, or equal priority */
              while (type <= _expression->stck[_expression->size - 1].type)
              {
                pop_expression(_expression);
#ifdef SFFE_DEVEL
                printf("%c",_expression->stck[_expression->size].c);
#endif
                
                if (_expression->size == 0)
                {
                  break;
                }
              };
              
              _expression->stck = (struct _operator *) realloc(_expression->stck, (_expression->size + 1) * sizeof(struct _operator));
            };
            
            sffunction *function =  *_function;
            
#ifdef SFFE_DEVEL
            struct _operator* opstck = &_expression->stck[_expression->size];
            opstck->c = *ech;
#endif
            
            /* store operator prority */
            _expression->stck[_expression->size].type = type;
            
            /* get function pointer */
#ifdef SFFE_DIRECT_FPTR
            _expression->stck[_expression->size].fnc = function->fptr;
#else
            _expression->stck[_expression->size].fnc = function;
#endif
            
            _expression->size += 1;
            
            _function += 1;
            ch1 = NULL;
          }break;
            
            /* F  */
          case 'f':{
            _expression->stck = (struct _operator *) realloc(_expression->stck, (_expression->size + 1) * sizeof(struct _operator));
            
            sffunction *function =  *_function;
            
            struct _operator* opstck = &_expression->stck[_expression->size];
#ifdef SFFE_DEVEL
            opstck->c = 'f';
#endif
            
            unsigned char  parcnt = function->parcnt & 0x1F;
            /* mark operator as a function, and store number of available parameters (0 - unlimited) */
            opstck->type = 0x60 | parcnt;
            opstck->args = parcnt;
            
            /* get function pointer */
#ifdef SFFE_DIRECT_FPTR
            _expression->stck[_expression->size].fnc = function->fptr;
#else
            _expression->stck[_expression->size].fnc = function;
#endif
            
            _expression->size += 1;
            
            _function += 1;
            ch1 = NULL;
            
            // consume ()
            //                    if(!parcnt)
            //                    {
            //                        ech += 2;
            //                    }
            
          }break;	// skip to ( ???
            
            /* (  */
          case '(':{
            /* store current stack */
            _tmp_exp = (struct __expression *) malloc(sizeof(struct __expression));
            _tmp_exp->prev = _expression;
            _expression = _tmp_exp;
            _expression->size = 0;
            _expression->stck = (struct _operator *) malloc(sizeof(struct _operator));
            memset(_expression->stck, 0, sizeof(struct _operator));
            
            
#ifdef SFFE_DEVEL
            _expression->stck[0].c = '_';
#endif
            
            token = 0;
          }break;
            
            /*  ; */
          case ';':{
            /* check if anything has been read !!! */
            if (ch1)
            {
#ifdef SFFE_DEVEL
              printf("%c", *ch1);
#endif
              _argument += 1;
              ch1 = NULL;
            };
            
            /* if there is something on stack, flush if we need to read next parameter */
            while (_expression->size)
            {
              pop_expression(_expression);
#ifdef SFFE_DEVEL
              printf("%c",_expression->stck[_expression->size].c);
#endif
            }
            
            struct __expression* pstack = _expression->prev;
            struct _operator* opstck = &pstack->stck[pstack->size - 1]; // here is last function before opening new op stack
            
            /* wrong number of parameters */
            if ((opstck->type & 0x1f) == 1)
            {
              THROW(SFFE_ERROR_PAR_CNT_ERROR);
            }
            
            /* reduce a number of allowed parameters */
            opstck->type = 0x60 | max(0,(opstck->type & 0x1f) - 1);
          }break;
            
            /* )  */
          case ')':{
            if (ch1)
            {
#ifdef SFFE_DEVEL
              printf("%c", *ch1);
#endif
              _argument += 1;
            }
            ch1 = NULL;
            
            /* if there is something on stack, flush it we need to read next parameter */
            while (_expression->size)
            {
              pop_expression(_expression);
#ifdef SFFE_DEVEL
              printf("%c",_expression->stck[_expression->size].c);
#endif
            }
            
            /* no stack available = stack overrelesed */
            if (!_expression->prev)
            {
              THROW(SFFE_ERROR_STACK_ERROR);
            }
            
            _tmp_exp = _expression;
            _expression = _tmp_exp->prev;
            
            /* destroy block stack */
            free(_tmp_exp->stck);
            free(_tmp_exp);
            
            /* parser was reading function, at the top of current stack is a function. identified by '*.t==3' */
            if(_expression->size)
            {
              struct _operator* opstck = &_expression->stck[_expression->size - 1]; // here is last function before opening new op stack
              if ((opstck->type & 0xE0) == 0x60)
              {
                
                /* wrong number of parameters */
                if ((opstck->type & 0x1f) > 1)
                {
                  THROW(SFFE_ERROR_PAR_CNT_ERROR);
                }
                
                pop_expression(_expression);
#ifdef SFFE_DEVEL
                printf("%c",_expression->stck[_expression->size].c);
#endif
                if (_expression->size)
                {
                  _expression->stck = (struct _operator *) realloc(_expression->stck, (_expression->size) * sizeof(struct _operator));
                }
              };
            }
            
          }break;
            
            /* n */
          case 'n':
            ch1 = ech;
            break;
            
        };
        ech += 1;
      };
      
      if (ch1)
      {
#ifdef SFFE_DEVEL
        printf("%c", *ch1);
#endif
        _argument += 1;
      }
      
      /*clean up _expression */
      _tmp_exp = _expression;
      while (_tmp_exp)
      {
        while (_tmp_exp->size)
        {
          pop_expression(_tmp_exp);
#ifdef SFFE_DEVEL
          printf("%c",_tmp_exp->stck[_expression->size].c);
#endif
        };
        _tmp_exp = _tmp_exp->prev;
      };
      
      /* set up formula call stack */
      (_parser->args)->parg = NULL;
      
      for (ui1 = 1; ui1 < _parser->argCount; ui1 += 1)
      {
        (_parser->args + ui1)->parg = (_parser->args + ui1 - 1);
      }
      
#ifdef SFFE_DEVEL
      printf("\n| numbers: ");
      for (ui1 = 0; ui1 < _parser->argCount; ui1 += 1)
      {
        if ((_parser->args + ui1)->value)
        {
#ifdef SFFE_DOUBLE
          printf(" %g", (*(_parser->args + ui1)->value));
#endif
        } else {
          printf(" [_]");
        }
      };
      
      printf("\n| functions fnctbl:");
      for (ui1 = 0; ui1 < _parser->oprCount; ui1 += 1)
      {
        printf(" 0x%.6X [%s]", (int) _functions[ui1]->fptr, _functions[ui1]->name);
      }
      
      printf("\n| functions used ptrs:");
      for (ui1 = 0; ui1 < _parser->oprCount; ui1 += 1)
      {
        printf(" 0x%.6X", (int) _parser->oprs[ui1].fnc);
      }
      
      double time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
      printf("\n| compiled in  %f s", time_spent);
      
      printf
      ("\n|-----------------------------------------\n+ < %s[%d] - parsing\n|-----------------------------------------\n",
       __FILE__, __LINE__);
#endif
      
      /* set up evaluation result pointer (result is stored in last operation return) */
      _parser->result = (SFNumber *) (_parser->oprs + _parser->oprCount - 1)->arg->value;
      
      if (!_parser->result) {
        THROW(SFFE_ERROR_MEM_ERROR);
      }
    }
  }
  FINALLY
  {
#ifdef SFFE_DEVEL
    sffe_print_error(error, ch1);
#endif
    sffe_setup_error(_parser, error, ch1);
    sffe_clear(&_parser);
    
    result = error;
  }
  ETRY;
  
  /*undefine defines */
#undef priority
#undef sfpopstack
#undef insertfnc
#undef code
#undef errset
#undef max

  /* cleanup expression stacks */
  while (_expression)
  {
    free(_expression->stck);
    _tmp_exp = _expression->prev;
    free(_expression);
    _expression = _tmp_exp;
  };

  /* free lookup tables */
  free(tokens);
  free(_functions);
  
#ifdef SFFE_DEVEL
  printf("\nparse - END\n");
#endif
  
  return result;
}

#undef sfset
#undef sfvar
