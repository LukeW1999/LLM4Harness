#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Nondeterministic inputs with required preconditions */
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Call the function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Compute expected allocation size and overflow flag */
        size_t allocation_size = 0;
        bool overflow = aws_mul_size_checked(initial_item_allocation,
                                              item_size,
                                              &allocation_size);
        /* Success implies no overflow */
        assert(!overflow);

        /* item_size and allocator must be stored */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);

        /* length is always zero after initialization */
        assert(list.length == 0);

        if (allocation_size > 0) {
            /* Data must be allocated and current_size set */
            assert(list.data != NULL);
            assert(list.current_size == allocation_size);
        } else {
            /* No allocation requested → data stays NULL and size zero */
            assert(list.data == NULL);
            assert(list.current_size == 0);
        }
    } else {
        /* On any error path the list is zero‑initialized */
        assert(list.data == NULL);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
    }

    /* 5. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
