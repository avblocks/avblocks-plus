#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <algorithm>

namespace primo
{
namespace program_options
{

template <typename T>
const T* literal(const char* narrow, const wchar_t* wide);

template<>
const char* literal<char>(const char* narrow, const wchar_t* wide) { return narrow; }

template<>
const wchar_t* literal<wchar_t>(const char* narrow, const wchar_t* wide) { return wide; }

#define LITERAL(char_type, x) literal<char_type>(x,L##x)


template <typename T>
inline std::basic_istringstream<T> &operator>>(std::basic_istringstream<T> &in, std::vector<std::basic_string<T>> &arr)
{
	std::basic_string<T> next;
	in >> next;
	arr.push_back(next);
	return in;
}



template <typename CHAR>
struct ParseFailure: public std::exception
{
    ParseFailure(std::basic_string<CHAR> arg0, std::basic_string<CHAR> val0, std::basic_string<CHAR> msg0)
        : arg(arg0), val(val0), msg(msg0)
    {

	}

    std::basic_string<CHAR> arg;
    std::basic_string<CHAR> val;
    std::basic_string<CHAR> msg;

    std::basic_string<CHAR> message() const
    {
        return msg + LITERAL(CHAR," arg:") + arg + LITERAL(CHAR," value:") + val;
    }
	
    const char* what() const throw()
	{ 
		return "Parse Error"; 
	}
};


// OptionBase: Virtual base class for storing information relating to a
// specific option This base class describes common elements.  Type specific
// information should be stored in a derived class.
template <typename CHAR>
struct OptionBase
{
    OptionBase(const std::basic_string<CHAR>& name, const std::basic_string<CHAR>& desc, bool flag)
        : opt_string(name), opt_desc(desc), opt_flag(flag)
    {};

    virtual ~OptionBase() {}

    // parse argument arg, to obtain a value for the option
    virtual void parse(const std::basic_string<CHAR>& arg) = 0;

    // set the argument to the default value
    virtual void setDefault() = 0;

    std::basic_string<CHAR> opt_string;
    std::basic_string<CHAR> opt_desc;
    bool opt_flag; // the option is flag and does not require a value
};


// Type specific option storage
template<typename CHAR, typename T>
struct Option : public OptionBase<CHAR>
{
    Option(const std::basic_string<CHAR>& name, T& storage, T default_val, const std::basic_string<CHAR>& desc, bool flag)
        : OptionBase<CHAR>(name, desc, flag), opt_storage(storage), opt_default_val(default_val)
    {}

    void parse(const std::basic_string<CHAR>& arg);
    
    void setDefault()
    {
        opt_storage = opt_default_val;
    }

    T& opt_storage;
    T opt_default_val;
};


// Generic parsing
template<typename CHAR, typename T>
inline void Option<CHAR, T>::parse(const std::basic_string<CHAR>& arg)
{
    std::basic_istringstream<CHAR> arg_ss (arg);
    arg_ss.exceptions(std::ios::failbit);
    try
    {
        arg_ss >> opt_storage;
    }
    catch (...)
    {
        throw ParseFailure<CHAR>(OptionBase<CHAR>::opt_string, arg, LITERAL(CHAR,"Parse error"));
    }
}

// string parsing is specialized -- copy the whole string, not just the first word
template<>
inline void Option<char, std::basic_string<char> >::parse(const std::basic_string<char>& arg)
{
    opt_storage = arg;
}

// string parsing is specialized -- copy the whole string, not just the first word
template<>
inline void Option<wchar_t, std::basic_string<wchar_t> >::parse(const std::basic_string<wchar_t>& arg)
{
    opt_storage = arg;
}

template<typename CHAR>
class OptionSpecific;

template<typename CHAR>
struct Names
{
    Names() : opt(0) {};
    ~Names()
    {
        if (opt)
        {
            delete opt;
        }
    }
    std::list<std::basic_string<CHAR> > opt_long;
    std::list<std::basic_string<CHAR> > opt_short;
    OptionBase<CHAR>* opt;
};

template<typename CHAR>
struct OptionsConfig
{
    ~OptionsConfig()
    {
        for (typename NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); it++)
        {
            delete *it;
        }
    }

    OptionSpecific<CHAR> addOptions()
    {
        return OptionSpecific<CHAR>(*this);
    }

    void addOption(OptionBase<CHAR> *opt)
    {
        Names<CHAR>* names = new Names<CHAR>();
        names->opt = opt;
        std::basic_string<CHAR>& opt_string = opt->opt_string;

        size_t opt_start = 0;
        for (size_t opt_end = 0; opt_end != std::basic_string<CHAR>::npos;)
        {
            opt_end = opt_string.find_first_of((CHAR)',', opt_start);
            bool force_short = 0;
            if (opt_string[opt_start] == (CHAR)'-')
            {
                opt_start++;
                force_short = 1;
            }
            std::basic_string<CHAR> opt_name = opt_string.substr(opt_start, opt_end - opt_start);
            if (force_short || opt_name.size() == 1)
            {
                names->opt_short.push_back(opt_name);
                opt_short_map[opt_name].push_back(names);
            }
            else
            {
                names->opt_long.push_back(opt_name);
                opt_long_map[opt_name].push_back(names);
            }
            opt_start += opt_end + 1;
        }
        opt_list.push_back(names);
    }


