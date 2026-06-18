#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str;

    if (nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the struct plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the allocator is set */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* fill the byte payload with nondet data (including the implicit NUL) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 2. Save old state for the “no‑free” case */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_bytes[MAX_STRING_LEN];

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        if (old_len > 0) {
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
        }
    }

    /* Determine whether the function will perform a free */
    int performed_free = (str != NULL && str->allocator != NULL);

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */
    if (!performed_free) {
        /* No free happened – the string (if any) must be unchanged */
        if (str != NULL) {
            __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
            __CPROVER_assert(str->len == old_len, "length unchanged");
            if (old_len > 0) {
                for (size_t i = 0; i < old_len; ++i) {
                    __CPROVER_assert(((uint8_t *)str->bytes)[i] == old_bytes[i],
                                     "bytes unchanged");
                }
            }
            __CPROVER_assert(aws_string_is_valid(str), "string remains valid");
        }
    } else {
        /* A free was performed – no further checks needed */
    }
}
