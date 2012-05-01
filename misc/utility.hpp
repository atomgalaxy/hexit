#ifndef UTILITY_HPP_GUARD
#define UTILITY_HPP_GUARD
/**
 * @file utility.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-05-01
 */

#include <cstdlib> // for rand()

namespace utility {

int rand(unsigned int start, unsigned int end)
{
    return std::rand()%(end-start)+start;
}

/** Knuth Shuffle */
template <typename Array>
void random_shuffle(Array& a)
{
    /*tuki pride random shuffle*/
    for (size_t i = a.size()-1; i > 1; i--) {
        size_t j = rand(0, i);
        // switch the ith and jth element
        auto temp = a[j];
        a[j] = a[i];
        a[i] = temp;
    }
}



}

#endif