    typedef std::list<Names<CHAR> *> NamesPtrList;
    NamesPtrList opt_list;

    typedef std::map<std::basic_string<CHAR>, NamesPtrList> NamesMap;
    NamesMap opt_long_map;
    NamesMap opt_short_map;
};


// Class with templated overloaded operator(), for use by OptionsConfig::addOptions()
template<typename CHAR>
class OptionSpecific
{
public:
    OptionSpecific(OptionsConfig<CHAR>& parent_) : parent(parent_) {}

    /**
    * Add option described by name to the parent Options list,
    *   with storage for the option's value
    *   with default_val as the default value
    *   with desc as an optional help description
    */

    template<typename T>
    OptionSpecific& operator()(const std::basic_string<CHAR>& name, T& storage, T default_val, 
                               const std::basic_string<CHAR>& desc = LITERAL(CHAR,""))
    {
        parent.addOption(new Option<CHAR, T>(name, storage, default_val, desc, false));
        return *this;
    }

    OptionSpecific& operator()(const std::basic_string<CHAR>& name, bool& storage, 
                               const std::basic_string<CHAR>& desc = LITERAL(CHAR,""))
    {
        parent.addOption(new Option<CHAR, bool>(name, storage, false, desc, true));
        return *this;
    }


private:
    OptionsConfig<CHAR>& parent;
};


/*
  format help text for a single option:
* using the formatting: "-x, --long",
* if a short/long option isn't specified, it is not printed
*/

template<typename CHAR>
inline void doHelpOpt(std::basic_ostream<CHAR>& out, const Names<CHAR>& entry, unsigned int pad_short = 0)
{
    pad_short = std::min<unsigned int>(pad_short, 8u);

    if (!entry.opt_short.empty())
    {
        unsigned int pad = std::max<int>((int)pad_short - (int)entry.opt_short.front().size(), 0);
        out << LITERAL(CHAR,"-") << entry.opt_short.front();
        if (!entry.opt_long.empty())
        {
            out << LITERAL(CHAR,", ");
        }

        out << std::basic_string<CHAR>(1 + pad, (CHAR)' ');
    }
    else
    {
        out << LITERAL(CHAR,"   ");
        out << std::basic_string<CHAR>(1 + pad_short, (CHAR)' ');
    }

    if (!entry.opt_long.empty())
    {
        out << LITERAL(CHAR,"--") << entry.opt_long.front();
    }
}


/* format the help text */
template<typename CHAR>
inline void doHelp(std::basic_ostream<CHAR>& out, OptionsConfig<CHAR>& opts, unsigned int columns = 80)
{
    const unsigned pad_short = 3;
    /* first pass: work out the longest option name */
    unsigned max_width = 0;
    for (typename OptionsConfig<CHAR>::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
    {
        std::basic_ostringstream<CHAR> line(std::ios_base::out);
        doHelpOpt(line, **it, pad_short);
        max_width = std::max<unsigned int>(max_width, (unsigned)line.tellp());
    }

    unsigned opt_width = std::min<unsigned int>(max_width + 2, 28u + pad_short) + 2;
    unsigned desc_width = columns - opt_width;

    /* second pass: write out formatted option and help text.
    *  - align start of help text to start at opt_width
    *  - if the option text is longer than opt_width, place the help
    *    text at opt_width on the next line.
    */
    for (typename OptionsConfig<CHAR>::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
    {
        std::basic_ostringstream<CHAR> line(std::ios_base::out);
        line << LITERAL(CHAR,"  ");
        doHelpOpt(line, **it, pad_short);

        const std::basic_string<CHAR>& opt_desc = (*it)->opt->opt_desc;
        if (opt_desc.empty())
        {
            /* no help text: output option, skip further processing */
            out << line.str() << std::endl;
            continue;
        }
        size_t currlength = size_t(line.tellp());
        if (currlength > opt_width)
        {
            /* if option text is too long (and would collide with the
            * help text, split onto next line */
            line << std::endl;
            currlength = 0;
        }
        /* split up the help text, taking into account new lines,
        *   (add opt_width of padding to each new line) */
        for (size_t newline_pos = 0, cur_pos = 0; cur_pos != std::string::npos; currlength = 0)
        {
            // print any required padding space for vertical alignment
            line << std::basic_string<CHAR>(1 + opt_width - currlength, (CHAR)' ');

            newline_pos = opt_desc.find_first_of((CHAR)'\n', newline_pos);
            if (newline_pos != std::string::npos)
            {
                /* newline found, print substring (newline needn't be stripped) */
                newline_pos++;
                line << opt_desc.substr(cur_pos, newline_pos - cur_pos);
                cur_pos = newline_pos;
                continue;
            }
            if (cur_pos + desc_width > opt_desc.size())
            {
                /* no need to wrap text, remainder is less than avaliable width */
                line << opt_desc.substr(cur_pos);
                break;
            }
            /* find a suitable point to split text (avoid spliting in middle of word) */
            size_t split_pos = opt_desc.find_last_of((CHAR)' ', cur_pos + desc_width);
            if (split_pos != std::string::npos)
            {
                /* eat up multiple space characters */
                split_pos = opt_desc.find_last_not_of((CHAR)' ', split_pos) + 1;
            }

            /* bad split if no suitable space to split at.  fall back to width */
            bool bad_split = split_pos == std::string::npos || split_pos <= cur_pos;
            if (bad_split)
            {
                split_pos = cur_pos + desc_width;
            }
            line << opt_desc.substr(cur_pos, split_pos - cur_pos);

            /* eat up any space for the start of the next line */
            if (!bad_split)
            {
                split_pos = opt_desc.find_first_not_of((CHAR)' ', split_pos);
            }
            cur_pos = newline_pos = split_pos;

            if (cur_pos >= opt_desc.size())
            {
                break;
            }

            line << std::endl;
        }

        out << line.str() << std::endl;
    }
}


// for all options in opts, set their storage to their specified default value
template<typename CHAR>
inline void setDefaults(OptionsConfig<CHAR>& opts)
{
    for (typename OptionsConfig<CHAR>::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
    {
        (*it)->opt->setDefault();
    }
}


template<typename CHAR>
struct ArgvParser
{
    ArgvParser(OptionsConfig<CHAR>& rOpts)
        :opts(rOpts)
    {}

    virtual ~ArgvParser() {}

    OptionsConfig<CHAR>& opts;

    const std::basic_string<CHAR> where() { return LITERAL(CHAR,"command line"); }

    unsigned int parse(unsigned argc, const CHAR* const argv[])
    {
        std::basic_string<CHAR> arg(argv[0]);
        size_t arg_opt_start = arg.find_first_not_of(LITERAL(CHAR,"-/"));
        std::basic_string<CHAR> name = arg.substr(arg_opt_start);

        bool allow_long = true;
        bool allow_short = true;

        bool found = false;
        typename OptionsConfig<CHAR>::NamesMap::iterator opt_it;
        if (allow_long)
        {
            opt_it = opts.opt_long_map.find(name);
            if (opt_it != opts.opt_long_map.end())
            {
                found = true;
            }
        }

        // check for the short list
        if (allow_short && !(found && allow_long))
        {
            opt_it = opts.opt_short_map.find(name);
            if (opt_it != opts.opt_short_map.end())
            {
                found = true;
            }
        }

        if (!found)
        {
            throw ParseFailure<CHAR>(name, std::basic_string<CHAR>(), LITERAL(CHAR,"Parse error. Unknown option."));
        }

        int argsConsumed = 0;
        {
            typename OptionsConfig<CHAR>::NamesPtrList opt_list = (*opt_it).second;

            /* multiple options may be registered for the same name allow each to parse value */
            for (typename OptionsConfig<CHAR>::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); ++it)
            {
                if ((*it)->opt->opt_flag)
                {
                    std::basic_string<CHAR> value(LITERAL(CHAR,"1"));
                    (*it)->opt->parse(value);
                }
                else
                {
                    if (argc <= 1)
                        throw ParseFailure<CHAR>(name, std::basic_string<CHAR>(), LITERAL(CHAR,"Parse error. Value not specified."));

                    std::basic_string<CHAR> value(argv[1]);
                    
                    (*it)->opt->parse(value);

                    argsConsumed = 1;
                }
            }
        }

        return argsConsumed;
    }
};


template<typename CHAR>
inline void scanArgv(OptionsConfig<CHAR>& opts, unsigned argc, const CHAR* const argv[])
{
    setDefaults<CHAR>(opts);
    ArgvParser<CHAR> avp(opts);

    for (unsigned i = 1; i < argc; i++)
    {
        if ((argv[i][0] != (CHAR)'-') && (argv[i][0] != (CHAR)'/'))
            throw ParseFailure<CHAR>(argv[i], std::basic_string<CHAR>(), LITERAL(CHAR,"Parse error. Unknown option."));

        i += avp.parse(argc - i, &argv[i]);
    }
}

/*
 * Parse a numeric pair in the format <num>x<num>
 */
//template<typename CharType, typename NumType>
//inline std::basic_istringstream<CharType> &operator>>(std::basic_istringstream<CharType> &in, 
//                                                      std::pair<NumType,NumType>& num)
//{
//	in >> num.first;
//
//	CharType ch;
//	in >> ch; //x,X
//	
//	in >> num.second;
//	return in;
//}

}
}
