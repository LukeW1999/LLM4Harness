#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* stack‑allocated buffer large enough for the string structure and its bytes */
        uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;

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
    }
}
