/*/////////////////////////////////////////////////////////////////////////////////////
// project : sFFe ( SegFault (or Segmentation Fault :) ) formula evalutaor )
// author  : Mateusz Malczak ( mateusz@malczak.info )
// wpage   :
///////////////////////////////////////////////////////////////////////////////////////
// possible config definitions
//   general
//	SFFE_DOUBLE - real math parser
//	SFFE_DEVEL - print extra info to stdout
//	SFFE_DIRECT_FPTR - use direct function pointers (!!!) ommits payload
/////////////////////////////////////////////////////////////////////////////////////*/

#ifndef SFFE_H
#define SFFE_H

#include <stdlib.h>

#define SFFE_DOUBLE 1

#define SFFE_DIRECT_FPTR 1


#ifdef SFFE_DOUBLE
	#define SFNumber		double	
#endif

typedef enum {
    sfvar_type_ptr,
    sfvar_type_managed_ptr
} sfvartype;


/* basic sffe argument 'stack' */
typedef struct sfargument__
{
    struct sfargument__ *parg;
    sfvartype type;
    SFNumber *value;
} sfarg;

/* sffe function prototype, parameters order is right-to-left (cdecl) */
typedef sfarg *(*sffptr)(sfarg * const a, void *payload);

/* constats eval functions */
typedef void (*cfptr) (SFNumber * cnst);

/* function type structure */
typedef struct
{
    sffptr fptr;
    unsigned char parcnt;
    /*FIXME changed from char* to char[20] to get rid of warnings during compilation */
    char *name;
    void *payload; // unmanaged opaque memory pointer
} sffunction;

/* basic sffe 'stack' operation ( function + result slot ) */
typedef struct
{
    sfarg*		arg;
#ifdef SFFE_DIRECT_FPTR
    sffptr fnc;
#else
    sffunction*	fnc;
#endif
} sfopr;

typedef struct
{
    char *name;
    sfvartype type;
    SFNumber *value;
} sfvariable;

typedef struct sfcontext__
{
    unsigned int funcsCount; /* number of default / user functions */
    sffunction *functions;
    
    unsigned int constsCount;
    cfptr *constants;
} sffe_context;

/* SFFE main structure */
typedef struct
{
/*public*/
    const char *expression;		/* parsed expression (read-only) */
    char *errormsg;         /* parser errors (read-only) */
    SFNumber *result;		/* evaluation result (read-only) */
    
/* protected/private */
    unsigned int argCount;	/* number of arguments in use */
    sfarg *args;
    
    unsigned int oprCount;	/* number of operations in use */
    sfopr *oprs;
    
    unsigned int varCount;	/* number of used variables */
    sfvariable *variables;
    
    unsigned int userfCount; /* number of user functions */
    sffunction *userf;
} sffe;

typedef enum {
  SFFE_ERROR_OK = 0,
  SFFE_ERROR_MEM_ERROR = 1,
  SFFE_ERROR_UNBALANCED_BRACKES,
  SFFE_ERROR_INVALID_FUNCTION,
  SFFE_ERROR_INAVLID_NUMBER,
  SFFE_ERROR_UNKNOWN_CONST,
  SFFE_ERROR_OPERATOR,
  SFFE_ERROR_STACK_ERROR,
  SFFE_ERROR_PAR_CNT_ERROR,
  SFFE_ERROR_NO_FUNCTIONS
} sffe_error;

#define SFFE sffe
#define sffeparser sffe
#define sfparser sffe
#define SFFEPARSER sffe

/* 'stack' slot value */
#define sfvalue(p) (*((p)->value))

/* function parameters */
#define sfaram1(p) ((p)->parg)
#define sfaram2(p) ((p)->parg->parg)
#define sfaram3(p) ((p)->parg->parg->parg)
#define sfaram4(p) ((p)->parg->parg->parg->parg)
#define sfaram5(p) ((p)->parg->parg->parg->parg->parg)
#define sfparamN(p,N) struct sfargument__ *r = p->parg; while((--N)>0) r = r->parg; return r;
/* and so on */


#ifdef __cplusplus
extern "C" {
#endif

/* create formula evaluator structure */
sffe *sffe_alloc(void);
  
/* free fe structure */
void sffe_free(sffe ** parser);
    
/* parse expression 'expression' and strore result in 'parser' struct, error (if any) returned */
sffe_error sffe_parse(sffe ** parser, const char *expression);
    
/* evaulate function and return evaluation result */
SFNumber sffe_eval(sffe * const parser);
    
/* register user function with name 'vname', with 'parcnt' parameters and defined with function pointed by 'funptr'*/
void* sffe_regfunc(sffe ** parser, const char *vname, unsigned int parcnt, sffptr funptr, void *payload);
    
/* get already registered variable pointer, NULL if variable was not registered */
sfvariable* sffe_var(sffe *const parser, const char* name);
    
/* register single variable 'vptrs' identified by name 'vchars' */
//void *sffe_regvar(sffe ** parser, SFNumber * vptrs, char vchars);
sfvariable* sffe_regvar(sffe ** parser, SFNumber * vptrs, const char* name);
    
/* register multiple variables */
void sffe_regvars(sffe ** parser, unsigned int cN, SFNumber ** vptrs, char* const* names);
    
//sffunction *sffe_function_alloc(char *name, sffptr function_pointer, unsigned char paramsCount, void *payload);

//void sffe_function_free(sffunction* function);
    
/* set 'vptrs' as 'vchars' variable  */
SFNumber* sffe_setvar(sffe ** parser, SFNumber vptrs, const char* name);

#ifdef __cplusplus
}
#endif

#ifdef SFFE_DOUBLE
	#include "sffe_real.h"
#endif


#endif
