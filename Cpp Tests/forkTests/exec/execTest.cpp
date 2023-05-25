#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    const char *path = "./helloWorld";
    const char *args = NULL;
    if (!fork())
    {
        execl(path, args, (char *)NULL);
    }
    else
    {
        sleep(1);
    }

    cout << "End of Parent Process with PID: " << getpid() << endl << endl;

    return 0;
}