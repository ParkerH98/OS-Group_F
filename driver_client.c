#include "parker.c"


// Usage Instructions:
// make c && ./c

int main()
{
    // int port = printRandoms(PORT, PORT + 4);

    clientSocket_SendReceive(PORT);
    return 0;
}