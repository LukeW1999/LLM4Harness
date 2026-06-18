#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 64
#define MAX_INITIAL_ITEM_ALLOCATION 10

void aws_array_list_front_harness() {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size within bounds */
    size_t item_size = (size_t)__CPROVER_nondet_uint();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* initialize the list */
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* ensure list has at least one element */
    __CPROVER_assume(list.length > 0);

    /* save a byte from the first element */
    struct store_byte_from_buffer storage;
    uint8_t *first_elem = (uint8_t *)list.data;
    save_byte_from_array(first_elem, list.item_size, &storage);

    /* call the function under test */
    void *front = aws_array_list_front(&list);

    /* postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(front == list.data);
    assert_byte_from_buffer_matches(first_elem, &storage);
}
