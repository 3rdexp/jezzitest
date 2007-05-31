#ifndef __K_TAGSAX_H__
#define __K_TAGSAX_H__

#include <cctype>
#include <string>
#include <cassert>
#include <vector>
#include <ostream>
#include <iterator>

#include "range.h"

namespace kong {

class tagparse_handler
{
public:
    virtual bool start_document() = 0;
    virtual bool start_element(const range_t & name, const attrarray_t &) = 0;
    virtual bool characters(const range_t & text) = 0;
    virtual bool end_element(const range_t & name) = 0;
    virtual bool entities(const range_t & text) = 0;
    virtual bool script(const range_t & text, const attrarray_t &) = 0;
    virtual bool end_document() = 0;
};

enum eTagState
{
    // <tag attr="value">data</tag>
    //                  [ �������� | �˳����� ]
    // main state
    state_start = 1, // [ �ĵ���ʼ ]
    state_tag_start, // [ < |  ]
    state_tag_end,   // [ > |  ]
    state_data,      // [ after state_tag_end or state_start | first < ]
    state_end,       // [ �ĵ����� ]

    // sub procedure state -- ע���ݴ�
    //                    [ ����state���� | �뿪state������ ]
    state_tag_name,    // [first not space | first space, >]
    state_attr_name,   // [ leave state_tag_name | space, = ]
    state_attr_split,  // [ leave state_attr_name | >, ", ', �����Ƿ���quote��������� ]
    state_attr_value,  // [ leave state_attr_split | >, quote ]
    state_end_tag_symbol_pre,   // [ </ ] </tag> or </ tag> or <tag />
    state_end_tag_symbol_post,  // [ leave state_atte_name ] <tag />

    // TODO: delete
    state_quote,
    state_comment,  // <!-- * -->
    state_script,   // <script xxx> * </script>
    state_entities, // <![CDATA[ * ]]>
};

template<typename HandlerT>
class tag_parse // noncopyable
{
public:
    tag_parse(HandlerT* handler) 
        : contine_(true), handler_(handler)
        , individual_script_(true)
    {}

    // template<class ForwardRange>
    bool parse(const char* beg, const char* end, HandlerT* handler = 0);
    bool parse(std::istream& stream, HandlerT* handler = 0)
    {
//        std::getline(stream, buf_, '\0');
        buf_.assign(std::istreambuf_iterator<char>(stream.rdbuf()), 
            std::istreambuf_iterator<char>());
        bool f = parse(buf_.c_str(), buf_.c_str() + buf_.size(), handler);
        buf_.clear();
        return f;
    }

    // options:
    // �Ƿ�Ե� > �󣬻��� < ǰ��space
    bool eat_space() const;
    void eat_space(bool f);

    bool is_html_tag_char(int ch)
    {
        // 0 - 9 a - z A - Z /  //
        return (ch >= 'a' && ch <= 'z')
            || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9')
            || (ch == '/') 
            || (ch == '!');
    }
private:

    enum eTagChar
    {
        char_left_bracket       = '<',
        char_right_bracket      = '>',
        char_excalmatory_mark   = '!',
        char_space              = ' ',
        char_quote = '\'',
        char_dquote = '"',
        char_equal  = '=',
        char_escape = '\\',

        char_end_tag = '/',
        char_hyphen = '-',
        char_cdata_end = ']',
    };

    // �򵥷ָ� <tag>data</tag>
    eTagState token_outof_tag(const char** ppbeg, const char* end);
    eTagState token_in_tag(const char** ppbeg, const char* end);

    // �� token_in_tag ��ϸ�ֵ� tag
    eTagState token_entities(const char** ppbeg, const char* end);
    eTagState token_script(const char** ppbeg, const char* end);

    // TODO: ϸ�� <tag attr=value> or </tag> 
    // ����ȫ���� token_in_tag ���

    // 
    eTagState token_next(const char* beg, const char* end, eTagState state_before, 
        eTagState state_new);

    // �����б���ָ��
    eTagState state_;
    range_t curtag_name_;
    range_t curattr_name_;
    range_t curattr_value_;
    attrarray_t curattr_array_;
    range_t curdata_;
    bool contine_;
    const char* curpos_;
    std::string buf_;

    // ���е��� call_xxx �����ٵ��� post_call ��������գ��Ա��´�ʹ��
    void call_start_document();
    void call_start_element(range_t & name, attrarray_t &);
    void call_characters(range_t & text);
    void call_end_element(range_t & name);
    void call_script(range_t & text, attrarray_t &);
    void call_entities(range_t & text);
    void call_end_document();

    void post_call_element();
    void post_call_document();
    void post_call_characters();
private:
    HandlerT* handler_;

    // script ���� call_script
    unsigned long individual_script_ : 1;


    // noncopyable
    tag_parse(const tag_parse&);
    tag_parse& operator=(const tag_parse&);
};

template<typename HandlerT>
inline void tag_parse<HandlerT>::post_call_element()
{
    curtag_name_.clear();
    curattr_array_.clear();
    curattr_name_.clear();
    curattr_value_.clear();
}

template<typename HandlerT>
inline void tag_parse<HandlerT>::post_call_document()
{
    buf_.clear();
}

template<typename HandlerT>
inline void tag_parse<HandlerT>::post_call_characters()
{
    curdata_.clear();
}

} // kong

#include "tagsax.inl"

#endif // __K_TAGSAX_H__
