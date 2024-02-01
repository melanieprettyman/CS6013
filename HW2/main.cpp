#include <sys/time.h>
#include <iostream>


extern "C" {

  void _sayHello();
  void _myPuts(const char *s, int length);
  timeval _myGTOD(); 

}


int main(int argc, char **argv) { 
    _myPuts("Hello",5);
     std::cout <<"\n";
    _sayHello();

  timeval currentTime =  _myGTOD();
  std::cout <<"\n"; 
  std::cout << currentTime.tv_sec;
  std::cout <<"\n";
  std::cout << currentTime.tv_usec;	  
  std::cout <<"\n";

    return 0;
}
