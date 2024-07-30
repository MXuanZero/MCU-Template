#ifndef LIB_ASSERT_H
#define LIB_ASSERT_H
/* Includes ------------------------------------------------------------------*/
/* Define --------------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef NDEBUG
#define lib_assert(ignore) ((void)0)
#else
#define lib_assert(cond)                                            \
	do {                                                        \
		if (!(cond)) {                                      \
			lib_port_assert(#cond, __FILE__, __LINE__); \
		}                                                   \
	} while (0)
#endif
/* Exported typedef ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void lib_port_assert(const char *msg, const char *file, unsigned line);
#endif // LIB_ASSERT_H
