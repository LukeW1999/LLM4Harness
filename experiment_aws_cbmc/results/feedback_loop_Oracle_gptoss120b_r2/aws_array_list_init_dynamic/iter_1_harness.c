#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum values for the nondeterministic inputs */
#define MAX_INITIAL_ITEM_ALLOCATION 1024U
#define MAX_ITEM_SIZE               256U

/* Nondeterministic generators for CBMC */
size_t nondet_size_t(void);

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic inputs with the required assumptions */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size                = nondet_size_t();

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Preserve a copy of the list structure for frame condition checks */
    struct aws_array_list old_list = list;

    /* Call the function under verification */
    int ret = aws_array_list_init_dynamic(
        &list,
        alloc,
        initial_item_allocation,
        item_size);

    /* Compute the expected allocation size and overflow flag using the same helper */
    size_t expected_allocation = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation);

    /* --------------------------------------------------------------------- */
    /* 1. Return‑value / error‑code correctness                               */
    /* --------------------------------------------------------------------- */
    if (overflow) {
        /* Multiplication overflow must cause an error */
        assert(ret == AWS_OP_ERR);
    } else {
        if (expected_allocation > 0) {
            /* Allocation may succeed or fail */
            if (list.data == NULL) {
                assert(ret == AWS_OP_ERR);
            } else {
                assert(ret == AWS_OP_SUCCESS);
            }
        } else {
            /* No allocation required – must succeed */
            assert(ret == AWS_OP_SUCCESS);
        }
    }

    /* --------------------------------------------------------------------- */
    /* 2. Output buffer invariants (length / capacity)                        */
    /* --------------------------------------------------------------------- */
    if (ret == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);               /* zeroed by AWS_ZERO_STRUCT */
        if (expected_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            assert(list.current_size == expected_allocation);
            assert(list.data != NULL);
        }
    } else {
        /* On error the structure must remain zeroed */
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* --------------------------------------------------------------------- */
    /* 3. Frame condition – memory not modified beyond contract               */
    /* --------------------------------------------------------------------- */
    /* The function only touches the fields of the list structure and,
       optionally, the memory block pointed to by list.data.  All other
       memory must remain unchanged.  Since we saved a copy of the original
       structure, we can compare the untouched fields. */
    assert(list.length == old_list.length);          /* remains zero */
    assert(list.current_size == old_list.current_size ||
           ret == AWS_OP_SUCCESS);                  /* may change only on success */
    assert(list.data == old_list.data ||
           ret == AWS_OP_SUCCESS);                  /* may change only on success */

    return 0;
}
