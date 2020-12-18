#pragma once

#include "matrix.hpp"

namespace HOLA
{
    /*Linear Programming*/
    namespace LP
    {
        /*Least square solvers*/
        template<typename T>
        Matrix<T> FastNNLSSolve(const Matrix<T>& A, const Matrix<T>& b, int iter_max = 1e2)
        {
            
        }

        template<typename T>
        Matrix<T> QRCPLSSolve(const Matrix<T>& A, const Matrix<T>& b)
        {

        }

    }

    /*Linear Algebra*/
    namespace LA
    {
        enum class SimplexResult { Infeasible, Unbounded, Done };

        template<typename T>
        SimplexResult SimplexSolve(
            const Matrix<T>& C, 
            const Matrix<T>& A, 
            const Matrix<T>& b, 
            const Matrix<T>& x, 
            T& min)
        {

        }
    }

}
