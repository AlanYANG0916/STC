//
// MATLAB Compiler: 6.2 (R2016a)
// Date: Tue Sep 12 10:09:26 2017
// Arguments: "-B" "macro_default" "-W" "cpplib:MATRICDIV" "-T" "link:lib"
// "matricdiv.m" "-C" 
//

#include <stdio.h>
#define EXPORTING_MATRICDIV 1
#include "MATRICDIV.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_MATRICDIV_C_API
#define LIB_MATRICDIV_C_API /* No special import/export declaration */
#endif

LIB_MATRICDIV_C_API 
bool MW_CALL_CONV MATRICDIVInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("MATRICDIV"), path_to_dll, _MAX_PATH))
    return false;
    bResult = mclInitializeComponentInstanceNonEmbeddedStandalone(  &_mcr_inst,
                                                                    path_to_dll,
                                                                    "MATRICDIV",
                                                                    LibTarget,
                                                                    error_handler, 
                                                                    print_handler);
    if (!bResult)
    return false;
  return true;
}

LIB_MATRICDIV_C_API 
bool MW_CALL_CONV MATRICDIVInitialize(void)
{
  return MATRICDIVInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_MATRICDIV_C_API 
void MW_CALL_CONV MATRICDIVTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_MATRICDIV_C_API 
void MW_CALL_CONV MATRICDIVPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_MATRICDIV_C_API 
bool MW_CALL_CONV mlxMatricdiv(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "matricdiv", nlhs, plhs, nrhs, prhs);
}

LIB_MATRICDIV_CPP_API 
void MW_CALL_CONV matricdiv(int nargout, mwArray& vector2, const mwArray& pitch, const 
                            mwArray& roll, const mwArray& rotation, const mwArray& tx, 
                            const mwArray& ty, const mwArray& tz)
{
  mclcppMlfFeval(_mcr_inst, "matricdiv", nargout, 1, 6, &vector2, &pitch, &roll, &rotation, &tx, &ty, &tz);
}

