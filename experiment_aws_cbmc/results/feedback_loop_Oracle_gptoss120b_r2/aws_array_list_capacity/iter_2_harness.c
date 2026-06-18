#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* --- nondeterministic setup --- */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024);               /* reasonable bound */

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 64);                 /* reasonable bound */

    size_t current_size = capacity * item_size;

    uint8_t *data = NULL;
    if (current_size > 0) {
        data = (uint8_t *)aws_mem_acquire(alloc, current_size);
        __CPROVER_assume(data != NULL);
        for (size_t i = 0; i < current_size; ++i) {
            data[i] = nondet_uint8_t();
        }
    }

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    /* --- initialize list --- */
    list.alloc = alloc;
    list.data = data;
    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;

    /* Precondition: list must be valid */
    __CPROVER_assert(aws_array_list_is_valid(&list), "precondition: list is valid");

    /* Preserve a copy of the list structure and its data for frame condition checks */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (current_size > 0) {
        old_data = malloc(current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, data, current_size);
    }

    /* --- call function under test --- */
    size_t cap = aws_array_list_capacity(&list);

    /* --- postconditions --- */

    /* 1. Return value correctness */
    assert(cap == (list.current_size / list.item_size));

    /* 2. List invariants (list fields unchanged) */
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);

    /* 3. Memory frame condition (data buffer unchanged) */
    if (current_size > 0) {
        assert(memcmp(data, old_data, current_size) == 0);
    }

    /* 4. List remains valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    if (data) {
        aws_mem_release(alloc, data);
    }
    free(old_data);
}
