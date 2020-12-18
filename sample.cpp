#include "stdio.h"

#include "include/ndarray.hpp"
#include "include/HOLA.hpp"

int main()
{
    auto A = HOLA::ndarray<float>::FromShape({ 4,4 });

    A[{HOLA::mapping_info::range(0, 2), HOLA::mapping_info::range(1, 3)}] = 3;
    auto A_ = A[{HOLA::mapping_info::range(1, 3), HOLA::mapping_info::range(1, 3)}];

    HOLA::ndarray<float> arr({
        HOLA::ndarray<float>({ 1,2,3 }),
        HOLA::ndarray<float>({ 4, 5,6 })
    }
    );

    HOLA::Matrix<float> M({ {1,2} });

    auto B = A_ * arr;

    printf("Hello.");
}
