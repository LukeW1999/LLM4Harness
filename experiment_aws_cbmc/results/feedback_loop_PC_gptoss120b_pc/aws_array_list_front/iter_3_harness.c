#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 64
#define MAX_INITIAL_ITEM_ALLOCATION 10

void aws_array_list_swap_harness() {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size within bounds */
    size_t item_size = nondet_size_t();
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

    /* snapshot old state */
    struct aws_array_list old = list;

    /* choose nondeterministic indices within bounds */
    size_t len = list.length;
    __CPROVER_assume(len > 0);
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* save a byte from each element to compare after the swap */
    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;
    uint8_t *ptr_a = (uint8_t *)list.data + a * list.item_size;
    uint8_t *ptr_b = (uint8_t *)list.data + b * list.item_size;
    __CPROVER_assume(AWS_MEM_IS_READABLE(ptr_a, list.item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(ptr_b, list.item_size));
    save_byte_from_array(ptr_a, list.item_size, &storage_a);
    save_byte_from_array(ptr_b, list.item_size, &storage_b);

    /* call the function under test */
    aws_array_list_swap(&list, a, b);

    /* postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);
    assert_byte_from_buffer_matches(ptr_a, &storage_b);
    assert_byte_from_buffer_matches(ptr_b, &storage_a);
}
