#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static void fill_nondet_bytes(uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
}

static void copy_bytes(uint8_t *dst, const uint8_t *src, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
}

static void assert_bytes_equal(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(a[i] == b[i]);
    }
}

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t mode = nondet_size_t();
    __CPROVER_assume(mode < 3);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(capacity < SIZE_MAX / item_size);

    if (mode == 0) {
        uint8_t *raw_array = malloc(capacity * item_size);
        __CPROVER_assume(raw_array != NULL);

        aws_array_list_init_static(&list, raw_array, capacity, item_size);
        list.length = capacity;
    } else {
        int init_result = aws_array_list_init_dynamic(&list, allocator, capacity, item_size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);

        if (mode == 1) {
            size_t length = nondet_size_t();
            __CPROVER_assume(length < capacity);
            list.length = length;
        } else {
            list.length = capacity;
        }
    }

    fill_nondet_bytes((uint8_t *)list.data, list.length * list.item_size);

    assert(list.item_size == item_size);
    assert(list.length <= capacity);
    assert(list.current_size == capacity * item_size);
    assert(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    fill_nondet_bytes(val, list.item_size);

    struct aws_array_list old = list;
    size_t old_len_bytes = old.length * old.item_size;
    size_t required_len_bytes = (old.length + 1) * old.item_size;

    uint8_t *old_data = malloc(old_len_bytes > 0 ? old_len_bytes : 1);
    __CPROVER_assume(old_data != NULL);
    copy_bytes(old_data, (const uint8_t *)old.data, old_len_bytes);

    uint8_t *old_val = malloc(old.item_size);
    __CPROVER_assume(old_val != NULL);
    copy_bytes(old_val, val, old.item_size);

    int result = aws_array_list_push_front(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert_bytes_equal(val, old_val, old.item_size);

    if (mode == 0) {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_bytes_equal((const uint8_t *)list.data, old_data, old_len_bytes);
    } else if (mode == 1) {
        assert(result == AWS_OP_SUCCESS);
        assert(list.length == old.length + 1);
        assert(list.current_size == old.current_size);
        assert(list.current_size >= required_len_bytes);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_bytes_equal((const uint8_t *)list.data, old_val, old.item_size);
        assert_bytes_equal((const uint8_t *)list.data + old.item_size, old_data, old_len_bytes);
    } else {
        if (result == AWS_OP_SUCCESS) {
            assert(list.length == old.length + 1);
            assert(list.current_size >= required_len_bytes);
            assert(list.alloc == old.alloc);
            assert(list.item_size == old.item_size);
            assert_bytes_equal((const uint8_t *)list.data, old_val, old.item_size);
            assert_bytes_equal((const uint8_t *)list.data + old.item_size, old_data, old_len_bytes);
        } else {
            assert(list.length == old.length);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
            assert(list.alloc == old.alloc);
            assert(list.item_size == old.item_size);
            assert_bytes_equal((const uint8_t *)list.data, old_data, old_len_bytes);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
