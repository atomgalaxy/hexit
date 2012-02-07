#ifndef SGR_EXCEPTIONS_HPP
#define SGR_EXCEPTIONS_HPP
/**
 * @file exceptions.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-06
 */
#include <string>
#include <boost/exception/all.hpp>

namespace sgr {
namespace err {

    struct exception_base : virtual std::exception, virtual boost::exception {};
    struct invalid_time : virtual exception_base {};
    struct invalid_beat : virtual exception_base {};
    struct end_of_song  : virtual exception_base {};
    typedef boost::error_info<struct tag_time_value, double> time_val;
    typedef boost::error_info<struct tag_beat_value, double> beat_val;
    typedef boost::error_info<struct tag_reason, std::string> reason;

}
}

#endif
