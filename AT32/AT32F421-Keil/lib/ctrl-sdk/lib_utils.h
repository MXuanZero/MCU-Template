#ifndef LIB_UTILS_H
#define LIB_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
/* Define ----------------------------------------------------------------------------------------*/
/* Exported macro --------------------------------------------------------------------------------*/
/* Exported typedef ------------------------------------------------------------------------------*/
/* Exported constants ----------------------------------------------------------------------------*/
#ifdef DEF_GIT
extern const char* lib_utils_git_tags;
#endif
extern const char* lib_utils_build_date;
extern const char* lib_utils_build_time;
/* Exported functions ----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif //LIB_UTILS_H
