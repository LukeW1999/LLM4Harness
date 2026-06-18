#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic inputs */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Call the function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must change on success */
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        assert(list.length == 0);                     /* zeroed by the function */
        /* current_size and data are linked: both zero or both non‑zero */
        assert((list.current_size == 0) == (list.data == NULL));
    } else {
        /* On failure the list is left zeroed */
        assert(list.current_size == 0);
        assert(list.length == 0);
        assert(list.item_size == 0);
        assert(list.alloc == NULL);
        assert(list.data == NULL);
    }

    /* 6. Fields that must remain unchanged regardless of outcome */
    /* length is already asserted above for both paths; no other fields exist. */

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
