//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED
#define AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED

#include <map>
#include <sstream>

#include <boost/array.hpp>
#include <boost/function.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace django {
namespace {
    namespace algo = boost::algorithm;
} // namespace

//
// formatter
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Options>
struct formatter {
  public:

    typedef Options                                                             options_type;
    typedef typename options_type::traits_type                                  traits_type;
    typedef typename options_type::boolean_type                                 boolean_type;
    typedef typename options_type::char_type                                    char_type;
    typedef typename options_type::size_type                                    size_type;
    typedef typename options_type::number_type                                  number_type;
    typedef typename options_type::date_type                                    date_type;
    typedef typename options_type::datetime_type                                datetime_type;
    typedef typename options_type::duration_type                                duration_type;
    typedef typename options_type::string_type                                  string_type;
    typedef typename options_type::value_type                                   value_type;
    typedef typename options_type::range_type                                   range_type;
    typedef typename options_type::sequence_type                                sequence_type;
    typedef typename options_type::arguments_type                               arguments_type;
    typedef typename options_type::context_type                                 context_type;

  private:

    typedef typename string_type::const_iterator                                string_iterator_type;
    typedef xpressive::match_results<string_iterator_type>                      string_match_type;

///
/// native_flags:
///     The flag specifiers native to Boost.DateTime. See
///     http://www.boost.org/doc/libs/release/doc/html/date_time/date_time_io.html#date_time.format_flags
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct native_flags {
        BOOST_STATIC_CONSTANT(size_type, total   = 45);
        BOOST_STATIC_CONSTANT(size_type, skipped = 8);
        BOOST_STATIC_CONSTANT(size_type, size    = total - skipped);
        // NOTE: Fields marked [*] are not used because their availability is platform-dependent.

        // Date flags:
        string_type a;
        string_type A;
        string_type b;
        string_type B;
        string_type c;
     // string_type C; [*]
        string_type d;
        string_type D;
     // string_type e; [*]
        string_type g;
        string_type G;
     // string_type h; [*]
        string_type j;
        string_type m;
        string_type u;
        string_type U;
     // string_type V; [*]
        string_type w;
        string_type W;
        string_type x;
        string_type y;
        string_type Y;

        // Time flags:
     // string_type -;
     // string_type +;
        string_type f;
        string_type F;
        string_type H;
        string_type I;
        string_type k;
        string_type l;
        string_type M;
        string_type O;
        string_type p;
     // string_type P; [*]
     // string_type r; [*]
        string_type R;
        string_type s;
        string_type S;
        string_type T;
        string_type q;
        string_type Q;
        string_type X;
        string_type z;
        string_type Z;
        string_type ZP;

        inline static native_flags from_datetime(datetime_type datetime) {
            static boost::array<string_type, native_flags::size> const format_flags = {
                { traits_type::literal("%a")
                , traits_type::literal("%A")
                , traits_type::literal("%b")
                , traits_type::literal("%B")
                , traits_type::literal("%c")
                , traits_type::literal("%d")
                , traits_type::literal("%D")
                , traits_type::literal("%g")
                , traits_type::literal("%G")
                , traits_type::literal("%j")
                , traits_type::literal("%m")
                , traits_type::literal("%u")
                , traits_type::literal("%U")
                , traits_type::literal("%w")
                , traits_type::literal("%W")
                , traits_type::literal("%x")
                , traits_type::literal("%y")
                , traits_type::literal("%Y")
                , traits_type::literal("%f")
                , traits_type::literal("%F")
                , traits_type::literal("%H")
                , traits_type::literal("%I")
                , traits_type::literal("%k")
                , traits_type::literal("%l")
                , traits_type::literal("%M")
                , traits_type::literal("%O")
                , traits_type::literal("%p")
                , traits_type::literal("%R")
                , traits_type::literal("%s")
                , traits_type::literal("%S")
                , traits_type::literal("%T")
                , traits_type::literal("%q")
                , traits_type::literal("%Q")
                , traits_type::literal("%X")
                , traits_type::literal("%z")
                , traits_type::literal("%Z")
                , traits_type::literal("%ZP")
                }
            };
            static string_type const delimiter     = string_type(1, char_type('&'));
            static string_type const format_string = algo::join(format_flags, delimiter);

            std::vector<string_type> specifiers;
            specifiers.reserve(native_flags::size);
            string_type const formatted_string = detail::format_time(format_string, datetime);
            algo::split(specifiers, formatted_string, algo::is_any_of(delimiter));
            BOOST_ASSERT(specifiers.size() == native_flags::size);

            native_flags const flags =
                { specifiers[0]
                , specifiers[1]
                , specifiers[2]
                , specifiers[3]
                , specifiers[4]
                , specifiers[5]
                , specifiers[6]
                , specifiers[7]
                , specifiers[8]
                , specifiers[9]
                , specifiers[10]
                , specifiers[11]
                , specifiers[12]
                , specifiers[13]
                , specifiers[14]
                , specifiers[15]
                , specifiers[16]
                , specifiers[17]
                , specifiers[18]
                , specifiers[19]
                , specifiers[20]
                , specifiers[21]
                , specifiers[22]
                , specifiers[23]
                , specifiers[24]
                , specifiers[25]
                , specifiers[26]
                , specifiers[27]
                , specifiers[28]
                , specifiers[29]
                , specifiers[30]
                , specifiers[31]
                , specifiers[32]
                , specifiers[33]
                , specifiers[34]
                , specifiers[35]
                , specifiers[36]
                };
            return flags;
        }
    };

///
/// cooked_flags:
///     The flag specifiers used by Django.
////////////////////////////////////////////////////////////////////////////////////////////////////

