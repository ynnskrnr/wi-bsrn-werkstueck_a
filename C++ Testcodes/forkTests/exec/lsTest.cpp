#include <unistd.h>
#include <stdlib.h>

int main() {

    execlp("ls", "ls", "./././processInfoLog", NULL);
    return 0;
}