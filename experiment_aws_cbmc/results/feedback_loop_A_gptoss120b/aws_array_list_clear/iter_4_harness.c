#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable value buffer (may be NULL if item_size==0) */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    if (list.item_size > 0) {
        __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    }

    /* 4. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_mem = {0};
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_mem);
    }

    /* 5. Keep a copy of the input value */
    uint8_t *val_copy = NULL;
    if (val) {
        val_copy = malloc(list.item_size);
        __CPROVER_assume(val_copy != NULL);
        memcpy(val_copy, val, list.item_size);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);
    /* Ensure the result is one of the expected outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity invariant */
        assert(aws_array_list_is_valid(&list));

        /* the data at the target index must match the input value */
        if (val_copy) {
            assert_bytes_match(
                (uint8_t *)list.data + (index * list.item_size),
                val_copy,
                list.item_size);
        }

        /* length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* data pointer and current_size may change (reallocation/growth) */
    } else {
        /* on failure the whole structure must remain unchanged */
        assert(result == AWS_OP_ERR);
        assert_array_list_equivalence(&list, &old, &old_mem);
    }

    /* 8. The list must always satisfy its validity predicate */
    assert(aws_array_list_is_valid(&list));
}
