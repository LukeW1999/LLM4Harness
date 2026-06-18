#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < aws_array_list_length(&list));
    __CPROVER_assume(b < aws_array_list_length(&list));

    size_t item_size = list.item_size;
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    memcpy(old_a, (uint8_t *)list.data + a * item_size, item_size);
    memcpy(old_b, (uint8_t *)list.data + b * item_size, item_size);

    struct aws_array_list old = list;

    aws_array_list_swap(&list, a, b);

    assert(aws_array_list_is_valid(&list));

    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    assert(memcmp((uint8_t *)list.data + a * item_size, old_b, item_size) == 0);
    assert(memcmp((uint8_t *)list.data + b * item_size, old_a, item_size) == 0);
}
