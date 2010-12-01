#define _XOPN_SOURCE 500
#include <features.h>
#include <assert.h>
#define __USE_GNU
#include <dlfcn.h>
#undef __USE_GNU

#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

static int (*orig_bind)(int sockfd, const struct sockaddr* addr, socklen_t addrlen);

__attribute__((constructor))
extern void bindlocal_init(void)
{
  orig_bind = dlsym(RTLD_NEXT, "bind");
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
  struct sockaddr_in* addr_in = (struct sockaddr_in *)addr;
  struct sockaddr_in override_addr_in;
  
  if (sizeof(struct sockaddr_in) == addrlen
      && addr_in->sin_family == AF_INET
      && addr_in->sin_addr.s_addr == htonl(0)
      && addr_in->sin_port != htons(0)) {
    memcpy(&override_addr_in, addr_in, sizeof(struct sockaddr_in));
    override_addr_in.sin_addr.s_addr = htonl(0x7f000001);    
    addr = (struct sockaddr*)&override_addr_in;
  }
  
  return (*orig_bind)(sockfd, addr, addrlen);
}
