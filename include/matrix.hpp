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
        
    };

}


