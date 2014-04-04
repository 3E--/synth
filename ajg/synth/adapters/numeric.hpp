//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED

#include <cmath>
#include <vector>
#include <iomanip>

#include <boost/cstdint.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

#include <ajg/synth/engines/detail.hpp>
#include <ajg/synth/adapters/adapter.hpp>

namespace ajg {
namespace synth {

//
// an abstract numeric type to allow disparate number comparisons
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits>
struct abstract_numeric_adapter : public abstract_adapter<Traits> {};

//
// base adapter implementation for primitive numeric types
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Traits, class Numeric>
struct numeric_adapter : public abstract_numeric_adapter<Traits> {

    AJG_SYNTH_ADAPTER_TYPEDEFS(Numeric, numeric_adapter);
    adapted_type /*const*/ adapted_;

  protected:

    numeric_adapter(adapted_type const& adapted) : adapted_(adapted) {}

  protected:

    virtual boolean_type equal_adapted(abstract_type const& that) const {
        return this->template equal_as<numeric_adapter>(that);
    }

    virtual boolean_type less_adapted(abstract_type const& that) const {
        return this->template less_as<numeric_adapter>(that);
    }

  public:

    // TODO: Centralize this logic in value_traits.
    boolean_type equal(abstract_type const& that) const {
        // Exact match; e.g. double == double
        if (numeric_adapter const *const match = dynamic_cast<numeric_adapter const*>(&that)) {
            return this->adapted_ == match->adapted_;
        }
        // Numeric match; e.g. int == double
        else if (dynamic_cast<const abstract_numeric_adapter<Traits> *>(&that)) {
            return this->count() == that.count();
        }
        else {
            return false;
        }
    }

    // TODO: Centralize this logic in value_traits.
    boolean_type less(abstract_type const& that) const {
        // Exact match; e.g. double < double
        if (numeric_adapter const *const match = dynamic_cast<numeric_adapter const*>(&that)) {
            return this->adapted_ < match->adapted_;
        }
        // Numeric match; e.g. int < double
        else if (dynamic_cast<const abstract_numeric_adapter<Traits> *>(&that)) {
            return this->count() < that.count();
        }
        else {
            return this->to_string() < that.to_string();
        }
    }

    number_type  count() const { return number_type(adapted_); }
    boolean_type test()  const { return adapted_ != Numeric(0); }
    void input (istream_type& in)        { in  >> adapted_; }
    void output(ostream_type& out) const { output_<Numeric>(out); }
    std::type_info const& type() const { return typeid(Numeric); }

  private:

    // for floating-point types
    template <class T>
    void output_(ostream_type& out, typename disable_if<is_integral<T> >::type* = 0) const {
        io::basic_ios_all_saver<char_type> saver(out);

        if (detail::is_integer(adapted_)) {
            out << std::fixed << std::setprecision(0);
        }

        if (out.flags() & out.hex) {
        // If they hex is desired, we convert to an
        // integral first, and then output as hex.
            out << static_cast<detail::integer_type>(adapted_);
        }
        else {
            out << adapted_;
        }
    }

    // for integral types
    template <class T>
    void output_(ostream_type& out, typename enable_if<is_integral<T> >::type* = 0) const {
        out << adapted_;
    }
};


#define AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(type)                          \
    template <class Traits>                                                 \
    struct adapter<Traits, type> : public numeric_adapter<Traits, type> {   \
        adapter(type const value) : numeric_adapter<Traits, type>(value) {} \
    }                                                                       \

//
// integrals
////////////////////////////////////////////////////////////////////////////////////////////////////

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char signed);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(char unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(short unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(int unsigned);

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long unsigned);

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
#ifndef DISABLE_WIDE_CHAR_SUPPORT
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(wchar_t);
#endif // !DISABLE_WIDE_CHAR_SUPPORT
#endif // !BOOST_NO_INTRINSIC_WCHAR_T

#ifdef BOOST_HAS_LONG_LONG
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long long unsigned);
#elif defined(BOOST_HAS_MS_INT64)
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(__int64 unsigned);
#endif

//
// floating-point
////////////////////////////////////////////////////////////////////////////////////////////////////

AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(float);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(double);
AJG_SYNTH_SPECIALIZE_NUMERIC_ADAPTER(long double);


}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_NUMERIC_HPP_INCLUDED

