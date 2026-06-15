#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 1024
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 256
#endif

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list list;
    assert(aws_array_list_init(&list, allocator, capacity, item_size) == AWS_OP_SUCCESS);

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;
    list.current_size = length * item_size;

    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(index < old.length);
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (list.data == old.data && old.data != NULL && old.current_size > 0) {
            size_t total = old.length * old.item_size;
            size_t prefix = index * old.item_size;
            if (prefix > 0) {
                assert_bytes_match((uint8_t *)list.data,
                                   (uint8_t *)old.data,
                                   prefix);
            }
            size_t suffix_start = (index + 1) * old.item_size;
            if (suffix_start < total) {
                size_t suffix_len = total - suffix_start;
                assert_bytes_match((uint8_t *)list.data + suffix_start,
                                   (uint8_t *)old.data + suffix_start,
                                   suffix_len);
            }
        }
    } else {
        assert(index >= old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    free(val);
    aws_array_list_clean_up(&list);
}
