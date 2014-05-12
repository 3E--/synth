//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED
#define AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

namespace boost {

// Forward declaration.
template <class T, std::size_t N> class array;

}

#include <ajg/synth/adapters/concrete_adapter.hpp>
#include <ajg/synth/adapters/container_adapter.hpp>

namespace ajg {
namespace synth {

//
// specialization for native arrays of statically known size
////////////////////////////////////////////////////////////////////////////////////////////////////

// Note: A reference is stored and not a copy because:
// a) To store a copy it'd have to be initialized element by element.
// b) It is difficult to envision a situation with a temporary (rvalue) native array.
//    UPDATE: Actually, it is not so difficult.
//      TODO: Consider making this a copy after all.
// c) The Boost.Array specialization makes use of that fact.

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, T[N]> : concrete_adapter<Behavior, T const (&)[N], adapter<Behavior, T[N]> > {
    adapter(T const (&adapted)[N]) : concrete_adapter<Behavior, T const (&)[N], adapter<Behavior, T[N]> >(adapted) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    floating_type to_floating()  const { return N; }
    boolean_type to_boolean() const { return N != 0; }
    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }
    boolean_type equal(adapter_type const& that) const { return this->equal_sequence(that); }

    const_iterator begin() const { return const_iterator(static_cast<T const*>(this->adapted_)); }
    const_iterator end()   const { return const_iterator(static_cast<T const*>(this->adapted_) + N); }
};

//
// specialization for native arrays of statically unknown size
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T>
struct adapter<Behavior, T[]> : concrete_adapter<Behavior, T* const, adapter<Behavior, T[]> > {
    adapter(T* const adapted, std::size_t const length) : concrete_adapter<Behavior, T* const, adapter<Behavior, T[]> >(adapted), length_(length) {}

    AJG_SYNTH_ADAPTER_TYPEDEFS(Behavior);

    floating_type to_floating()  const { return this->length_; }
    boolean_type  to_boolean() const { return this->length_ != 0; }

    void output(ostream_type& out) const { behavior_type::enumerate(*this, out); }
    boolean_type equal(adapter_type const& that) const { return this->equal_sequence(that); }

    const_iterator begin() const { return this->adapted_ + 0; }
    const_iterator end()   const { return this->adapted_ + this->length_; }

    std::type_info const& type() const { return typeid(T*); } // XXX: return typeid(T[]);

  protected:

    virtual boolean_type equal_adapted(adapter_type const& that) const {
        return this->template equal_as<adapter>(that);
    }

    virtual boolean_type less_adapted(adapter_type const& that) const {
        return this->template less_as<adapter>(that);
    }

  private:

    std::size_t length_;
};

//
// specialization for boost::array
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Behavior, class T, std::size_t N>
struct adapter<Behavior, boost::array<T, N> >  : container_adapter<Behavior, boost::array<T, N> > {
    adapter(boost::array<T, N> const& adapted) : container_adapter<Behavior, boost::array<T, N> >(adapted) {}
};

}} // namespace ajg::synth

#endif // AJG_SYNTH_ADAPTERS_ARRAY_HPP_INCLUDED

