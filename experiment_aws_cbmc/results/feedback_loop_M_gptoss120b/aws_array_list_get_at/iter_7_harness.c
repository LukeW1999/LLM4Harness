#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

void aws_array_list_get_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.allocator = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, (size_t)MAX_INITIAL_ITEM_ALLOCATION, (size_t)MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Nondeterministic inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= (size_t)MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* data at index matches the original list element */
        if (list.item_size > 0 && list.data != NULL) {
            assert_bytes_match(
                val,
                (uint8_t *)list.data + index * list.item_size,
                list.item_size);
        }

        /* unchanged bytes outside the read region */
        if (list.current_size > 0) {
            size_t saved_offset = old_byte.offset;
            if (!(saved_offset >= index * list.item_size &&
                  saved_offset < (index + 1) * list.item_size)) {
                assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
            }
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        if (list.current_size > 0) {
            assert_array_list_equivalence(&list, &old, &old_byte);
        }

        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    /* 6. Global validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
