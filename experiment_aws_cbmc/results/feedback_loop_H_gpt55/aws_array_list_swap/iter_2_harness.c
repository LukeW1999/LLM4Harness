#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t length = nondet_size_t();
    __CPROVER_assume(length > 0);
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(length <= SIZE_MAX / item_size);

    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.length = length;
    list.current_size = length * item_size;
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    for (size_t i = 0; i < list.current_size; ++i) {
        ((uint8_t *)list.data)[i] = (uint8_t)nondet_size_t();
    }

    assert(list.alloc == aws_default_allocator());
    assert(list.item_size > 0);
    assert(list.item_size <= MAX_ITEM_SIZE);
    assert(list.length > 0);
    assert(list.current_size == list.length * list.item_size);
    assert(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t() % list.length;
    size_t b = nondet_size_t() % list.length;

    assert(a < list.length);
    assert(b < list.length);

    struct aws_array_list old = list;

    uint8_t old_item_a[MAX_ITEM_SIZE];
    uint8_t old_item_b[MAX_ITEM_SIZE];

    int get_a_result = aws_array_list_get_at(&list, old_item_a, a);
    int get_b_result = aws_array_list_get_at(&list, old_item_b, b);

    assert(get_a_result == AWS_OP_SUCCESS);
    assert(get_b_result == AWS_OP_SUCCESS);
    assert(aws_array_list_is_valid(&list));

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    uint8_t new_item_a[MAX_ITEM_SIZE];
    uint8_t new_item_b[MAX_ITEM_SIZE];

    int new_get_a_result = aws_array_list_get_at(&list, new_item_a, a);
    int new_get_b_result = aws_array_list_get_at(&list, new_item_b, b);

    assert(new_get_a_result == AWS_OP_SUCCESS);
    assert(new_get_b_result == AWS_OP_SUCCESS);

    assert_bytes_match(new_item_a, old_item_b, list.item_size);
    assert_bytes_match(new_item_b, old_item_a, list.item_size);

    assert(aws_array_list_is_valid(&list));
}
