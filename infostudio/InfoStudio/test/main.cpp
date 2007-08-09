#include <windows.h>

extern void test_querymap();
extern void test_core();

int main(int, char**)
{
    HRESULT hRes = ::CoInitialize(NULL);
    ::DefWindowProc(NULL, 0, 0, 0L);

    test_core();
    test_querymap();
    return 0;
}