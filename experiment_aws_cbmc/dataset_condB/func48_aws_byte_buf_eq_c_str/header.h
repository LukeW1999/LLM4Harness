#ifndef AWS_COMMON_BYTE_BUF_H
#define AWS_COMMON_BYTE_BUF_H

#include <aws/common/array_list.h>
#include <aws/common/byte_order.h>
#include <aws/common/common.h>

#include <string.h>

AWS_PUSH_SANE_WARNING_LEVEL

struct aws_byte_buf {
    
    size_t len;
    uint8_t *buffer;
    size_t capacity;
    struct aws_allocator *allocator;
};

struct aws_byte_cursor {
    
    size_t len;
    uint8_t *ptr;
};

#define AWS_BYTE_CURSOR_PRI(C) ((int)(C).len < 0 ? 0 : (int)(C).len), (const char *)(C).ptr

#define AWS_BYTE_BUF_PRI(B) ((int)(B).len < 0 ? 0 : (int)(B).len), (const char *)(B).buffer

#define AWS_BYTE_CUR_INIT_FROM_STRING_LITERAL(literal)                                                                 \
    {.ptr = (uint8_t *)(const char *)(literal), .len = sizeof(literal) - 1}

typedef bool(aws_byte_predicate_fn)(uint8_t value);

AWS_EXTERN_C_BEGIN

AWS_COMMON_API
bool aws_array_eq(const void *const array_a, const size_t len_a, const void *array_b, const size_t len_b);

AWS_COMMON_API
bool aws_array_eq_ignore_case(
    const void *const array_a,
    const size_t len_a,
    const void *const array_b,
    const size_t len_b);

AWS_COMMON_API
bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);

AWS_COMMON_API
bool aws_array_eq_c_str_ignore_case(const void *const array, const size_t array_len, const char *const c_str);

AWS_COMMON_API
int aws_byte_buf_init(struct aws_byte_buf *buf, struct aws_allocator *allocator, size_t capacity);

AWS_COMMON_API int aws_byte_buf_init_copy(
    struct aws_byte_buf *dest,
    struct aws_allocator *allocator,
    const struct aws_byte_buf *src);

AWS_COMMON_API
int aws_byte_buf_init_from_file(struct aws_byte_buf *out_buf, struct aws_allocator *alloc, const char *filename);

AWS_COMMON_API
int aws_byte_buf_init_from_file_with_size_hint(
    struct aws_byte_buf *out_buf,
    struct aws_allocator *alloc,
    const char *filename,
    size_t size_hint);

AWS_COMMON_API
bool aws_byte_buf_is_valid(const struct aws_byte_buf *const buf);

AWS_COMMON_API
bool aws_byte_cursor_is_valid(const struct aws_byte_cursor *cursor);

AWS_COMMON_API
int aws_byte_buf_init_copy_from_cursor(
    struct aws_byte_buf *dest,
    struct aws_allocator *allocator,
    struct aws_byte_cursor src);

AWS_COMMON_API
int aws_byte_buf_init_cache_and_update_cursors(struct aws_byte_buf *dest, struct aws_allocator *allocator, ...);

AWS_COMMON_API
void aws_byte_buf_clean_up(struct aws_byte_buf *buf);

AWS_COMMON_API
void aws_byte_buf_clean_up_secure(struct aws_byte_buf *buf);

AWS_COMMON_API
void aws_byte_buf_reset(struct aws_byte_buf *buf, bool zero_contents);

AWS_COMMON_API
void aws_byte_buf_secure_zero(struct aws_byte_buf *buf);

AWS_COMMON_API
bool aws_byte_buf_eq(const struct aws_byte_buf *const a, const struct aws_byte_buf *const b);

AWS_COMMON_API
bool aws_byte_buf_eq_ignore_case(const struct aws_byte_buf *const a, const struct aws_byte_buf *const b);

AWS_COMMON_API
bool aws_byte_buf_eq_c_str(const struct aws_byte_buf *const buf, const char *const c_str);

AWS_COMMON_API
bool aws_byte_buf_eq_c_str_ignore_case(const struct aws_byte_buf *const buf, const char *const c_str);

AWS_COMMON_API
bool aws_byte_cursor_next_split(
    const struct aws_byte_cursor *AWS_RESTRICT input_str,
    char split_on,
    struct aws_byte_cursor *AWS_RESTRICT substr);

AWS_COMMON_API
int aws_byte_cursor_split_on_char(
    const struct aws_byte_cursor *AWS_RESTRICT input_str,
    char split_on,
    struct aws_array_list *AWS_RESTRICT output);

AWS_COMMON_API
int aws_byte_cursor_split_on_char_n(
    const struct aws_byte_cursor *AWS_RESTRICT input_str,
    char split_on,
    size_t n,
    struct aws_array_list *AWS_RESTRICT output);