    struct cooked_flags {
        string_type a;
        string_type A;
        string_type b;
        string_type B;
        string_type c;
        string_type d;
        string_type D;
        string_type e;
        string_type E;
        string_type f;
        string_type F;
        string_type g;
        string_type G;
        string_type h;
        string_type H;
        string_type i;
        string_type I;
        string_type j;
        string_type l;
        string_type L;
        string_type m;
        string_type M;
        string_type n;
        string_type N;
        string_type o;
        string_type O;
        string_type P;
        string_type r;
        string_type s;
        string_type S;
        string_type t;
        string_type T;
        string_type u;
        string_type U;
        string_type w;
        string_type W;
        string_type y;
        string_type Y;
        string_type z;
        string_type Z;

        inline static cooked_flags cook_flags(native_flags const& flags, datetime_type const& datetime) {
            size_type     const year        = datetime.date().year();
            boolean_type  const is_am       = flags.p == traits_type::literal("AM");
            boolean_type  const is_pm       = flags.p == traits_type::literal("PM");
            boolean_type  const has_minutes = flags.M != traits_type::literal("00");
            boolean_type  const is_midnight = flags.H == "00" && !has_minutes;
            boolean_type  const is_noon     = flags.H == "12" && !has_minutes;
            boolean_type  const is_dst      = false;  // TODO: Implement, e.g. using tm_isdst from struct tm.
            boolean_type  const is_leapyear = ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
            datetime_type const epoch       = datetime_type(date_type(1970, 1, 1));
            string_type   const meridiem    = is_am ? traits_type::literal("a.m.")
                                            : is_pm ? traits_type::literal("p.m.")
                                            : string_type();
            string_type   const succint     = algo::trim_left_copy(flags.l)
                                            + (has_minutes ? ':' + flags.M : string_type());
            string_type   const informal    = is_midnight ? traits_type::literal("midnight")
                                            : is_noon     ? traits_type::literal("noon")
                                            : (succint + ' ' + meridiem);

            cooked_flags cooked;
            cooked.a = meridiem;
            cooked.A = flags.p;
            cooked.b = algo::to_lower_copy(flags.b);
            cooked.B = "";       // NOTE: "Not implemented" per the spec.
            cooked.c = flags.Y + '-' + flags.m + '-' + flags.d + 'T' + flags.H + ':' + flags.M + ':' + flags.S;
            cooked.d = flags.d;
            cooked.D = flags.a;
            cooked.e = flags.z;  // TODO: Ignored with ptimes.
            cooked.E = flags.B;  // TODO: Make locale-aware.
            cooked.f = informal;
            cooked.F = flags.B;
            cooked.g = algo::trim_left_copy(flags.l);
            cooked.G = algo::trim_left_copy(flags.k);
            cooked.h = flags.I;
            cooked.H = flags.H;
            cooked.i = flags.M;
            cooked.I = is_dst ? traits_type::literal("1") : traits_type::literal("0");
            cooked.j = algo::trim_left_copy_if(flags.d, algo::is_any_of("0"));
            cooked.l = flags.A;
            cooked.L = traits_type::to_string(is_leapyear);
            cooked.m = flags.m;
            cooked.M = flags.b;
            cooked.n = algo::trim_left_copy_if(flags.m, algo::is_any_of("0"));
            cooked.N = flags.b;  // TODO: Use A.P. style.
            cooked.o = flags.G;
            cooked.O = "";       // TODO: Implement.
            cooked.P = informal;
            cooked.r = flags.a + ',' + ' ' + flags.d + ' ' + flags.b + ' ' + flags.Y + ' ' + flags.T; // TODO: Include non-UTC timezones.
            cooked.s = flags.S;
            cooked.S = "";       // TODO: Implement.
            cooked.t = "";       // TODO: Implement.
            cooked.T = "";       // TODO: Implement.
            cooked.u = algo::trim_left_copy_if(flags.f, algo::is_any_of("."));
            cooked.U = traits_type::to_string((datetime - epoch).seconds());
            cooked.w = flags.w;
            cooked.W = "";       // TODO: Like %V but without leading zeros.
            cooked.y = flags.y;
            cooked.Y = flags.Y;
            cooked.z = algo::trim_left_copy_if(flags.j, algo::is_any_of("0"));
            cooked.Z = "";       // TODO: Implement.
            return cooked;
        }
    };

