#pragma once

#include <vector>
#include <memory>
#include <cassert>

#include "ndarray.hpp"

/*Not complete.*/

namespace HOLA
{
    
    template<typename T>
    class Vector : public ndarray<T>
    {
    public:
        Vector():ndarray(){}

        template<typename _It>
        Vector(_It begin, _It end)
            :ndarray(begin, end)
        {            
        }

        Vector(const std::vector<T>& data):Vector(data.begin(), data.end()){}



        static float dot(const Vector& a, const Vector& b)
        {
            
        }
 
    };

    template<typename T>
    class Matrix : public ndarray<T>
    {
    public:

        Matrix():ndarray(){}
        Matrix(const std::vector<std::vector<T>>& data)
            :ndarray()
        {
            int h = data.size();
            int w = h > 0 ? data[0].size() : 0;

            int len = w * h;

            _dims = mapper({ h,w });

            _data.reset(new _buffer<T>({ h,w }));

            if(len <= 0) return;

            for(int y = 0; y < h; y++)
            {
                for(int x = 0; x < w; x++)
                {
                    _data.at({ y, x }) = data[y][x];
                }
            }
        }

        static Matrix FromShape(int h, int w)
        {
            Matrix res;
            res._dims = mapper({ h,w });
            res._data.reset(new _buffer<T>({ h,w }));

            return res;
        }

        static Matrix Identity(int n)
        {
            
        }


        Matrix Transpose()
        {
            
        }


        void SwapCol(int a, int b)
        {
            
        }


        void SwapRow(int a, int b)
        {

        }

        T FNorm2()
        {
            
        }

        T FNorm()
        {
            
        }

        void GaussianEliminate(int row, int col)
        {

        }

        /// <summary>
        /// QR decomposition with column pivoting, A*P = Q*R
        /// </summary>
        /// <param name="A">Decomp matrix, also R output</param>
        /// <param name="Q">Unary matrix</param>
        /// <param name="P">Permutation matrix</param>
        /// <returns>matrix rank</returns>
        static int ColPivotQR(Matrix& A, Matrix& Q, std::vector<int>& P)
        {
        }

        int ColPivotQR(Matrix& Q, std::vector<int>& P)
        {
            return ColPivotQR(*this, Q, P);
        }
    };

}


