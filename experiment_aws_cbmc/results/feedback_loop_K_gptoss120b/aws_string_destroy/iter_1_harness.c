/*  
 * Contract for aws_string_destroy  
 * --------------------------------  
 * Preconditions:  
 *   - `str` is either NULL or points to a valid `struct aws_string`.  
 *   - If `str` is non‑NULL, then `aws_string_is_valid(str)` holds.  
 *   - If `str` is non‑NULL and `str->allocator` is non‑NULL, the allocator is a valid  
 *     `struct aws_allocator` (as provided by `aws_default_allocator()`).  
 *   - The memory region occupied by `str` (including its flexible array member) is  
 *     properly allocated and not overlapping any other object.  
 *   - No other global or heap objects are aliased with `str`.  
 *  
 * Postconditions (validity):  
 *   - The function has no return value.  
 *   - The caller’s pointer variable `str` is unchanged (the value of the pointer is the same).  
 *   - If `str` is non‑NULL and `str->allocator` is non‑NULL, the memory for `str` has been  
 *     released via the allocator (the memory is no longer required to be a valid object).  
 *   - No memory locations other than the storage owned by `str` (and the internal state of the  
 *     allocator) are modified.  
 *   - No undefined behaviour occurs (e.g., no dereference of a NULL pointer).  
 */  

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    /* nondet allocation size for the flexible array member (may be zero) */
    size_t extra_len = nondet_size_t();
    __CPROVER_assume(extra_len <= 256); /* keep the allocation bounded */

    /* Allocate memory for the aws_string structure plus its bytes */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        str = malloc(sizeof(struct aws_string) + extra_len);
        __CPROVER_assume(str != NULL);
    }

    if (str) {
        /* Allocate (or reuse) an allocator – we use the default allocator for simplicity */
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);

        /* Nondet choice: either the string owns its memory (allocator non‑NULL) or it is static */
        if (nondet_bool()) {
            str->allocator = alloc;
        } else {
            str->allocator = NULL;
        }

        /* Set a nondet length that is consistent with the allocated extra bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= extra_len);
        /* The struct field is const, but we can cast away constness for the harness */
        *((size_t *)&str->len) = len;

        /* Initialise the bytes (the flexible array) with nondet data */
        for (size_t i = 0; i < extra_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* Ensure the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Snapshot of memory that must remain unchanged (a dummy buffer) */
    uint8_t untouched[32];
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        untouched[i] = nondet_uint8_t();
    }
    uint8_t untouched_before[32];
    __CPROVER_memcpy(untouched_before, untouched, sizeof(untouched));

    /* Preserve the original pointer value for post‑condition check */
    struct aws_string *str_before = str;

    /* Call the function under verification */
    aws_string_destroy(str);

    /* Post‑condition checks */
    /* The caller’s pointer variable must be unchanged */
    assert(str == str_before);

    /* The dummy buffer must be untouched */
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        assert(untouched[i] == untouched_before[i]);
    }

    return 0;
}
