#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare inputs (may be NULL) */
    struct aws_string *str;
    const char *c_str;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a nondeterministic length string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* allocate space for struct + (len-1) extra bytes for flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);
        /* allocator can be any pointer (including NULL) */
        str->allocator = (struct aws_allocator *)nondet_uint64_t();
        str->len = len;
        /* nondeterministically fill the bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        /* assume the string satisfies the library invariant */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide if c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* allocate a nondeterministic C‑string (null‑terminated) */
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[c_len] = '\0';
        c_str = buf;
        /* assume the C‑string satisfies the library invariant */
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* 2. Save old state */
    struct aws_string *old_str_ptr = str;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    const char *old_c_str_ptr = c_str;
    struct store_byte_from_buffer old_c_str_bytes;
    if (c_str != NULL) {
        /* compute length of the original C string */
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        save_byte_from_array((const uint8_t *)c_str, c_len, &old_c_str_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Assert postconditions for both success and failure paths */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(str == old_str_ptr);
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    assert(c_str == old_c_str_ptr);
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_bytes);
    }

    /* 6. Assert validity invariants always hold */
    assert(aws_string_is_valid(str));
    assert(aws_c_string_is_valid(c_str));

    /* clean up */
    free((void *)str);
    free((void *)c_str);
}
