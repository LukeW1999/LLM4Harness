#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_back_harness() {
    struct aws_array_list list;
    void *val;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_length = nondet_size_t();

    ensure_array_list_has_allocated_data_member(&list);
    list.item_size = max_item_size;
    list.current_size = max_item_alloc * max_item_size;
    list.length = initial_length;
    list.data = malloc(list.current_size);

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size == list.item_size * max_item_alloc);
    __CPROVER_assume(list.length <= max_item_alloc);
    __CPROVER_assume(initial_length > 0); // Ensure there is at least one element to get the back from

    val = malloc(max_item_size);

    struct aws_array_list old_list = list;
    void *old_val = val;

    __CPROVER_assume(aws_array_list_is_valid(&list)); // Ensure the list is valid before calling aws_array_list_back

    int result = aws_array_list_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(memcmp(val, (uint8_t *)list.data + (list.item_size * (list.length - 1)), list.item_size) == 0);
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(memcmp(val, old_val, list.item_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));

    free(list.data);
    free(val);
}
