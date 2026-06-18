#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC harness for aws_array_list_pop_front */
void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before the call */
    struct aws_array_list old = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Post‑condition checks */

    /* Validity must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* Success path: length is decremented by one */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length - 1);
    } else {
        /* Failure path: length unchanged */
        assert(list.length == old_length);
    }

    /* Fields that must remain unchanged regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.length == old.length ? list.length == old_length : true); /* redundancy for clarity */
}
