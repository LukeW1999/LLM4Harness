#include <assert.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and initialize a bounded array list */
    struct aws_array_list list;
    list.alloc = allocator;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    size_t init_capacity = nondet_size_t();
    __CPROVER_assume(init_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    aws_array_list_init(&list, allocator, init_capacity, list.item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer `val` */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
        size_t expected_len = (old.length > index) ? old.length : (index + 1);
        assert(list.length == expected_len);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_bytes);
        assert(aws_array_list_is_valid(&list));
    }

    /* 7. Clean up */
    free(val);
    aws_array_list_clean_up(&list);
}
