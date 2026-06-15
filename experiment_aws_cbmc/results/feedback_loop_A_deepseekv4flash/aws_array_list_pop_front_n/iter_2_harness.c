#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_pop_front_n_harness() {
    /* nondeterministic list and n */
    struct aws_array_list list;
    size_t n;

    /* ensure list is valid and has allocated data */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;
    size_t old_length = old.length;
    size_t item_size = old.item_size;

    /* call the function */
    aws_array_list_pop_front_n(&list, n);

    /* postconditions */
    /* alloc, item_size, data, current_size must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* length update */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* data contents: if n > 0 and n < old_length, the first (old_length - n) items should match old data starting at offset n */
    if (n > 0 && n < old_length) {
        size_t remaining_bytes = (old_length - n) * item_size;
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data + n * item_size, remaining_bytes);
    }

    /* list validity invariant */
    assert(aws_array_list_is_valid(&list));
}
