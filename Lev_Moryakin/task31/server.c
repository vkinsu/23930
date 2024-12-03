#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <sys/select.h>
#include <termios.h>

int main() {
  unlink("./riba");
  struct sockaddr_un addr;
    
  int fd,rc;


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

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }



  char buf[256];
  int rd; 
    int max_fd;
    fd_set read_fd,tmp_fd;
    int ignore;
    int opened;
    int selected;
    struct sockaddr_un cl_addr;
    int size = sizeof(cl_addr);
    FD_ZERO(&tmp_fd);
    FD_SET(fd,&tmp_fd);
    max_fd = fd;
    
    while(1){
        read_fd = tmp_fd;
        printf("waiting...\n");
        selected = select(max_fd + 1,&read_fd, NULL,NULL,NULL);
        for(int i = 0; i < max_fd + 1; i++){
            if(!FD_ISSET(i,&read_fd))continue;
            if(i == fd){
                size = sizeof(cl_addr);
                opened = accept(fd,(struct sockaddr *)&cl_addr, &size);
                if(opened > max_fd)max_fd = opened;
                FD_SET(opened, &tmp_fd);
                printf("opened descriptor %d\n",opened);
            }
            else{
                rd = read(i,buf,sizeof(buf) - 1);
                if(!rd){
                    close(i);
                    FD_CLR(i,&tmp_fd);
                    printf("closed %d\n",i);
                    continue;
                }
                buf[rd] = '\0';
                if(strcmp(buf,"close\n") == 0){close(fd); return 1;}
                printf("read bytes from %d\n",i);
                for(int j = 0; j < sizeof(buf); j++)
                    buf[j] = toupper(buf[j]);
                printf("%s",buf);
            }
        }
    }
  
  close(fd);

  return 0;
}
