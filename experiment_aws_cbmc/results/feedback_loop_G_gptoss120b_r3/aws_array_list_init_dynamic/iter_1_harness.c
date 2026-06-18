#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Non‑deterministic inputs with appropriate bounds */
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_allocator *alloc = aws_default_allocator();

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Fields that must be set on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        if (list.current_size == 0) {
            assert(list.data == NULL);
        } else {
            assert(list.data != NULL);
        }
        /* The post‑condition from the implementation */
        assert(list.current_size == 0 || list.data);
    } else {
        /* On failure the list should remain zeroed */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.length == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
        /* Ensure no change from the old (zeroed) state */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Fields that never change regardless of result (none beyond those above) */

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
