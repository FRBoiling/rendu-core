#ifndef RENDU_COMMON_PLATFORM_WINDOWS_H_
#define RENDU_COMMON_PLATFORM_WINDOWS_H_

#if defined(RENDU_PLATFORM_LINUX)
	#include <sys/types.h>
	#include <sys/ioctl.h>
	#include <sys/time.h>
	#include <sys/socket.h>
	#include <poll.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <signal.h>
	#include <dlfcn.h>
	#include <unistd.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/ptrace.h>

	// Definition
	#define RENDU_SLEEP(ms)            usleep(ms * 1000)
	#define RENDU_DEBUGGING()          (ptrace(PT_TRACE_ME, 0, 0, 0) < 0)
	#define RENDU_BREAKPOINT()			asm("int3")

	#define RENDU_API_EXPORT			__attribute__((visibility("default")))
	#define RENDU_API_IMPORT			__attribute__((visibility("default")))

	#define RENDU_MODULE_HANDLE        void*
	#define RENDU_MODULE_LOAD(a)       dlopen((a), RTLD_LAZY | RTLD_GLOBAL)
	#define RENDU_MODULE_GETSYM(a, b)  dlsym(a, b)
	#define RENDU_MODULE_UNLOAD(a)     dlclose(a)
	#define RENDU_MODULE_EXTENSION     ".so"

	#define RENDU_SEPARATOR '/'

#endif // RENDU_PLATFORM_LINUX
#endif // RENDU_COMMON_PLATFORM_WINDOWS_H_
