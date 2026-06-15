#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;
    void *val;

    /* Nondeterministic list; ensure it is valid and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, NULL, NULL)); // no strict bounds
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Require item_size > 0 so that operations are meaningful */
    __CPROVER_assume(list.item_size > 0);

    /* Nondeterministic index, but prevent overflow in multiplication */
    __CPROVER_assume(index == (index * list.item_size) / list.item_size);

    /* Allocate nondet buffer of the correct size */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Assert that the list remains valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* Check that the allocator and item_size are unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the element at index matches the value written */
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                           (const uint8_t *)val, list.item_size);
        /* Length is updated correctly */
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
    } else {
        /* On failure, the list must be unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }
}
