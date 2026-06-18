#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum bounds for nondeterministic inputs */
#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE               256

void aws_array_list_init_dynamic_harness(void) {
    /* Allocate the list structure */
    struct aws_array_list list;

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic inputs with required assumptions */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Frame condition: snapshot of unrelated memory */
    unsigned char other_mem[128];
    for (size_t i = 0; i < sizeof(other_mem); ++i) {
        other_mem[i] = nondet_uchar();
    }
    unsigned char other_mem_snapshot[128];
    __CPROVER_memcpy(other_mem_snapshot, other_mem, sizeof(other_mem));

    /* Call the function under verification */
    int ret = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Compute expected allocation size safely */
    size_t expected_allocation = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation);

    /* Postcondition 1: Return value correctness */
    if (overflow) {
        __CPROVER_assert(ret == AWS_OP_ERR,
                         "Return should be error when multiplication overflows");
    } else {
        if (expected_allocation > 0) {
            /* Allocation may still fail */
            if (list.data == NULL) {
                __CPROVER_assert(ret == AWS_OP_ERR,
                                 "Return should be error when allocation fails");
            } else {
                __CPROVER_assert(ret == AWS_OP_SUCCESS,
                                 "Return should be success when allocation succeeds");
            }
        } else {
            /* Zero allocation size should succeed without allocating */
            __CPROVER_assert(ret == AWS_OP_SUCCESS,
                             "Return should be success when allocation size is zero");
        }
    }

    /* Postcondition 2: Output buffer invariants */
    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(list.item_size == item_size,
                         "item_size should be set to input item_size on success");
        __CPROVER_assert(list.alloc == alloc,
                         "alloc should be set to the provided allocator on success");
        __CPROVER_assert(list.length == 0,
                         "length should be zero after initialization");
        __CPROVER_assert(list.current_size == expected_allocation,
                         "current_size should equal the computed allocation size on success");
        if (expected_allocation > 0) {
            __CPROVER_assert(list.data != NULL,
                             "data pointer must be non-NULL when allocation size > 0");
        } else {
            __CPROVER_assert(list.data == NULL,
                             "data pointer must be NULL when allocation size == 0");
        }
    } else {
        /* On error the list should remain zeroed */
        __CPROVER_assert(list.item_size == 0,
                         "item_size must be zero on error");
        __CPROVER_assert(list.alloc == NULL,
                         "alloc must be NULL on error");
        __CPROVER_assert(list.length == 0,
                         "length must be zero on error");
        __CPROVER_assert(list.current_size == 0,
                         "current_size must be zero on error");
        __CPROVER_assert(list.data == NULL,
                         "data must be NULL on error");
    }

    /* Postcondition 3: Frame condition (unrelated memory unchanged) */
    __CPROVER_assert(__CPROVER_equal(other_mem, other_mem_snapshot, sizeof(other_mem)),
                     "Unrelated memory must remain unchanged");

    return 0;
}
