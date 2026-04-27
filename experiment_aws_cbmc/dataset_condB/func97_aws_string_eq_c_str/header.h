#ifndef AWS_COMMON_STRING_H
#define AWS_COMMON_STRING_H

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>

AWS_PUSH_SANE_WARNING_LEVEL

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4623) 
#    pragma warning(disable : 4626) 
#    pragma warning(disable : 5027) 
#endif
struct aws_string {
    struct aws_allocator *const allocator;
    
    const size_t len;
    
    const uint8_t bytes[1];
};

#ifdef AWS_OS_WINDOWS
struct aws_wstring {
    struct aws_allocator *const allocator;
    
    const size_t len;
    
    const wchar_t bytes[1];
};
#endif 

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

AWS_EXTERN_C_BEGIN

#ifdef AWS_OS_WINDOWS

AWS_COMMON_API struct aws_wstring *aws_string_convert_to_wstring(
    struct aws_allocator *allocator,
    const struct aws_string *to_convert);

AWS_COMMON_API struct aws_wstring *aws_string_convert_to_wchar_from_byte_cursor(
    struct aws_allocator *allocator,
    const struct aws_byte_cursor *to_convert);

AWS_COMMON_API
void aws_wstring_destroy(struct aws_wstring *str);

AWS_COMMON_API struct aws_string *aws_string_convert_from_wchar_str(
    struct aws_allocator *allocator,
    const struct aws_wstring *to_convert);

AWS_COMMON_API struct aws_string *aws_string_convert_from_wchar_byte_cursor(
    struct aws_allocator *allocator,
    const struct aws_byte_cursor *to_convert);

AWS_COMMON_API struct aws_string *aws_string_convert_from_wchar_c_str(
    struct aws_allocator *allocator,
    const wchar_t *to_convert);

AWS_COMMON_API struct aws_wstring *aws_wstring_new_from_cursor(
    struct aws_allocator *allocator,
    const struct aws_byte_cursor *w_str_cur);

AWS_COMMON_API struct aws_wstring *aws_wstring_new_from_array(
    struct aws_allocator *allocator,
    const wchar_t *w_str,
    size_t length);

AWS_COMMON_API const wchar_t *aws_wstring_c_str(const struct aws_wstring *str);

AWS_COMMON_API size_t aws_wstring_num_chars(const struct aws_wstring *str);

AWS_COMMON_API size_t aws_wstring_size_bytes(const struct aws_wstring *str);

AWS_COMMON_API bool aws_wstring_is_valid(const struct aws_wstring *str);

#endif 

AWS_COMMON_API
bool aws_string_eq(const struct aws_string *a, const struct aws_string *b);

AWS_COMMON_API
bool aws_string_eq_ignore_case(const struct aws_string *a, const struct aws_string *b);

AWS_COMMON_API
bool aws_string_eq_byte_cursor(const struct aws_string *str, const struct aws_byte_cursor *cur);

AWS_COMMON_API
bool aws_string_eq_byte_cursor_ignore_case(const struct aws_string *str, const struct aws_byte_cursor *cur);

AWS_COMMON_API
bool aws_string_eq_byte_buf(const struct aws_string *str, const struct aws_byte_buf *buf);

AWS_COMMON_API
bool aws_string_eq_byte_buf_ignore_case(const struct aws_string *str, const struct aws_byte_buf *buf);

AWS_COMMON_API
bool aws_string_eq_c_str(const struct aws_string *str, const char *c_str);

AWS_COMMON_API
bool aws_string_eq_c_str_ignore_case(const struct aws_string *str, const char *c_str);

AWS_COMMON_API
struct aws_string *aws_string_new_from_c_str(struct aws_allocator *allocator, const char *c_str);

AWS_COMMON_API
struct aws_string *aws_string_new_from_array(struct aws_allocator *allocator, const uint8_t *bytes, size_t len);

AWS_COMMON_API
struct aws_string *aws_string_new_from_string(struct aws_allocator *allocator, const struct aws_string *str);

AWS_COMMON_API
struct aws_string *aws_string_new_from_cursor(struct aws_allocator *allocator, const struct aws_byte_cursor *cursor);

AWS_COMMON_API
struct aws_string *aws_string_new_from_buf(struct aws_allocator *allocator, const struct aws_byte_buf *buf);

AWS_COMMON_API
void aws_string_destroy(struct aws_string *str);

AWS_COMMON_API
void aws_string_destroy_secure(struct aws_string *str);

AWS_COMMON_API
int aws_string_compare(const struct aws_string *a, const struct aws_string *b);

AWS_COMMON_API
int aws_array_list_comparator_string(const void *a, const void *b);

#define AWS_STATIC_STRING_FROM_LITERAL(name, literal)                                                                  \
    static const struct {                                                                                              \
        struct aws_allocator *const allocator;                                                                         \
        const size_t len;                                                                                              \
        const uint8_t bytes[sizeof(literal)];                                                                          \
    } name##_s = {NULL, sizeof(literal) - 1, literal};                                                                 \
    static const struct aws_string *name = (struct aws_string *)(&name##_s) 

#define AWS_STRING_FROM_LITERAL(name, literal)                                                                         \
    static const struct {                                                                                              \
        struct aws_allocator *const allocator;                                                                         \
        const size_t len;                                                                                              \
        const uint8_t bytes[sizeof(literal)];                                                                          \
    } name##_s = {NULL, sizeof(literal) - 1, literal};                                                                 \
    const struct aws_string *(name) = (struct aws_string *)(&name##_s)

AWS_COMMON_API
bool aws_byte_buf_write_from_whole_string(
    struct aws_byte_buf *AWS_RESTRICT buf,
    const struct aws_string *AWS_RESTRICT src);

AWS_COMMON_API
struct aws_byte_cursor aws_byte_cursor_from_string(const struct aws_string *src);

AWS_COMMON_API
struct aws_string *aws_string_clone_or_reuse(struct aws_allocator *allocator, const struct aws_string *str);

AWS_COMMON_API
int aws_secure_strlen(const char *str, size_t max_read_len, size_t *str_len);

AWS_STATIC_IMPL
const uint8_t *aws_string_bytes(const struct aws_string *str);

AWS_STATIC_IMPL
const char *aws_string_c_str(const struct aws_string *str);

AWS_STATIC_IMPL
bool aws_string_is_valid(const struct aws_string *str);

AWS_STATIC_IMPL
bool aws_c_string_is_valid(const char *str);

AWS_STATIC_IMPL
bool aws_char_is_space(uint8_t c);

AWS_EXTERN_C_END

#ifndef AWS_NO_STATIC_IMPL
#    include <aws/common/string.inl>
#endif 

AWS_POP_SANE_WARNING_LEVEL

#endif
