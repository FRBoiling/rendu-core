/*
* Created by boil on 2023/10/25.
*/

#ifndef RENDU_FILE_EVENT_H
#define RENDU_FILE_EVENT_H

typedef void FileProc(int fd, void *clientData, int mask);

struct FileEvent {
  int m_mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
  FileProc *rfileProc;
  FileProc *wfileProc;
  void *clientData;
};

struct FiredEvent {
  int m_fd;
  int m_mask;
} ;

#endif //RENDU_FILE_EVENT_H
