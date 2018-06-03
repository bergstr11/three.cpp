//
// Created by csell on 02.06.2018.
//

#ifndef THREE_PP_OSDECL_H
#define THREE_PP_OSDECL_H

#ifdef _WIN32
# ifdef COMPILE_THREEPP_DLL
#   define DLX __declspec(dllexport)
# else
#   ifdef USE_THREEPP_DLL
#     define DLX __declspec(dllimport)
#   else
#     define DLX
#   endif
# endif
#else
# define DLX
#endif //_WIN32

#endif //THREE_PP_OSDECL_H
