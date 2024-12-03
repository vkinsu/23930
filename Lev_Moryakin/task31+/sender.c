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
  char send0[] = "show\n";
  char send1[] = "hello\n";
  char send2[] = "world\n";
  char send3[] = "close\n";
  write(fd,send0,sizeof(send0));
  usleep(1000);
  write(fd,send1,sizeof(send1));
  usleep(1000);
  write(fd,send2,sizeof(send2));
  usleep(1000);
  write(fd,send3,sizeof(send3));

  close(fd);

  return 0;
}