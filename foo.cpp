
#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>

int main()
{

  time_t now = time(0);
  
  struct tm tstruct;

  tstruct = *localtime(&now);
  char buf[80];
  
  strftime(buf,sizeof(buf),"%Y-%m-%d %X",&tstruct);
  printf("Current time is %s\n",buf);

  //std::string date1 = "2012-12-06 14:28:51";
  std::string date1 = "2012-12-07";
  //std::string date2 = "2012-12-06 14:28:52";
  std::string date2 = "2013-02-10 23:59:59";

  date1 = buf;

  if (date1 < date2) {
    printf("%s is before %s\n",date1.c_str(),date2.c_str());
  }
  else {
    printf("%s is *after* %s\n",date1.c_str(),date2.c_str());
  }
}
