#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness(void) {
    /* allocator for the list */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index (bounded to keep state space reasonable) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2 + 1));

    /* 3. Allocate a readable source buffer for the value to be copied */
    void *val = NULL;
    if (list.item_size > 0) {
        val = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* length grows to at least index+1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the value at the specified index matches the input */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* all other bytes of the underlying buffer remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    } else {
        /* On failure the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 7. The array list must always be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    if (val != NULL) {
        aws_mem_release(allocator, val);
    }
}
