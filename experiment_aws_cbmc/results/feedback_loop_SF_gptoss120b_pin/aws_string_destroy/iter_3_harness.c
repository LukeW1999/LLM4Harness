#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness(void) {
    /* Symbolic length for the string data */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 256);

    /* Symbolic byte contents */
    uint8_t data[256];
    for (size_t i = 0; i < len; ++i) {
        data[i] = nondet_uint8_t();
    }

    /* Nondeterministically decide whether the string has an allocator */
    struct aws_allocator *alloc;
    if (nondet_bool()) {
        alloc = aws_default_allocator();
    } else {
        alloc = NULL;
    }

    /* Create the string */
    struct aws_string *str = aws_string_new_from_array(alloc, data, len);
    __CPROVER_assume(str != NULL);

    /* PRE-CALL SNAPSHOT */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t *old_bytes = NULL;
    if (old_len > 0) {
        old_bytes = malloc(old_len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < old_len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    /* Call the function under verification */
    aws_string_destroy(str);

    /* POSTCONDITIONS */
    assert(__CPROVER_is_freed(str));
    assert(!__CPROVER_is_freed(old_allocator));
    if (old_len > 0) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == data[i]);
        }
    }
}
