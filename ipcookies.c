#include <sys/types.h>
#include <sys/socket.h>
#define __APPLE_USE_RFC_3542
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <netinet/icmp6.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "ipcookies.h"

void die_perror(char *msg) {
  perror(msg);
  exit(1);
}

void ipcookies_icmp_send(void *buf, struct in6_addr *icmp_dst_addr) {
  static int icmp_sock = -1;
  struct sockaddr_in6 sa_dst;

  if (icmp_sock < 0) {
    icmp_sock = socket(PF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
  }
  if (icmp_sock > 0) {
    /* FIXME: recalculate the checksum here */
    sa_dst.sin6_family = AF_INET6;
    sa_dst.sin6_addr = *icmp_dst_addr;
    sendto(icmp_sock, buf, IPCOOKIES_ICMP_SIZE, 0, (struct sockaddr *)&sa_dst, sizeof(sa_dst));
  }
}

ipcookie_entry_t *ipcookie_find_by_address(ipcookie_full_state_t *ipck, struct in6_addr *src) {
  return NULL;
}

void ipcookie_update_mtime(ipcookie_entry_t *ce) {
  ce->mtime_lo16 = 0xffff & time(NULL);
}

int ipcookie_verify_stateless(ipcookie_t testcookie, struct in6_addr *src) {
  /* FIXME */
  return 0;
}

void ipcookie_set_stateless(ipcookie_t *target_cookie, struct in6_addr *peer) {
  /* FIXME */
}



ipcookie_full_state_t *mmap_ipcookies(void) {
  int fd;
  ipcookie_full_state_t *ipck = NULL;

  fd = shm_open("/ipcookies", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    die_perror("ipcookies shm_open");
  }
  if (ftruncate(fd, sizeof(ipcookie_full_state_t)) == -1) {
    // die_perror("ipcookies ftruncate");
    perror("ipcookies ftruncate");
  }
  ipck = mmap(NULL, sizeof(*ipck),
       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ipck == MAP_FAILED) {
    die_perror("ipcookies mmap");
  }
  close(fd);
  return ipck;
}
