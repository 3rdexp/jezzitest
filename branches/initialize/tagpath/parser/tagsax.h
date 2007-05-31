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
    //                  [ 进入条件 | 退出条件 ]
    // main state
    state_start = 1, // [ 文档开始 ]
    state_tag_start, // [ < |  ]
    state_tag_end,   // [ > |  ]
    state_data,      // [ after state_tag_end or state_start | first < ]
    state_end,       // [ 文档结束 ]

    // sub procedure state -- 注意容错
    //                    [ 进入state条件 | 离开state的条件 ]
    state_tag_name,    // [first not space | first space, >]
    state_attr_name,   // [ leave state_tag_name | space, = ]
    state_attr_split,  // [ leave state_attr_name | >, ", ', 根据是否有quote的情况决定 ]
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
    // 是否吃掉 > 后，或者 < 前的space
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

    // 简单分隔 <tag>data</tag>
    eTagState token_outof_tag(const char** ppbeg, const char* end);
    eTagState token_in_tag(const char** ppbeg, const char* end);

    // 在 token_in_tag 中细分的 tag
    eTagState token_entities(const char** ppbeg, const char* end);
    eTagState token_script(const char** ppbeg, const char* end);

    // TODO: 细分 <tag attr=value> or </tag> 
    // 现在全部在 token_in_tag 完成

    // 
    eTagState token_next(const char* beg, const char* end, eTagState state_before, 
        eTagState state_new);

    // 分析中保存指针
    eTagState state_;
    range_t curtag_name_;
    range_t curattr_name_;
    range_t curattr_value_;
    attrarray_t curattr_array_;
    range_t curdata_;
    bool contine_;
    const char* curpos_;
    std::string buf_;

    // 所有调用 call_xxx 函后，再调用 post_call 把数据清空，以备下次使用
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

    // script 调用 call_script
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
