#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    /* nondet inputs */
    struct aws_array_list list;
    size_t index;
    size_t item_size;

    /* bounded data structures */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* precondition: val points to readable memory of size item_size */
    size_t item_size_val = list.item_size;
    void *val = malloc(item_size_val);
    __CPROVER_assume(val != NULL);

    /* assume index is not so large that it would overflow size_t arithmetic */
    __CPROVER_assume(index < SIZE_MAX);

    /* save old state */
    struct aws_array_list old = list;

    /* call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* VALIDITY INVARIANT: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* UNCHANGED FIELDS (always): allocator and item_size */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* SUCCESS POSTCONDITIONS */
        /* the value at index is copied from val */
        assert_bytes_match(((uint8_t *)list.data) + index * item_size_val, (uint8_t *)val, item_size_val);

        /* length is updated if index >= old.length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* FAILURE POSTCONDITIONS: list is completely unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }
}
