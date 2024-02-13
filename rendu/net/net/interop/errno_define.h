/*
* Created by boil on 2024/2/20.
*/

#ifndef RENDU_NET_NET_INTEROP_CROSS_PLATFORM_RD_ERRNO_H_
#define RENDU_NET_NET_INTEROP_CROSS_PLATFORM_RD_ERRNO_H_

#include "net_define.h"

#define FAIL (-1)
#define SUCCESS 0

#define RD_ERROR_CODE_NUM_BEGINE 65536 // 0x00010000

#define RD__FAIL                FAIL                /* Operation fail */
#define RD__SUCCESS             SUCCESS             /* Operation success */

#define RD__EPERM               RD_ERROR_CODE_NUM_BEGINE  +    1             /* Operation not permitted */
#define RD__ENOENT              RD_ERROR_CODE_NUM_BEGINE  +    2             /* No such file or directory */
#define RD__ESRCH               RD_ERROR_CODE_NUM_BEGINE  +    3             /* No such process */
#define RD__EINTR               RD_ERROR_CODE_NUM_BEGINE  +    4             /* Interrupted system call */
#define RD__EIO                 RD_ERROR_CODE_NUM_BEGINE  +    5             /* Input/output error */
#define RD__ENXIO               RD_ERROR_CODE_NUM_BEGINE  +    6             /* Device not configured */
#define RD__E2BIG               RD_ERROR_CODE_NUM_BEGINE  +    7             /* Argument list too long */
#define RD__ENOEXEC             RD_ERROR_CODE_NUM_BEGINE  +    8             /* Exec format error */
#define RD__EBADF               RD_ERROR_CODE_NUM_BEGINE  +    9             /* Bad file descriptor */
#define RD__ECHILD              RD_ERROR_CODE_NUM_BEGINE  +    10             /* No child processes */
#define RD__EDEADLK             RD_ERROR_CODE_NUM_BEGINE  +    11             /* Resource deadlock avoided */
#define RD__ENOMEM              RD_ERROR_CODE_NUM_BEGINE  +    12             /* Cannot allocate memory */
#define RD__EACCES              RD_ERROR_CODE_NUM_BEGINE  +    13             /* Permission denied */
#define RD__EFAULT              RD_ERROR_CODE_NUM_BEGINE  +    14             /* Bad address */
#define RD__ENOTBLK             RD_ERROR_CODE_NUM_BEGINE  +    15             /* Block device required */
#define RD__EBUSY               RD_ERROR_CODE_NUM_BEGINE  +    16             /* Device / Resource busy */
#define RD__EEXIST              RD_ERROR_CODE_NUM_BEGINE  +    17             /* File exists */
#define RD__EXDEV               RD_ERROR_CODE_NUM_BEGINE  +    18             /* Cross-device link */
#define RD__ENODEV              RD_ERROR_CODE_NUM_BEGINE  +    19             /* Operation not supported by device */
#define RD__ENOTDIR             RD_ERROR_CODE_NUM_BEGINE  +    20             /* Not a directory */
#define RD__EISDIR              RD_ERROR_CODE_NUM_BEGINE  +    21             /* Is a directory */
#define RD__EINVAL              RD_ERROR_CODE_NUM_BEGINE  +    22             /* Invalid argument */
#define RD__ENFILE              RD_ERROR_CODE_NUM_BEGINE  +    23             /* Too many open files in system */
#define RD__EMFILE              RD_ERROR_CODE_NUM_BEGINE  +    24             /* Too many open files */
#define RD__ENOTTY              RD_ERROR_CODE_NUM_BEGINE  +    25             /* Inappropriate ioctl for device */
#define RD__ETXTBSY             RD_ERROR_CODE_NUM_BEGINE  +    26             /* Text file busy */
#define RD__EFBIG               RD_ERROR_CODE_NUM_BEGINE  +    27             /* File too large */
#define RD__ENOSPC              RD_ERROR_CODE_NUM_BEGINE  +    28             /* No space left on device */
#define RD__ESPIPE              RD_ERROR_CODE_NUM_BEGINE  +    29             /* Illegal seek */
#define RD__EROFS               RD_ERROR_CODE_NUM_BEGINE  +    30             /* Read-only file system */
#define RD__EMLINK              RD_ERROR_CODE_NUM_BEGINE  +    31             /* Too many links */
#define RD__EPIPE               RD_ERROR_CODE_NUM_BEGINE  +    32             /* Broken pipe */
#define RD__EDOM                RD_ERROR_CODE_NUM_BEGINE  +    33             /* Numerical argument out of domain */
#define RD__ERANGE              RD_ERROR_CODE_NUM_BEGINE  +    34             /* Result too large */
#define RD__EAGAIN              RD_ERROR_CODE_NUM_BEGINE  +    35             /* Resource temporarily unavailable */
#define RD__EWOULDBLOCK         RD_ERROR_CODE_NUM_BEGINE  +    36                         /* O  p    eration would block */
#define RD__EINPROGRESS         RD_ERROR_CODE_NUM_BEGINE  +    37             /* Operation now in progress */
#define RD__EALREADY            RD_ERROR_CODE_NUM_BEGINE  +    38             /* Operation already in progress */
#define RD__ENOTSOCK            RD_ERROR_CODE_NUM_BEGINE  +    39             /* Socket operation on non-socket */
#define RD__EDESTADDRREQ        RD_ERROR_CODE_NUM_BEGINE  +    40             /* Destination address required */
#define RD__EMSGSIZE            RD_ERROR_CODE_NUM_BEGINE  +    41             /* Message too long */
#define RD__EPROTOTYPE          RD_ERROR_CODE_NUM_BEGINE  +    42             /* Protocol wrong type for socket */
#define RD__ENOPROTOOPT         RD_ERROR_CODE_NUM_BEGINE  +    43             /* Protocol not available */
#define RD__EPROTONOSUPPORT     RD_ERROR_CODE_NUM_BEGINE  +    44             /* Protocol not supported */
#define RD__ESOCKTNOSUPPORT     RD_ERROR_CODE_NUM_BEGINE  +    45             /* Socket type not supported */
#define RD__ENOTSUP             RD_ERROR_CODE_NUM_BEGINE  +    46             /* Operation not supported */
#define RD__EOPNOTSUPP          RD_ERROR_CODE_NUM_BEGINE  +    47                                   /* Op  e    ration not supported on socket */
#define RD__EPFNOSUPPORT        RD_ERROR_CODE_NUM_BEGINE  +    48             /* Protocol family not supported */
#define RD__EAFNOSUPPORT        RD_ERROR_CODE_NUM_BEGINE  +    49             /* Address family not supported by protocol family */
#define RD__EADDRINUSE          RD_ERROR_CODE_NUM_BEGINE  +    50             /* Address already in use */
#define RD__EADDRNOTAVAIL       RD_ERROR_CODE_NUM_BEGINE  +    51             /* Can't assign requested address */
#define RD__ENETDOWN            RD_ERROR_CODE_NUM_BEGINE  +    52             /* Network is down */
#define RD__ENETUNREACH         RD_ERROR_CODE_NUM_BEGINE  +    53             /* Network is unreachable */
#define RD__ENETRESET           RD_ERROR_CODE_NUM_BEGINE  +    54             /* Network dropped connection on reset */
#define RD__ECONNABORTED        RD_ERROR_CODE_NUM_BEGINE  +    55             /* Software caused connection abort */
#define RD__ECONNRESET          RD_ERROR_CODE_NUM_BEGINE  +    56             /* Connection reset by peer */
#define RD__ENOBUFS             RD_ERROR_CODE_NUM_BEGINE  +    57             /* No buffer space available */
#define RD__EISCONN             RD_ERROR_CODE_NUM_BEGINE  +    58             /* Socket is already connected */
#define RD__ENOTCONN            RD_ERROR_CODE_NUM_BEGINE  +    59             /* Socket is not connected */
#define RD__ESHUTDOWN           RD_ERROR_CODE_NUM_BEGINE  +    60             /* Can't send after socket shutdown */
#define RD__ETOOMANYREFS        RD_ERROR_CODE_NUM_BEGINE  +    61             /* Too many references: can't splice */
#define RD__ETIMEDOUT           RD_ERROR_CODE_NUM_BEGINE  +    62             /* Operation timed out */
#define RD__ECONNREFUSED        RD_ERROR_CODE_NUM_BEGINE  +    63             /* Connection refused */
#define RD__ELOOP               RD_ERROR_CODE_NUM_BEGINE  +    64             /* Too many levels of symbolic links */
#define RD__ENAMETOOLONG        RD_ERROR_CODE_NUM_BEGINE  +    65             /* File name too long */
#define RD__EHOSTDOWN           RD_ERROR_CODE_NUM_BEGINE  +    66             /* Host is down */
#define RD__EHOSTUNREACH        RD_ERROR_CODE_NUM_BEGINE  +    67             /* No route to host */
#define RD__ENOTEMPTY           RD_ERROR_CODE_NUM_BEGINE  +    68             /* Directory not empty */
#define RD__EPROCLIM            RD_ERROR_CODE_NUM_BEGINE  +    69             /* Too many processes */
#define RD__EUSERS              RD_ERROR_CODE_NUM_BEGINE  +    70             /* Too many users */
#define RD__EDQUOT              RD_ERROR_CODE_NUM_BEGINE  +    71             /* Disc quota exceeded */
#define RD__ESTALE              RD_ERROR_CODE_NUM_BEGINE  +    72             /* Stale NFS file handle */
#define RD__EREMOTE             RD_ERROR_CODE_NUM_BEGINE  +    73             /* Too many levels of remote in path */
#define RD__EBADRPC             RD_ERROR_CODE_NUM_BEGINE  +    74             /* RPC struct is bad */
#define RD__ERPCMISMATCH        RD_ERROR_CODE_NUM_BEGINE  +    75             /* RPC version wrong */
#define RD__EPROGUNAVAIL        RD_ERROR_CODE_NUM_BEGINE  +    76             /* RPC prog. not avail */
#define RD__EPROGMISMATCH       RD_ERROR_CODE_NUM_BEGINE  +    77             /* Program version wrong */
#define RD__EPROCUNAVAIL        RD_ERROR_CODE_NUM_BEGINE  +    78             /* Bad procedure for program */
#define RD__ENOLCK              RD_ERROR_CODE_NUM_BEGINE  +    79             /* No locks available */
#define RD__ENOSYS              RD_ERROR_CODE_NUM_BEGINE  +    80             /* Function not implemented */
#define RD__EFTYPE              RD_ERROR_CODE_NUM_BEGINE  +    81             /* Inappropriate file type or format */
#define RD__EAUTH               RD_ERROR_CODE_NUM_BEGINE  +    82             /* Authentication error */
#define RD__ENEEDAUTH           RD_ERROR_CODE_NUM_BEGINE  +    83             /* Need authenticator */
#define RD__EPWROFF             RD_ERROR_CODE_NUM_BEGINE  +    84     /* Device power is off */
#define RD__EDEVERR             RD_ERROR_CODE_NUM_BEGINE  +    85     /* Device error, e.g. paper out */
#define RD__EOVERFLOW           RD_ERROR_CODE_NUM_BEGINE  +    86             /* Value too large to be stored in data type */
#define RD__EBADEXEC            RD_ERROR_CODE_NUM_BEGINE  +    87     /* Bad executable */
#define RD__EBADARCH            RD_ERROR_CODE_NUM_BEGINE  +    88     /* Bad CPU type in executable */
#define RD__ESHLIBVERS          RD_ERROR_CODE_NUM_BEGINE  +    89     /* Shared library version mismatch */
#define RD__EBADMACHO           RD_ERROR_CODE_NUM_BEGINE  +    90     /* Malformed Macho file */
#define RD__ECANCELED           RD_ERROR_CODE_NUM_BEGINE  +    91             /* Operation canceled */
#define RD__EIDRM               RD_ERROR_CODE_NUM_BEGINE  +    92             /* Identifier removed */
#define RD__ENOMSG              RD_ERROR_CODE_NUM_BEGINE  +    93             /* No message of desired type */
#define RD__EILSEQ              RD_ERROR_CODE_NUM_BEGINE  +    94             /* Illegal byte sequence */
#define RD__ENOATTR             RD_ERROR_CODE_NUM_BEGINE  +    95             /* Attribute not found */
#define RD__EBADMSG             RD_ERROR_CODE_NUM_BEGINE  +    96             /* Bad message */
#define RD__EMULTIHOP           RD_ERROR_CODE_NUM_BEGINE  +    97             /* Reserved */
#define RD__ENODATA             RD_ERROR_CODE_NUM_BEGINE  +    98             /* No message available on STREAM */
#define RD__ENOLINK             RD_ERROR_CODE_NUM_BEGINE  +    99             /* Reserved */
#define RD__ENOSR               RD_ERROR_CODE_NUM_BEGINE  +    100             /* No STREAM resources */
#define RD__ENOSTR              RD_ERROR_CODE_NUM_BEGINE  +    101             /* Not a STREAM */
#define RD__EPROTO              RD_ERROR_CODE_NUM_BEGINE  +    102             /* Protocol error */
#define RD__ETIME               RD_ERROR_CODE_NUM_BEGINE  +    103             /* STREAM ioctl timeout */
#define RD__ENOPOLICY           RD_ERROR_CODE_NUM_BEGINE  +    104             /* No such policy registered */
#define RD__ENOTRECOVERABLE     RD_ERROR_CODE_NUM_BEGINE  +    105             /* State not recoverable */
#define RD__EOWNERDEAD          RD_ERROR_CODE_NUM_BEGINE  +    106             /* Previous owner died */
#define RD__EHOSTNOTFOUND       RD_ERROR_CODE_NUM_BEGINE  +    107             /* Host not found */
#define RD__ESOCKETERROR        RD_ERROR_CODE_NUM_BEGINE  +    108             /* Socket error */
#define RD__EQFULL              RD_ERROR_CODE_NUM_BEGINE  +    109             /* Interface output queue is full */
#define RD__ELAST               RD_ERROR_CODE_NUM_BEGINE  +    110             /* Must be equal largest errno */

#endif//RENDU_NET_NET_INTEROP_CROSS_PLATFORM_RD_ERRNO_H_
