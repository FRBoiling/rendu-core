#ifndef RENDU_COMMON_PLATFORM_PROCESSOR_H_
#define RENDU_COMMON_PLATFORM_PROCESSOR_H_

/* ----------- [Processor detect] ----------- */
#if defined(__x86_64__) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64)
	#define RENDU_PROCESSOR_NAME	"Intel x86-64"
	#define RENDU_PROCESSOR_BITS	64
	#define RENDU_PROCESSOR_AMD64
#elif defined(__i386__) || defined(_M_IX86)
	#define RENDU_PROCESSOR_NAME	"Intel x86"
	#define RENDU_PROCESSOR_BITS	32
	#define RENDU_PROCESSOR_X86
#elif defined (__sparc__)
	#define RENDU_PROCESSOR_NAME	"Sparc"
	#define RENDU_PROCESSOR_SPARC
	#if defined(__arch64__)
		#define RENDU_PROCESSOR_BITS	64
	#else
		#error Sparc 32bit is not supported
	#endif
#elif defined(__ia64__) || defined(_M_IA64)
	#define RENDU_PROCESSOR_NAME	"Intel IA64"
	#define RENDU_PROCESSOR_BITS	64
	#define RENDU_PROCESSOR_IA64
#elif defined(_ARCH_PPC64) || defined(_M_PPC)
	#define RENDU_PROCESSOR_NAME	"IBM PowerPC64"
	#define RENDU_PROCESSOR_BITS	64
	#define RENDU_PROCESSOR_PPC64
#elif defined(__arm__)  || defined(_M_ARM)
	#define RENDU_PROCESSOR_NAME	"ARM"
	#if defined(__aarch64__)
		#define RENDU_PROCESSOR_BITS	64
	#else
		#define RENDU_PROCESSOR_BITS	32
	#endif
	#define RENDU_PROCESSOR_ARM
#endif

#endif  // RENDU_COMMON_PLATFORM_PROCESSOR_H_
