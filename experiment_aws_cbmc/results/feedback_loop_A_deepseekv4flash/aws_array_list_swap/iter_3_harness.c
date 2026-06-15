#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    /* Non-deterministic data structures */
    struct aws_array_list list;
    struct aws_allocator allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0);

    list.alloc = &allocator;

    size_t a;
    size_t b;
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old_list = list;

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    size_t sz = list.item_size;
    for (size_t i = 0; i < sz; i++) {
        assert(((uint8_t*)list.data)[a * sz + i] == ((uint8_t*)old_list.data)[b * sz + i]);
        assert(((uint8_t*)list.data)[b * sz + i] == ((uint8_t*)old_list.data)[a * sz + i]);
    }

    assert(aws_array_list_is_valid(&list));
}
