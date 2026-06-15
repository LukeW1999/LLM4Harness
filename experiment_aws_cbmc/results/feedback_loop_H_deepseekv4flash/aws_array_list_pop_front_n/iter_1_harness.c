#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n;

    /* Assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    size_t old_length = aws_array_list_length(&list);

    /* Call function */
    aws_array_list_pop_front_n(&list, n);

    /* Postconditions */
    size_t new_length = aws_array_list_length(&list);
    if (n >= old_length) {
        assert(new_length == 0);
    } else if (n > 0) {
        assert(new_length == old_length - n);
        /* Remaining elements are shifted to the front */
        size_t remaining_bytes = (old_length - n) * old.item_size;
        assert_bytes_match((const uint8_t *)list.data,
                           (const uint8_t *)old.data + n * old.item_size,
                           remaining_bytes);
    } else {
        /* n == 0: nothing changes */
        assert(new_length == old_length);
        size_t total_bytes = old_length * old.item_size;
        if (total_bytes > 0) {
            assert_bytes_match((const uint8_t *)list.data,
                               (const uint8_t *)old.data,
                               total_bytes);
        }
    }

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Validity invariant holds after call */
    assert(aws_array_list_is_valid(&list));
}
