#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);
bool nondet_bool(void);
uint8_t nondet_uint8_t(void);

void aws_string_destroy_harness(void) {
    /* Symbolic length for the string data */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 256);

    /* Allocate memory for the aws_string structure plus its flexible array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(str != NULL);

    /* Nondeterministically decide whether the string has an allocator */
    if (nondet_bool()) {
        str->allocator = aws_default_allocator();
    } else {
        str->allocator = NULL;
    }

    /* Initialize the length field */
    str->len = len;

    /* Initialize the byte contents */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* PRE-CALL SNAPSHOT */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t *old_bytes = NULL;
    if (len > 0) {
        old_bytes = malloc(len);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    /* Call the function under verification */
    aws_string_destroy(str);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* The string memory should be freed */
    assert(__CPROVER_is_freed(str));

    /* The allocator object used by the string must remain unchanged and not freed */
    assert(!__CPROVER_is_freed(old_allocator));

    /* The snapshot of the string's byte contents must remain unchanged */
    if (old_len > 0) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == old_bytes[i]);
        }
    }
}
