#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>
#define DELETE_SYMBOL "\33[D\33[K"
#define DELETION "\33[2K\r"
using namespace std;
struct termios current_settings, new_settings;

void restore() {
  tcsetattr(STDIN_FILENO,TCSAFLUSH, &current_settings);
}

int main(){
    tcgetattr(STDIN_FILENO, &current_settings);
    atexit(restore);
    new_settings = current_settings;
    new_settings.c_iflag &= ~IGNCR;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ICANON;
    
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_settings);
    string str,word;
    char symbol;
    int count = 0;
    int found_char;
    while(1){
      read(STDIN_FILENO,&symbol,1);
      if(symbol == '\n')continue;
      if(symbol == new_settings.c_cc[VERASE]){
        if(count == 0)continue;
        write(STDIN_FILENO,DELETE_SYMBOL,strlen(DELETE_SYMBOL));
        str.pop_back();
        if(count > 0)count--; 
        continue;
      }
      if(symbol == new_settings.c_cc[VKILL]){
        write(STDIN_FILENO,DELETION,strlen(DELETION));
        str = "";
        count = 0;
        continue;
      }
      if(symbol == new_settings.c_cc[VWERASE]){
        found_char = 0;
        for(; count > 0; count--){
          if(str[count - 1] != ' ')found_char = 1;
          if(str[count - 1] == ' ' && found_char == 1)break;
          write(STDIN_FILENO,DELETE_SYMBOL,strlen(DELETE_SYMBOL));
          str.pop_back();
        }
        continue;
      }
      if(symbol == '\4'){
        if(count == 0)return 0;
        continue;
      }
      str += symbol;
      write(STDIN_FILENO,&symbol,1);
      write(STDIN_FILENO,"\7",1);
      count++;

      if(count > 40){
        word = "";
        for(; count > 0;count--){
          if(str[count - 1] == ' ')break;
          word += str[count - 1];
          write(STDIN_FILENO,DELETE_SYMBOL,strlen(DELETE_SYMBOL));
        }
        str = "";
        for(int i = word.length() - 1; i >= 0; i--)
          str += word[i];
        count = word.length();

        write(STDIN_FILENO,"\n",1);
        write(STDIN_FILENO,word.c_str(),word.length());
      }
    }
}