AWS_COMMON_API
int aws_byte_cursor_find_exact(
    const struct aws_byte_cursor *AWS_RESTRICT input_str,
    const struct aws_byte_cursor *AWS_RESTRICT to_find,
    struct aws_byte_cursor *first_find);

AWS_COMMON_API
struct aws_byte_cursor aws_byte_cursor_right_trim_pred(
    const struct aws_byte_cursor *source,
    aws_byte_predicate_fn *predicate);

AWS_COMMON_API
struct aws_byte_cursor aws_byte_cursor_left_trim_pred(
    const struct aws_byte_cursor *source,
    aws_byte_predicate_fn *predicate);

AWS_COMMON_API
struct aws_byte_cursor aws_byte_cursor_trim_pred(
    const struct aws_byte_cursor *source,
    aws_byte_predicate_fn *predicate);

AWS_COMMON_API
bool aws_byte_cursor_satisfies_pred(const struct aws_byte_cursor *source, aws_byte_predicate_fn *predicate);

AWS_COMMON_API
int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

AWS_COMMON_API
int aws_byte_buf_append_with_lookup(
    struct aws_byte_buf *AWS_RESTRICT to,
    const struct aws_byte_cursor *AWS_RESTRICT from,
    const uint8_t *lookup_table);

AWS_COMMON_API
int aws_byte_buf_append_dynamic(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

AWS_COMMON_API
int aws_byte_buf_append_dynamic_secure(struct aws_byte_buf *to, const struct aws_byte_cursor *from);

AWS_COMMON_API
int aws_byte_buf_append_byte_dynamic(struct aws_byte_buf *buffer, uint8_t value);

AWS_COMMON_API
int aws_byte_buf_append_byte_dynamic_secure(struct aws_byte_buf *buffer, uint8_t value);

AWS_COMMON_API
int aws_byte_buf_append_and_update(struct aws_byte_buf *to, struct aws_byte_cursor *from_and_update);

AWS_COMMON_API
int aws_byte_buf_append_null_terminator(struct aws_byte_buf *buf);

AWS_COMMON_API
int aws_byte_buf_reserve(struct aws_byte_buf *buffer, size_t requested_capacity);

AWS_COMMON_API
int aws_byte_buf_reserve_relative(struct aws_byte_buf *buffer, size_t additional_length);

AWS_COMMON_API
int aws_byte_buf_cat(struct aws_byte_buf *dest, size_t number_of_args, ...);

AWS_COMMON_API
bool aws_byte_cursor_eq(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b);

AWS_COMMON_API
bool aws_byte_cursor_eq_ignore_case(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b);

AWS_COMMON_API
bool aws_byte_cursor_eq_byte_buf(const struct aws_byte_cursor *const a, const struct aws_byte_buf *const b);

AWS_COMMON_API
bool aws_byte_cursor_eq_byte_buf_ignore_case(const struct aws_byte_cursor *const a, const struct aws_byte_buf *const b);

AWS_COMMON_API
bool aws_byte_cursor_eq_c_str(const struct aws_byte_cursor *const cursor, const char *const c_str);

AWS_COMMON_API
bool aws_byte_cursor_eq_c_str_ignore_case(const struct aws_byte_cursor *const cursor, const char *const c_str);

AWS_COMMON_API
bool aws_byte_cursor_starts_with(const struct aws_byte_cursor *input, const struct aws_byte_cursor *prefix);

AWS_COMMON_API
bool aws_byte_cursor_starts_with_ignore_case(const struct aws_byte_cursor *input, const struct aws_byte_cursor *prefix);

AWS_COMMON_API
uint64_t aws_hash_array_ignore_case(const void *array, const size_t len);

AWS_COMMON_API
uint64_t aws_hash_byte_cursor_ptr_ignore_case(const void *item);

AWS_COMMON_API
const uint8_t *aws_lookup_table_to_lower_get(void);

AWS_COMMON_API
const uint8_t *aws_lookup_table_hex_to_num_get(void);

AWS_COMMON_API
int aws_byte_cursor_compare_lexical(const struct aws_byte_cursor *lhs, const struct aws_byte_cursor *rhs);

AWS_COMMON_API
int aws_byte_cursor_compare_lookup(
    const struct aws_byte_cursor *lhs,
    const struct aws_byte_cursor *rhs,
    const uint8_t *lookup_table);

AWS_COMMON_API struct aws_byte_buf aws_byte_buf_from_c_str(const char *c_str);

AWS_COMMON_API struct aws_byte_buf aws_byte_buf_from_array(const void *bytes, size_t len);

AWS_COMMON_API struct aws_byte_buf aws_byte_buf_from_empty_array(const void *bytes, size_t capacity);

AWS_COMMON_API struct aws_byte_cursor aws_byte_cursor_from_buf(const struct aws_byte_buf *const buf);

AWS_COMMON_API struct aws_byte_cursor aws_byte_cursor_from_c_str(const char *c_str);

AWS_COMMON_API struct aws_byte_cursor aws_byte_cursor_from_array(const void *const bytes, const size_t len);

AWS_COMMON_API struct aws_byte_cursor aws_byte_cursor_advance(struct aws_byte_cursor *const cursor, const size_t len);

AWS_COMMON_API struct aws_byte_cursor aws_byte_cursor_advance_nospec(struct aws_byte_cursor *const cursor, size_t len);

AWS_COMMON_API bool aws_byte_cursor_read(
    struct aws_byte_cursor *AWS_RESTRICT cur,
    void *AWS_RESTRICT dest,
    const size_t len);

AWS_COMMON_API bool aws_byte_cursor_read_and_fill_buffer(
    struct aws_byte_cursor *AWS_RESTRICT cur,
    struct aws_byte_buf *AWS_RESTRICT dest);

AWS_COMMON_API bool aws_byte_cursor_read_u8(struct aws_byte_cursor *AWS_RESTRICT cur, uint8_t *AWS_RESTRICT var);

AWS_COMMON_API bool aws_byte_cursor_read_be16(struct aws_byte_cursor *cur, uint16_t *var);

AWS_COMMON_API bool aws_byte_cursor_read_be24(struct aws_byte_cursor *cur, uint32_t *var);

AWS_COMMON_API bool aws_byte_cursor_read_be32(struct aws_byte_cursor *cur, uint32_t *var);

AWS_COMMON_API bool aws_byte_cursor_read_be_i32(struct aws_byte_cursor *cur, int32_t *var);

AWS_COMMON_API bool aws_byte_cursor_read_be64(struct aws_byte_cursor *cur, uint64_t *var);

AWS_COMMON_API bool aws_byte_cursor_read_float_be32(struct aws_byte_cursor *cur, float *var);

AWS_COMMON_API bool aws_byte_cursor_read_float_be64(struct aws_byte_cursor *cur, double *var);

AWS_COMMON_API bool aws_byte_cursor_read_hex_u8(struct aws_byte_cursor *cur, uint8_t *var);

AWS_COMMON_API bool aws_byte_buf_advance(
    struct aws_byte_buf *const AWS_RESTRICT buffer,
    struct aws_byte_buf *const AWS_RESTRICT output,
    const size_t len);

AWS_COMMON_API bool aws_byte_buf_write(
    struct aws_byte_buf *AWS_RESTRICT buf,
    const uint8_t *AWS_RESTRICT src,
    size_t len);

AWS_COMMON_API bool aws_byte_buf_write_from_whole_buffer(
    struct aws_byte_buf *AWS_RESTRICT buf,
    struct aws_byte_buf src);

AWS_COMMON_API bool aws_byte_buf_write_from_whole_cursor(
    struct aws_byte_buf *AWS_RESTRICT buf,
    struct aws_byte_cursor src);

AWS_COMMON_API struct aws_byte_cursor aws_byte_buf_write_to_capacity(
    struct aws_byte_buf *buf,
    struct aws_byte_cursor *advancing_cursor);

AWS_COMMON_API bool aws_byte_buf_write_u8(struct aws_byte_buf *AWS_RESTRICT buf, uint8_t c);

AWS_COMMON_API bool aws_byte_buf_write_u8_n(struct aws_byte_buf *buf, uint8_t c, size_t count);

AWS_COMMON_API bool aws_byte_buf_write_be16(struct aws_byte_buf *buf, uint16_t x);

AWS_COMMON_API bool aws_byte_buf_write_be24(struct aws_byte_buf *buf, uint32_t x);

AWS_COMMON_API bool aws_byte_buf_write_be32(struct aws_byte_buf *buf, uint32_t x);

AWS_COMMON_API bool aws_byte_buf_write_float_be32(struct aws_byte_buf *buf, float x);

AWS_COMMON_API bool aws_byte_buf_write_be64(struct aws_byte_buf *buf, uint64_t x);

AWS_COMMON_API bool aws_byte_buf_write_float_be64(struct aws_byte_buf *buf, double x);

AWS_COMMON_API bool aws_isalnum(uint8_t ch);

AWS_COMMON_API bool aws_isalpha(uint8_t ch);

AWS_COMMON_API bool aws_isdigit(uint8_t ch);

AWS_COMMON_API bool aws_isxdigit(uint8_t ch);

AWS_COMMON_API bool aws_isspace(uint8_t ch);

AWS_COMMON_API
int aws_byte_cursor_utf8_parse_u64(struct aws_byte_cursor cursor, uint64_t *dst);

AWS_COMMON_API
int aws_byte_cursor_utf8_parse_i64(struct aws_byte_cursor cursor, int64_t *dst);

AWS_COMMON_API
int aws_byte_cursor_utf8_parse_u64_hex(struct aws_byte_cursor cursor, uint64_t *dst);

AWS_EXTERN_C_END
AWS_POP_SANE_WARNING_LEVEL

#endif
