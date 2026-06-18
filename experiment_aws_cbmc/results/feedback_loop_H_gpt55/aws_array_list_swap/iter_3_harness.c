#undef NDEBUG
#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static void fill_nondet_item(uint8_t *item, size_t item_size) {
    for (size_t i = 0; i < item_size; ++i) {
        item[i] = (uint8_t)nondet_size_t();
    }
}

static void assert_byte_arrays_equal(const uint8_t *lhs, const uint8_t *rhs, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(lhs[i] == rhs[i]);
    }
}

void aws_array_list_swap_harness(void) {
    assert(1);

    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t length = nondet_size_t();
    __CPROVER_assume(length >= 2);
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(length <= SIZE_MAX / item_size);

    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    int init_result = aws_array_list_init_dynamic(&list, allocator, length, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == aws_default_allocator());
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == length * item_size);
    assert(list.data != NULL);

    uint8_t item[MAX_ITEM_SIZE];

    for (size_t i = 0; i < length; ++i) {
        fill_nondet_item(item, item_size);
        int push_result = aws_array_list_push_back(&list, item);
        assert(push_result == AWS_OP_SUCCESS);
        assert(aws_array_list_is_valid(&list));
        assert(list.length == i + 1);
    }

    assert(list.alloc == aws_default_allocator());
    assert(list.item_size == item_size);
    assert(list.length == length);
    assert(list.current_size == length * item_size);
    assert(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t();
    __CPROVER_assume(a < list.length);

    size_t b;
    bool same_index = nondet_bool();
    if (same_index) {
        b = a;
    } else {
        b = nondet_size_t();
        __CPROVER_assume(b < list.length);
        __CPROVER_assume(b != a);
    }

    assert(a < list.length);
    assert(b < list.length);

    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;

    uint8_t old_item_a[MAX_ITEM_SIZE];
    uint8_t old_item_b[MAX_ITEM_SIZE];

    int get_a_result = aws_array_list_get_at(&list, old_item_a, a);
    int get_b_result = aws_array_list_get_at(&list, old_item_b, b);

    assert(get_a_result == AWS_OP_SUCCESS);
    assert(get_b_result == AWS_OP_SUCCESS);
    assert(aws_array_list_is_valid(&list));

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old_alloc);
    assert(list.alloc == aws_default_allocator());
    assert(list.data == old_data);
    assert(list.current_size == old_current_size);
    assert(list.length == old_length);
    assert(list.item_size == old_item_size);
    assert(aws_array_list_is_valid(&list));

    uint8_t new_item_a[MAX_ITEM_SIZE];
    uint8_t new_item_b[MAX_ITEM_SIZE];

    int new_get_a_result = aws_array_list_get_at(&list, new_item_a, a);
    int new_get_b_result = aws_array_list_get_at(&list, new_item_b, b);

    assert(new_get_a_result == AWS_OP_SUCCESS);
    assert(new_get_b_result == AWS_OP_SUCCESS);

    assert_byte_arrays_equal(new_item_a, old_item_b, old_item_size);
    assert_byte_arrays_equal(new_item_b, old_item_a, old_item_size);

    assert(aws_array_list_is_valid(&list));
}
