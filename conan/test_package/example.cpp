#include <math/Vector.h>

#include <cstdlib>

int main()
{
    using namespace ad::math;
    Vec<2, int> a{1, 2};
    a + Vec<2, int>{3, 4};

    return EXIT_SUCCESS;
}
