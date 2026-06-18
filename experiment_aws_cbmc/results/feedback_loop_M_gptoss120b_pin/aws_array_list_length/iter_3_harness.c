#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    /* avoid size_t overflow in address calculation */
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    /* 4. Allocate a readable buffer for val */
    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *val_buf = aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val_buf != NULL);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Global invariant: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Length becomes max(old.length, index+1) */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* The stored element matches the input value */
        {
            uint8_t *dest = (uint8_t *)list.data + index * list.item_size;
            for (size_t i = 0; i < list.item_size; ++i) {
                assert(dest[i] == val_buf[i]);
            }
        }

        /* Fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Capacity may grow, but never shrink */
        assert(list.current_size >= old.current_size);
    } else {
        /* On failure the whole structure must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    aws_mem_release(allocator, val_buf);
}
