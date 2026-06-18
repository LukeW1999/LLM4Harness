#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    aws_array_list_clean_up(&list);

    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    if (old.alloc && old.data) {
        assert(list.alloc == NULL);
        assert(list.data == NULL);
    } else {
        assert(list.alloc == NULL);
        assert(list.data == NULL);
    }

    assert(aws_array_list_is_valid(&list));
}
