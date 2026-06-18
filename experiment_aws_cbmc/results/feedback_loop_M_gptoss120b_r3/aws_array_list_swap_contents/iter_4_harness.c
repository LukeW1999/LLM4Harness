#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_contents_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    struct aws_array_list list_a;
    aws_array_list_init(&list_a, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_sz);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));

    struct aws_array_list list_b;
    aws_array_list_init(&list_b, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_sz);
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    __CPROVER_assume(&list_a != &list_b);

    /* bound the lengths to keep CBMC tractable */
    __CPROVER_assume(list_a.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list_b.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list_a.current_size <= list_a.length);
    __CPROVER_assume(list_b.current_size <= list_b.length);

    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;

    if (list_a.data != NULL && list_a.current_size > 0) {
        save_byte_from_array(list_a.data, list_a.current_size * list_a.item_size, &storage_a);
    }
    if (list_b.data != NULL && list_b.current_size > 0) {
        save_byte_from_array(list_b.data, list_b.current_size * list_b.item_size, &storage_b);
    }

    aws_array_list_swap_contents(&list_a, &list_b);

    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);

    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);

    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);

    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    assert_array_list_equivalence(&list_a, &old_b, &storage_b);
    assert_array_list_equivalence(&list_b, &old_a, &storage_a);

    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
