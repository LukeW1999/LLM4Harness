#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    int case_id = nondet_int();
    __CPROVER_assume(case_id >= 0 && case_id <= 2);

    struct aws_allocator *orig_allocator = NULL;
    size_t orig_len = 0;
    uint8_t *orig_bytes = NULL;

    if (case_id == 0) {
        /* str is NULL */
        str = NULL;
    } else if (case_id == 1) {
        /* Valid string with non‑NULL allocator */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        uint8_t *buf = (uint8_t *)malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(aws_default_allocator(), buf, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        orig_allocator = str->allocator;
        orig_len = str->len;

        free(buf);
    } else {
        /* String with NULL allocator */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        /* Allocate the struct on the stack */
        uint8_t mem[sizeof(struct aws_string) + 256];
        __CPROVER_assume(len <= 256);
        struct aws_string *s = (struct aws_string *)mem;
        s->allocator = NULL;
        s->len = len;

        uint8_t *bytes = (uint8_t *)s->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        str = s;
        orig_allocator = s->allocator;
        orig_len = s->len;

        orig_bytes = (uint8_t *)malloc(len);
        __CPROVER_assume(orig_bytes != NULL);
        memcpy(orig_bytes, bytes, len);
    }

    aws_string_destroy(str);

    if (str != NULL && orig_allocator == NULL) {
        __CPROVER_assert(str->allocator == NULL, "allocator unchanged when not freed");
        __CPROVER_assert(str->len == orig_len, "length unchanged when not freed");
        for (size_t i = 0; i < orig_len; ++i) {
            __CPROVER_assert(((uint8_t *)str->bytes)[i] == orig_bytes[i],
                             "bytes unchanged when not freed");
        }
    }
}
