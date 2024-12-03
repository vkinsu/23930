#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



int main() {
  struct sockaddr_un addr;
  int fd;


  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }


  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "./riba", sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
  char send[] = "Goodbye, world!\n";
  if(write(fd,send,sizeof(send)) == -1){ 
    printf("didtn write lol\n");
    return -1;
  }
  close(fd);

  return 0;
}