#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;

    size_t n;
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);

    aws_array_list_pop_front_n(&list, n);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.data == old_list.data);

    if (n >= old_list.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_list.length - n);
        size_t moved_items = old_list.length - n;
        size_t moved_bytes = moved_items * old_list.item_size;
        size_t offset_bytes = n * old_list.item_size;
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old_list.data + offset_bytes,
                           moved_bytes);
    }
}
