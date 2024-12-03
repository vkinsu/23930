#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>


int main() {
  unlink("./riba");
  struct sockaddr_un addr;
  int fd,cl,rc;


  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, "./riba", sizeof(addr.sun_path)-1);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 1) == -1) {
    perror("listen error");
    exit(-1);
  }

  if (cl = accept(fd, NULL, NULL), cl == -1) {
    perror("accept error");
  }

  char bytes[256];
  int rd; 
  while(rd = read(cl,bytes,sizeof(bytes)), rd > 0){
    for(int i = 0; i < sizeof(bytes); i++){
      if((int)bytes[i] > 96 && (int)bytes[i] < 123)bytes[i] -= 32; 
    }
    printf("%s",bytes);
  }
  
  close(cl);
  close(fd);

  return 0;
}
