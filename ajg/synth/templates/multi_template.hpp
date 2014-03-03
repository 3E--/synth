//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED

#include <memory>
#include <vector>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <ajg/synth/engines/detail.hpp> // For AJG_UNREACHABLE

namespace ajg {
namespace synth {
namespace detail {

// TODO: Replace with multi_engine.
template <class Char, template <class C, class E> class Template, class Django, class SSI, class TMPL>
struct multi_template {

  public:

    typedef Char   char_type;
    typedef Django django_engine_type;
    typedef SSI    ssi_engine_type;
    typedef TMPL   tmpl_engine_type;

    typedef typename Template<char_type, django_engine_type>::type django_template_type;
    typedef typename Template<char_type, ssi_engine_type>::type    ssi_template_type;
    typedef typename Template<char_type, tmpl_engine_type>::type   tmpl_template_type;

    typedef bool                          boolean_type;
    typedef std::basic_string<char_type>  string_type;
    typedef std::basic_ostream<char_type> stream_type;
    typedef std::vector<string_type>      directories_type;

    // typedef typename engine_type::context_type context_type;

  public:

    template <class Source>
    multi_template( Source&                 source
                  , string_type      const& engine_name
                  , boolean_type     const  autoescape
                  , string_type      const& default_value
                  , directories_type const& directories
                  )
        : django_template_(engine_name == "django" ? new django_template_type(source) : 0)
        , ssi_template_   (engine_name == "ssi"    ? new ssi_template_type   (source) : 0)
        , tmpl_template_  (engine_name == "tmpl"   ? new tmpl_template_type  (source) : 0)
        , django_options_(autoescape, default_value, directories)
        , ssi_options_(default_value, directories) // TODO: size_format, time_format, error_message
        , tmpl_options_() { // TODO: directories

        if (!django_template_ && !ssi_template_ && !tmpl_template_) {
            throw std::invalid_argument("engine_name");
        }
    }

  protected:

    // TODO: Support post-construction options (maybe using something like multi_options?)
    //       e.g. Options const& options = django_options_, ssi_options_, tmpl_options_

    template <class X, class Context>
    void render(stream_type& stream, Context const& context) const {
        if (django_template_) {
            return django_template_->render(stream,
                X::template adapt_context<typename django_template_type::context_type>(context), django_options_);
        }
        else if (ssi_template_) {
            return ssi_template_->render(stream,
                X::template adapt_context<typename ssi_template_type::context_type>(context), ssi_options_);
        }
        else if (tmpl_template_) {
            return tmpl_template_->render(stream,
                X::template adapt_context<typename tmpl_template_type::context_type>(context), tmpl_options_);
        }
        AJG_UNREACHABLE;
    }

    template <class X, class Context>
    string_type render_to_string(Context const& context) const {
        if (django_template_) {
            return django_template_->render_to_string(
                X::template adapt_context<typename django_template_type::context_type>(context), django_options_);
        }
        else if (ssi_template_) {
            return ssi_template_->render_to_string(
                X::template adapt_context<typename ssi_template_type::context_type>(context), ssi_options_);
        }
        else if (tmpl_template_) {
            return tmpl_template_->render_to_string(
                X::template adapt_context<typename tmpl_template_type::context_type>(context), tmpl_options_);
        }
        AJG_UNREACHABLE;
    }

    template <class X, class Context>
    void render_to_file(string_type const& filepath, Context const& context) const {
        if (django_template_) {
            return django_template_->render_to_file(filepath,
                X::template adapt_context<typename django_template_type::context_type>(context), django_options_);
        }
        else if (ssi_template_) {
            return ssi_template_->render_to_file(filepath,
                X::template adapt_context<typename ssi_template_type::context_type>(context), ssi_options_);
        }
        else if (tmpl_template_) {
            return tmpl_template_->render_to_file(filepath,
                X::template adapt_context<typename tmpl_template_type::context_type>(context), tmpl_options_);
        }
        AJG_UNREACHABLE;
    }

  private:

    // TODO: Use unique_ptr?
    boost::shared_ptr<django_template_type>     django_template_;
    boost::shared_ptr<ssi_template_type>        ssi_template_;
    boost::shared_ptr<tmpl_template_type>       tmpl_template_;
    typename django_template_type::options_type django_options_;
    typename ssi_template_type::options_type    ssi_options_;
    typename tmpl_template_type::options_type   tmpl_options_;
};

}}} // namespace ajg::synth::detail

#endif // AJG_SYNTH_TEMPLATES_MULTI_TEMPLATE_HPP_INCLUDED