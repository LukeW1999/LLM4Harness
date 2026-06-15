#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        /* allocate a non‑NULL string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate space for the header plus len bytes (flexible array already has 1 byte) */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* allocator may be NULL (static string) or a real allocator */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* initialise the bytes with nondet values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        /* null‑terminator (not counted in len) */
        bytes[len] = 0;

        /* assume the freshly created string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save old state for later checks */
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(bytes, len, &old_bytes);
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;

        /* call the function under test */
        aws_string_destroy_secure(str);

        /* post‑condition: the data bytes up to len are zeroed */
        for (size_t i = 0; i < len; ++i) {
            __CPROVER_assert(bytes[i] == 0, "aws_string_destroy_secure: bytes are zeroed");
        }

        /* unchanged fields */
        __CPROVER_assert(str->len == old_len, "aws_string_destroy_secure: len unchanged");
        __CPROVER_assert(str->allocator == old_allocator, "aws_string_destroy_secure: allocator unchanged");

        /* if the string was statically allocated (allocator == NULL) it must remain valid */
        if (str->allocator == NULL) {
            __CPROVER_assert(aws_string_is_valid(str), "aws_string_destroy_secure: static string remains valid");
        }
    } else {
        /* str is NULL – the function should be a no‑op */
        str = NULL;
        aws_string_destroy_secure(str);
        /* no observable state change to assert */
    }
}
