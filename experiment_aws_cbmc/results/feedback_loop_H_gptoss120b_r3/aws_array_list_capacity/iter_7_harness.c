#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t max_items = MAX_INITIAL_ITEM_ALLOCATION;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Initialize the list with a bounded capacity */
    aws_array_list_init(&list, allocator, max_items, item_size);

    /* Constrain the current length of the list */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_items);
    list.length = len;
    list.current_size = len * item_size;

    /* Ensure the list is in a valid state */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));
    }

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        if (item_size > 0 && list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * item_size), val, item_size);
        }
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.current_size >= old.current_size);
        assert(list.current_size >= (index + 1) * item_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_bytes);
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    if (val) {
        aws_mem_release(allocator, val);
    }

    aws_array_list_clean_up(&list);
}
