#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Prepare a readable input value */
    size_t item_sz = list.item_size;
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index, bounded to avoid overflow */
    size_t index = nondet_size_t();
    if (item_sz != 0) {
        size_t max_index = (size_t)-1 / item_sz;
        __CPROVER_assume(index <= max_index);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* item size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* length updates correctly */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the element at the given index now matches the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        /* all other existing elements remain unchanged */
        for (size_t i = 0; i < old.length; ++i) {
            if (i != index) {
                assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                                   (uint8_t *)old.data + (i * old.item_size),
                                   list.item_size);
            }
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               list.current_size);
        }
    }

    /* 6. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
