#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

void aws_array_list_capacity_harness() {
    /* Choose small, bounded nondet values to keep verification tractable */
    size_t item_size = (size_t)nondet_uint8_t();
    __CPROVER_assume(item_size > 0 && item_size <= 8);

    size_t capacity = (size_t)nondet_uint8_t();
    __CPROVER_assume(capacity > 0 && capacity <= 8);

    struct aws_array_list list;
    list.item_size = item_size;
    list.capacity = capacity;

    /* Length must be within capacity */
    list.length = (size_t)nondet_uint8_t();
    __CPROVER_assume(list.length <= list.capacity);

    /* Current size is capacity multiplied by item size */
    list.current_size = list.capacity * list.item_size;

    /* Provide a non‑NULL data pointer when capacity is non‑zero */
    list.data = (void *)nondet_uint64_t();
    __CPROVER_assume(list.data != NULL);

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t cap = aws_array_list_capacity(&list);

    /* The capacity must be at least the current length */
    assert(cap >= list.length);
    /* Capacity expressed in number of items should match the underlying buffer size */
    assert(cap * list.item_size == list.current_size);

    assert(aws_array_list_is_valid(&list));
}
