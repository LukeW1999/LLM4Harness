#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    /* nondet inputs */
    struct aws_array_list list;
    size_t n;

    /* bound the list to keep the proof tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* ensure the data member is allocated and list is valid */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save the old state */
    struct aws_array_list old = list;
    size_t old_length = old.length;

    /* call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* postconditions: fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* postcondition: length change */
    if (old_length == 0 || n >= old_length) {
        assert(list.length == 0);
    } else if (n == 0) {
        assert(list.length == old_length);
    } else {
        assert(list.length == old_length - n);
    }

    /* postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));
}
