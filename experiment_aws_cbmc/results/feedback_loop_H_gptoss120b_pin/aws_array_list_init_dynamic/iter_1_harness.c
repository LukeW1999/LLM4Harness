#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic inputs, bounded */
    struct aws_allocator *alloc = aws_default_allocator();

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 4. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑conditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocation size = initial_item_allocation * item_size (no overflow) */
        assert(list.current_size == initial_item_allocation * item_size);

        /* data pointer is NULL iff size is zero */
        if (list.current_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }

        /* fields set by the function */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
    } else {
        /* on error the struct is zeroed */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.data == NULL);
    }

    /* 6. Fields that must not change regardless of result (none in this case) */

    /* 7. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
