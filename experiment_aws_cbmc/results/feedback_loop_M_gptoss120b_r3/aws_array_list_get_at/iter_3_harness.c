#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare nondeterministic inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));

    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *out = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(out != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        out[i] = nondet_uint8_t();
    }

    /* 4. Call the function under test */
    int result = aws_array_list_get_at(&list, out, index);

    /* 5. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* index must be within the old length */
        __CPROVER_assert(index < old.length, "index within bounds on success");
        /* the value retrieved must match the stored value */
        assert_bytes_match(
            (uint8_t *)old.data + (index * old.item_size),
            out,
            old.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    aws_mem_release(allocator, out);
}
