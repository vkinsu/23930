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
  printf("socket number: %d\n",fd);

  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "./riba", sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
  char buf[256];
  int len; 
  while(1){
    fgets(buf,sizeof(buf), stdin);
    if(!write(fd,buf,strlen(buf))){ 
      printf("didnt write lol\n");
      return -1;
    }
    if(strcmp(buf,"close\n") == 0)return 1;
  }
  
  return 0;
}