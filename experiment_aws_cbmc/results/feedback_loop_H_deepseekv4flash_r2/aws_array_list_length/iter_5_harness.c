#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 100

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t item_size;
    size_t length;
    size_t current_size;
    void *data;

    /* nondeterministic initialization */
    item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    length = nondet_size_t();
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    current_size = length * item_size;
    data = malloc(current_size);
    __CPROVER_assume(data != NULL);

    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;
    list.data = data;

    /* Ensure the list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Input value and index */
    size_t index;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;

    /* Save a copy of val */
    uint8_t *saved_val = malloc(item_size);
    __CPROVER_assume(saved_val != NULL);
    memcpy(saved_val, val, item_size);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* Check that the element at index matches val */
        uint8_t *target = (uint8_t *)list.data + index * item_size;
        assert_bytes_match(target, val, item_size);
        /* val should be unchanged */
        assert_bytes_match(val, saved_val, item_size);
    } else {
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    free(saved_val);
    free(data);
}
