#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>

using namespace std;

int main()
{
    int cpu, status;
    string s = "/proc/"+to_string(getpid())+"/status";
    const char *c = s.c_str();
    status = syscall(SYS_getcpu, &cpu, NULL, NULL);
    cout << endl << "Runns on Core: " << cpu << endl << endl;
    //
    cout << endl << "status: " << system(c) << endl << endl;

    return 0;
}