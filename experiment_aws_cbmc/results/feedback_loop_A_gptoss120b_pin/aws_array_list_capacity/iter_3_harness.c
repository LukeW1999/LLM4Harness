#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    list.alloc = allocator; /* ensure allocator is valid */

    /* 2. Allocate a nondeterministic value buffer */
    size_t item_size = list.item_size;
    if (item_size == 0) {
        item_size = 1;
    }
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save a byte from the value for later comparison */
    struct store_byte_from_buffer val_storage;
    save_byte_from_array(val, item_size, &val_storage);

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain valid */
        assert(aws_array_list_is_valid(&list));

        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Length updates correctly */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The element at the given index now matches the input value */
        assert_bytes_match(
            (const uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    /* Clean up */
    free(val);
}
