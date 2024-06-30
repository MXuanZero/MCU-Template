/* Includes */
#include "gd32f30x.h"
#include "systick.h"
#include <sys/stat.h>
#include <errno.h>
#include <sys/times.h>


/* Variables */
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	(void)pid;
	(void)sig;
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}    /* Make sure we hang here */
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
	(void)file;
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		*ptr++ = __io_getchar();
	}

	return len;
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
	(void)file;
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}

int _close(int file)
{
	(void)file;
	return -1;
}


int _fstat(int file, struct stat *st)
{
	(void)file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	(void)file;
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	(void)file;
	(void)ptr;
	(void)dir;
	return 0;
}

int _open(char *path, int flags, ...)
{
	(void)path;
	(void)flags;
	/* Pretend like we always fail */
	return -1;
}

int _wait(int *status)
{
	(void)status;
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	(void)name;
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	(void)buf;
	return -1;
}

int _stat(char *file, struct stat *st)
{
	(void)file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	(void)old;
	(void)new;
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	(void)name;
	(void)argv;
	(void)env;
	errno = ENOMEM;
	return -1;
}

static uint8_t *__sbrk_heap_end = NULL;
void *_sbrk(ptrdiff_t incr)
{
  extern uint8_t _end; /* Symbol defined in the linker script */
  extern uint8_t _estack; /* Symbol defined in the linker script */
  extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */
  const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
  const uint8_t *max_heap = (uint8_t *)stack_limit;
  uint8_t *prev_heap_end;

  /* Initialize heap end at first call */
  if (NULL == __sbrk_heap_end)
  {
    __sbrk_heap_end = &_end;
  }

  /* Protect heap from growing into the reserved MSP stack */
  if (__sbrk_heap_end + incr > max_heap)
  {
    errno = ENOMEM;
    return (void *)-1;
  }

  prev_heap_end = __sbrk_heap_end;
  __sbrk_heap_end += incr;

  return (void *)prev_heap_end;
}


static volatile uint32_t sys_delay_ms = 0;
void systick_config(void)
{
	/* setup systick timer for 1000Hz interrupts */
	if (SysTick_Config(SystemCoreClock / 1000U)){
		/* capture error */
		while (1){
		}
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay(uint32_t count)
{
	sys_delay_ms = count;
	while(0U != sys_delay_ms){
	}
}

void delay_decrement(void)
{
	if (0U != sys_delay_ms){
		sys_delay_ms--;
	}
}
