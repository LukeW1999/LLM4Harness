#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    struct store_byte_from_buffer old_data = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_data);
    }

    /* 3. Nondeterministic inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2 + 1));

    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        uint8_t *val_bytes = (uint8_t *)val;
        for (size_t i = 0; i < list.item_size; ++i) {
            val_bytes[i] = nondet_uint8_t();
        }
    }

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* fields that must stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length may increase */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the stored element must equal the input value */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (uint8_t *)val,
                               list.item_size);
        }
    } else {
        /* on failure the whole structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_data);
        }
    }

    /* 6. Clean up */
    free(val);
}
