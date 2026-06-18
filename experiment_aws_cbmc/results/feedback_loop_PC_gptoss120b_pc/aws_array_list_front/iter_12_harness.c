#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 64
#define MAX_INITIAL_ITEM_ALLOCATION 10

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = (size_t)__CPROVER_nondet_uint();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.length > 0);

    struct store_byte_from_buffer storage;
    uint8_t *first_elem = (uint8_t *)list.data;
    save_byte_from_array(first_elem, list.item_size, &storage);

    void *front = aws_array_list_front(&list);

    assert(aws_array_list_is_valid(&list));
    assert(front == list.data);
    assert_byte_from_buffer_matches(first_elem, &storage);
}
