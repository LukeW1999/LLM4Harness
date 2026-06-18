#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Non‑deterministic inputs */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Compute expected allocation size and overflow flag */
    size_t allocation_size = 0;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);

    /* 5. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 6. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* No overflow and allocation (if any) succeeded */
        assert(!overflow);
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == allocation_size);
        if (allocation_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
    } else {
        /* Failure: either overflow or allocation failure – list is zeroed */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* 7. Fields that must not change regardless of result (none in this API) */

    /* 8. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