  public:

    static string_type format_datetime( options_type  const& options
                                      , string_type   const& label
                                      , datetime_type const& datetime
                                      ) {
        native_flags const native = native_flags::from_datetime(datetime);
        cooked_flags const cooked = cooked_flags::cook_flags(native, datetime);
        std::basic_ostringstream<char_type> stream;
        typename options_type::formats_type::const_iterator const it = options.formats.find(label);
        string_type const format = it == options.formats.end() ? label : it->second;

        // TODO: This might not be UTF8-safe; consider using a utf8_iterator.
        BOOST_FOREACH(char_type const c, format) {
            switch (c) {
            case char_type('a'): stream << cooked.a; break;
            case char_type('A'): stream << cooked.A; break;
            case char_type('b'): stream << cooked.b; break;
            case char_type('B'): stream << cooked.B; break;
            case char_type('c'): stream << cooked.c; break;
            case char_type('d'): stream << cooked.d; break;
            case char_type('D'): stream << cooked.D; break;
            case char_type('e'): stream << cooked.e; break;
            case char_type('E'): stream << cooked.E; break;
            case char_type('f'): stream << cooked.f; break;
            case char_type('F'): stream << cooked.F; break;
            case char_type('g'): stream << cooked.g; break;
            case char_type('G'): stream << cooked.G; break;
            case char_type('h'): stream << cooked.h; break;
            case char_type('H'): stream << cooked.H; break;
            case char_type('i'): stream << cooked.i; break;
            case char_type('I'): stream << cooked.I; break;
            case char_type('j'): stream << cooked.j; break;
            case char_type('l'): stream << cooked.l; break;
            case char_type('L'): stream << cooked.L; break;
            case char_type('m'): stream << cooked.m; break;
            case char_type('M'): stream << cooked.M; break;
            case char_type('n'): stream << cooked.n; break;
            case char_type('N'): stream << cooked.N; break;
            case char_type('o'): stream << cooked.o; break;
            case char_type('O'): stream << cooked.O; break;
            case char_type('P'): stream << cooked.P; break;
            case char_type('r'): stream << cooked.r; break;
            case char_type('s'): stream << cooked.s; break;
            case char_type('S'): stream << cooked.S; break;
            case char_type('t'): stream << cooked.t; break;
            case char_type('T'): stream << cooked.T; break;
            case char_type('u'): stream << cooked.u; break;
            case char_type('U'): stream << cooked.U; break;
            case char_type('w'): stream << cooked.w; break;
            case char_type('W'): stream << cooked.W; break;
            case char_type('y'): stream << cooked.y; break;
            case char_type('Y'): stream << cooked.Y; break;
            case char_type('z'): stream << cooked.z; break;
            case char_type('Z'): stream << cooked.Z; break;
            default: stream << c;
            }
        }

        BOOST_ASSERT(stream);
        return stream.str();
    }

  public:

    // TODO: Proper, localizable formatting.
    static string_type format_duration( options_type  const& options
                                      , duration_type const& duration
                                      ) {
        BOOST_STATIC_CONSTANT(size_type, N = 6);

        static size_type const seconds[N] = { 60 * 60 * 24 * 365
                                            , 60 * 60 * 24 * 30
                                            , 60 * 60 * 24 * 7
                                            , 60 * 60 * 24
                                            , 60 * 60
                                            , 60
                                            };
        static string_type const units[N] = { traits_type::literal("year")
                                            , traits_type::literal("month")
                                            , traits_type::literal("week")
                                            , traits_type::literal("day")
                                            , traits_type::literal("hour")
                                            , traits_type::literal("minute")
                                            };

        if (duration.is_negative()) {
            return formatter::pluralize_unit(0, units[N - 1], options);
        }

        string_type result;
        size_type const total = duration.total_seconds();
        size_type count = 0, i = 0;

        for (; i < N; ++i) {
            if ((count = total / seconds[i])) {
                break;
            }
        }

        result += formatter::pluralize_unit(count, units[i], options);

        if (i + 1 < N) {
            if ((count = (total - (seconds[i] * count)) / seconds[i + 1])) {
                result += traits_type::literal(", ") + pluralize_unit(count, units[i + 1], options);
            }
        }

        return result;
    }

  private:

    /*inline static string_type nonbreaking(string_type const& s) {
        return boost::algorithm::replace_all_copy(s, traits_type::literal(" "), options.nonbreaking_space);
    }*/

    inline static string_type pluralize_unit( size_type    const  n
                                            , string_type  const& s
                                            , options_type const& options
                                            ) {
        string_type const suffix = n == 1 ? string_type() : traits_type::literal("s");
        return traits_type::to_string(n) + options.nonbreaking_space + s + suffix;
    }
};

}}} // namespace ajg::synth::django

#endif // AJG_SYNTH_ENGINES_DJANGO_FORMATTER_HPP_INCLUDED