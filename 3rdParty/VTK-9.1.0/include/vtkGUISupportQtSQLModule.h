
#ifndef VTKGUISUPPORTQTSQL_EXPORT_H
#define VTKGUISUPPORTQTSQL_EXPORT_H

#ifdef VTKGUISUPPORTQTSQL_STATIC_DEFINE
#  define VTKGUISUPPORTQTSQL_EXPORT
#  define VTKGUISUPPORTQTSQL_NO_EXPORT
#else
#  ifndef VTKGUISUPPORTQTSQL_EXPORT
#    ifdef GUISupportQtSQL_EXPORTS
        /* We are building this library */
#      define VTKGUISUPPORTQTSQL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define VTKGUISUPPORTQTSQL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef VTKGUISUPPORTQTSQL_NO_EXPORT
#    define VTKGUISUPPORTQTSQL_NO_EXPORT 
#  endif
#endif

#ifndef VTKGUISUPPORTQTSQL_DEPRECATED
#  define VTKGUISUPPORTQTSQL_DEPRECATED __declspec(deprecated)
#endif

#ifndef VTKGUISUPPORTQTSQL_DEPRECATED_EXPORT
#  define VTKGUISUPPORTQTSQL_DEPRECATED_EXPORT VTKGUISUPPORTQTSQL_EXPORT VTKGUISUPPORTQTSQL_DEPRECATED
#endif

#ifndef VTKGUISUPPORTQTSQL_DEPRECATED_NO_EXPORT
#  define VTKGUISUPPORTQTSQL_DEPRECATED_NO_EXPORT VTKGUISUPPORTQTSQL_NO_EXPORT VTKGUISUPPORTQTSQL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VTKGUISUPPORTQTSQL_NO_DEPRECATED
#    define VTKGUISUPPORTQTSQL_NO_DEPRECATED
#  endif
#endif

#endif /* VTKGUISUPPORTQTSQL_EXPORT_H */
