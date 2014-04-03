//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
#define AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED

#include <map>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>
#include <typeinfo>

#include <boost/none_t.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ajg/synth/value_iterator.hpp>

namespace ajg {
namespace synth {
namespace detail {
inline std::string get_type_name(std::type_info const& info) {
    return info.name(); // TODO: Unmangle where needed.
}
} // namespace detail

//
// conversion_error (TODO: Consider renaming bad_conversion.)
////////////////////////////////////////////////////////////////////////////////////////////////////

struct conversion_error : public std::runtime_error {
    conversion_error(std::type_info const& a, std::type_info const& b)
        : std::runtime_error("could not convert value of type `"
                                 + detail::get_type_name(a) + "' to `"
                                 + detail::get_type_name(b) + "'") {}
    ~conversion_error() throw () {}
};

//
// default_value_traits
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Char, class Value>
struct default_value_traits {
  public:

    typedef default_value_traits                        self_type;
    typedef boost::none_t                               none_type;
    typedef Char                                        char_type;
    typedef std::size_t                                 size_type;
    typedef Value                                       value_type;
    typedef bool                                        boolean_type;
    typedef double                                      number_type;
    typedef boost::gregorian::date                      date_type;
 // typedef TODO                                        time_type;
    typedef boost::posix_time::ptime                    datetime_type;
    typedef boost::posix_time::time_duration            duration_type;
    typedef std::basic_string<char_type>                string_type;
    typedef std::basic_istream<char_type>               istream_type;
    typedef std::basic_ostream<char_type>               ostream_type;

    typedef std::vector<value_type>                     sequence_type;
    typedef std::map<string_type, value_type>           mapping_type;

    typedef value_iterator<value_type const>            iterator;
    typedef value_iterator<value_type const>            const_iterator;

    typedef std::pair<const_iterator, const_iterator>   range_type;

  public:

    template <class To, class From>
    inline static To to(From const& from) {
        To to;
        // Very crude conversion method for now:
        std::basic_stringstream<char_type> stream;
        stream << from;
        stream >> to;
        return to;
    }

    template <class From>
    inline static string_type to_string(From const& from) {
        try {
            return boost::lexical_cast<string_type>(from);
        }
        catch (boost::bad_lexical_cast const&) {
            AJG_SYNTH_THROW(conversion_error(typeid(from), typeid(string_type)));
        }
    }

    template <class From>
    inline static number_type to_number(From const& from) {
        return self_type::template to<number_type>(from);
    }

    inline static string_type literal(char const* const s) {
        return self_type::widen(std::string(s));
    }

    /* TODO:
    template <typename C, size_type N>
    inline static ... literal(C const (&n)[N]) { ... }
    */

    /// transcode
    ///     This function allows us to centralize string conversion
    ///     in order to properly, yet orthogonally, support Unicode.
    ////////////////////////////////////////////////////////////////////////////
    template <class To, class From>
    inline static std::basic_string<To> transcode(std::basic_string<From> const& s) {
        // return boost::lexical_cast<std::basic_string<To> >(s);
        return std::basic_string<To>(s.begin(), s.end());
    }

    inline static std::basic_string<char> narrow(std::basic_string<Char> const& s) {
        return self_type::template transcode<Char, char>(s);
    }

    inline static std::basic_string<Char> widen(std::basic_string<char> const& s) {
        return self_type::template transcode<char, Char>(s);
    }
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_VALUE_TRAITS_HPP_INCLUDED
