#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_c_str_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *str;
    const char *c_str;

    /* nondet decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate struct with flexible array member on the stack */
        struct aws_string *tmp = (struct aws_string *)alloca(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(tmp != NULL);

        tmp->allocator = aws_default_allocator();
        tmp->len = len;

        /* fill bytes with nondet values */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)tmp->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(tmp));
        str = tmp;
    }

    /* nondet decide if c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);

        char *buf = (char *)alloca(c_len + 1);
        __CPROVER_assume(buf != NULL);

        for (size_t i = 0; i < c_len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[c_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old immutable state of str */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes_storage;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes_storage);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Postcondition assertions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == aws_array_eq_c_str(str->bytes, str->len, c_str));
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes_storage);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
