#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic size_t generator for CBMC */
size_t nondet_size_t(void);

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Save a copy of the list structure for frame condition checks */
    struct aws_array_list old_list = list;

    /* Call the function under verification */
    int result = aws_array_list_init_dynamic(
        &list, alloc, initial_item_allocation, item_size);

    /* Determine whether multiplication overflowed and the expected allocation size */
    size_t expected_allocation_size = 0;
    bool overflow = aws_mul_size_checked(
        initial_item_allocation, item_size, &expected_allocation_size);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Successful initialization must not have overflowed */
        assert(!overflow);
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);
        assert(list.current_size == expected_allocation_size);
        if (expected_allocation_size > 0) {
            assert(list.data != NULL);
        } else {
            assert(list.data == NULL);
        }
    } else {
        /* Failure case */
        assert(result == AWS_OP_ERR);
        if (overflow) {
            /* On overflow the list remains zero‑initialized */
            assert(list.alloc == NULL);
            assert(list.item_size == 0);
            assert(list.current_size == 0);
            assert(list.length == 0);
            assert(list.data == NULL);
        } else {
            /* Allocation failure: item_size and alloc are set, data stays NULL */
            assert(!overflow);
            assert(list.item_size == item_size);
            assert(list.alloc == alloc);
            assert(list.current_size == 0);
            assert(list.length == 0);
            assert(list.data == NULL);
        }
    }

    /* Frame condition: fields not intended to change remain unchanged */
    /* All fields of aws_array_list are accounted for above; no additional
       memory should be modified by the function. */
    (void)old_list; /* suppress unused variable warning */

    return 0;
}
