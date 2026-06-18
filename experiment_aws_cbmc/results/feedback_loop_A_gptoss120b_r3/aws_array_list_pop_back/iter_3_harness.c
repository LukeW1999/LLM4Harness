#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare output buffer */
    uint8_t out[MAX_ITEM_SIZE];

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, out);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
    }

    /* 6. Fields that must not change */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
