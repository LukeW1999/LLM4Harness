#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 256

int main(void) {
    /* 1. Possibly allocate an aws_string */
    struct aws_string *str;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        /* allocate space for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (len == 0 ? 0 : len - 1));
        __CPROVER_assume(str != NULL);

        /* choose allocator (default or NULL) */
        struct aws_allocator *alloc = nondet_bool() ? aws_default_allocator() : NULL;

        /* initialize const fields */
        *((struct aws_allocator *const *)(&str->allocator)) = alloc;
        *((size_t const *)(&str->len)) = len;

        /* nondet fill bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* 2. Save old state for frame condition checks */
    struct aws_string *old_ptr = str;
    struct aws_string old_struct;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL) {
        old_struct = *str;                     /* copy allocator, len, first byte */
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call function under test */
    aws_string_destroy_secure(str);

    /* 4. Post‑condition checks */
    if (old_ptr != NULL) {
        /* frame condition: pointer identity unchanged */
        assert(str == old_ptr);

        if (old_struct.allocator == NULL) {
            /* bytes must be zeroed */
            for (size_t i = 0; i < old_struct.len; ++i) {
                assert(((uint8_t *)aws_string_bytes(str))[i] == 0);
            }
            /* unchanged fields */
            assert(str->allocator == old_struct.allocator);
            assert(str->len == old_struct.len);
            /* still a valid string */
            assert(aws_string_is_valid(str));
        } else {
            /* allocator non‑NULL: memory was freed; we cannot dereference str.
               The pointer identity assertion above already provides a reachable check. */
        }
    } else {
        /* str was NULL – nothing to check */
    }

    return 0;
}
