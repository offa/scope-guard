#include <scope.h>

int main()
{
    sr::scope_exit se{[]{}};
    sr::scope_fail sf{[]{}};
    sr::scope_success ss{[]{}};
    sr::unique_resource ur{3, [](auto x){}};

    return 0;
}
