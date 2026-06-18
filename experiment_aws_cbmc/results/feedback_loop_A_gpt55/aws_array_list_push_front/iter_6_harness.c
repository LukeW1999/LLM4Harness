#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define HARNESS_MAX_ITEM_SIZE 4
#define HARNESS_MAX_INITIAL_ITEMS 3
#define HARNESS_MAX_INITIAL_BYTES (HARNESS_MAX_ITEM_SIZE * HARNESS_MAX_INITIAL_ITEMS)

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

    uint8_t static_storage[HARNESS_MAX_INITIAL_BYTES];
    uint8_t value_storage[HARNESS_MAX_ITEM_SIZE];
    uint8_t old_value[HARNESS_MAX_ITEM_SIZE];
    uint8_t old_contents[HARNESS_MAX_INITIAL_BYTES];

    assert(allocator != NULL);

    size_t mode = nondet_size_t();
    __CPROVER_assume(mode < 3);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= HARNESS_MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0);
    __CPROVER_assume(initial_capacity <= HARNESS_MAX_INITIAL_ITEMS);

    size_t initial_size_bytes = initial_capacity * item_size;
    __CPROVER_assume(initial_size_bytes <= HARNESS_MAX_INITIAL_BYTES);

    assert(item_size > 0);
    assert(item_size <= HARNESS_MAX_ITEM_SIZE);
    assert(initial_capacity > 0);
    assert(initial_capacity <= HARNESS_MAX_INITIAL_ITEMS);
    assert(initial_size_bytes <= sizeof(static_storage));

    if (mode == 0) {
        aws_array_list_init_static(&list, static_storage, initial_capacity, item_size);
        list.length = initial_capacity;
    } else {
        int init_result = aws_array_list_init_dynamic(&list, allocator, initial_capacity, item_size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);

        if (mode == 1) {
            size_t length = nondet_size_t();
            __CPROVER_assume(length < initial_capacity);
            list.length = length;
        } else {
            list.length = initial_capacity;
        }
    }

    assert(list.item_size == item_size);
    assert(list.current_size == initial_size_bytes);
    assert(list.length <= initial_capacity);
    assert(list.data != NULL);
    assert(aws_array_list_is_valid(&list));

    fill_nondet_bytes((uint8_t *)list.data, list.length * list.item_size);
    fill_nondet_bytes(value_storage, list.item_size);

    struct aws_array_list old = list;
    size_t old_len_bytes = old.length * old.item_size;
    size_t required_len_bytes = (old.length + 1) * old.item_size;

    assert(old.item_size == item_size);
    assert(old.length <= initial_capacity);
    assert(old_len_bytes <= HARNESS_MAX_INITIAL_BYTES);
    assert(required_len_bytes > old_len_bytes);

    copy_bytes(old_contents, (const uint8_t *)old.data, old_len_bytes);
    copy_bytes(old_value, value_storage, old.item_size);

    int result = aws_array_list_push_front(&list, value_storage);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert_bytes_equal(value_storage, old_value, old.item_size);

    if (mode == 0) {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_bytes_equal((const uint8_t *)list.data, old_contents, old_len_bytes);
    } else if (mode == 1) {
        assert(result == AWS_OP_SUCCESS);
        assert(list.length == old.length + 1);
        assert(list.current_size == old.current_size);
        assert(list.current_size >= required_len_bytes);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_bytes_equal((const uint8_t *)list.data, old_value, old.item_size);
        assert_bytes_equal((const uint8_t *)list.data + old.item_size, old_contents, old_len_bytes);
    } else {
        if (result == AWS_OP_SUCCESS) {
            assert(list.length == old.length + 1);
            assert(list.current_size >= required_len_bytes);
            assert(list.alloc == old.alloc);
            assert(list.item_size == old.item_size);
            assert_bytes_equal((const uint8_t *)list.data, old_value, old.item_size);
            assert_bytes_equal((const uint8_t *)list.data + old.item_size, old_contents, old_len_bytes);
        } else {
            assert(list.length == old.length);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
            assert(list.alloc == old.alloc);
            assert(list.item_size == old.item_size);
            assert_bytes_equal((const uint8_t *)list.data, old_contents, old_len_bytes);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
