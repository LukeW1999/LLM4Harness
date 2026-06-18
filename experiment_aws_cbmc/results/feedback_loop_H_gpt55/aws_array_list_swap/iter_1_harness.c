#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t old_item_a[MAX_ITEM_SIZE];
    uint8_t old_item_b[MAX_ITEM_SIZE];
    uint8_t old_item_other[MAX_ITEM_SIZE];

    int get_a_result = aws_array_list_get_at(&list, old_item_a, a);
    int get_b_result = aws_array_list_get_at(&list, old_item_b, b);
    assert(get_a_result == AWS_OP_SUCCESS);
    assert(get_b_result == AWS_OP_SUCCESS);

    size_t other = nondet_size_t();
    bool check_other = other < list.length && other != a && other != b;
    if (check_other) {
        int get_other_result = aws_array_list_get_at(&list, old_item_other, other);
        assert(get_other_result == AWS_OP_SUCCESS);
    }

    assert(aws_array_list_is_valid(&list));
    assert(a < list.length);
    assert(b < list.length);

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    uint8_t new_item_a[MAX_ITEM_SIZE];
    uint8_t new_item_b[MAX_ITEM_SIZE];
    uint8_t new_item_other[MAX_ITEM_SIZE];

    int new_get_a_result = aws_array_list_get_at(&list, new_item_a, a);
    int new_get_b_result = aws_array_list_get_at(&list, new_item_b, b);
    assert(new_get_a_result == AWS_OP_SUCCESS);
    assert(new_get_b_result == AWS_OP_SUCCESS);

    assert_bytes_match(new_item_a, old_item_b, list.item_size);
    assert_bytes_match(new_item_b, old_item_a, list.item_size);

    if (check_other) {
        int new_get_other_result = aws_array_list_get_at(&list, new_item_other, other);
        assert(new_get_other_result == AWS_OP_SUCCESS);
        assert_bytes_match(new_item_other, old_item_other, list.item_size);
    }

    assert(aws_array_list_is_valid(&list));
}
