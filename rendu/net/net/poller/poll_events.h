/*
* Created by boil on 2024/3/12.
*/

#ifndef RENDU_NET_NET_POLLER_POLL_EVENTS_H_
#define RENDU_NET_NET_POLLER_POLL_EVENTS_H_

#include "net_define.h"

NET_NAMESPACE_BEGIN

enum PollEvents {
  RD_POLLNONE = 0,
  RD_POLLIN = 0x0001,      /* any readable data available */

  RD_POLLPRI = 0x0002,     /* OOB/Urgent readable data */
  RD_POLLOUT = 0x0004,     /* file descriptor is writeable */
  RD_POLLRDNORM = 0x0040,  /* non-OOB/URG data available */
  RD_POLLWRNORM = RD_POLLOUT, /* no write type differentiation */
  RD_POLLRDBAND = 0x0080,  /* OOB/Urgent readable data */
  RD_POLLWRBAND = 0x0100,  /* OOB/Urgent data can be written */

  /*
 * FreeBSD extensions: polling on a regular file might return one
 * of these events (currently only supported on local filesystems).
 */
  RD_POLLEXTEND = 0x0200, /* file may have been extended */
  RD_POLLATTRIB = 0x0400, /* file attributes may have changed */
  RD_POLLNLINK = 0x0800,  /* (un)link/rename may have happened */
  RD_POLLWRITE = 0x1000,  /* file's contents may have changed */

  /*
 * These events are set if they occur regardless of whether they were
 * requested.
 */
  RD_POLLERR = 0x0008,  /* some poll error occurred */
  RD_POLLHUP = 0x0010,  /* file descriptor was "hung up" */
  RD_POLLNVAL = 0x0020, /* requested events "invalid" */

  RD_POLLSTANDARD = (RD_POLLIN | RD_POLLPRI | RD_POLLOUT | RD_POLLRDNORM | RD_POLLRDBAND | RD_POLLWRBAND | RD_POLLERR | RD_POLLHUP | RD_POLLNVAL)
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_POLLER_POLL_EVENTS_H_
