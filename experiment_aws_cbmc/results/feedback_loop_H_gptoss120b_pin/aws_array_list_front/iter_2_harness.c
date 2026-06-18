#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Prepare nondeterministic input value */
    void *val = NULL;
    if (list.item_size > 0) {
        val = allocator->mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        uint8_t *p = (uint8_t *)val;
        for (size_t i = 0; i < list.item_size; ++i) {
            p[i] = nondet_uint8_t();
        }
    }

    /* 4. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase to index+1 */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* Data at the index must match the input value */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               (uint8_t *)val,
                               list.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 7. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
