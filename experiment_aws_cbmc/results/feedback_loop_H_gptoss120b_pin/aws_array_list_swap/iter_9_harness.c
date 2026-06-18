#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 256

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_swap_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.allocator = allocator;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.length <= list.capacity);

    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t old_elem_a[MAX_ITEM_SIZE];
    uint8_t old_elem_b[MAX_ITEM_SIZE];
    uint8_t new_elem_a[MAX_ITEM_SIZE];
    uint8_t new_elem_b[MAX_ITEM_SIZE];

    int rc_a_old = aws_array_list_get_at(&list, old_elem_a, a);
    int rc_b_old = aws_array_list_get_at(&list, old_elem_b, b);
    assert(rc_a_old == AWS_OP_SUCCESS);
    assert(rc_b_old == AWS_OP_SUCCESS);

    aws_array_list_swap(&list, a, b);

    assert(list.allocator == old.allocator);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.capacity == old.capacity);
    assert(list.data == old.data);

    int rc_a_new = aws_array_list_get_at(&list, new_elem_a, a);
    int rc_b_new = aws_array_list_get_at(&list, new_elem_b, b);
    assert(rc_a_new == AWS_OP_SUCCESS);
    assert(rc_b_new == AWS_OP_SUCCESS);

    if (a == b) {
        assert(memcmp(new_elem_a, old_elem_a, list.item_size) == 0);
        assert(memcmp(new_elem_b, old_elem_b, list.item_size) == 0);
    } else {
        assert(memcmp(new_elem_a, old_elem_b, list.item_size) == 0);
        assert(memcmp(new_elem_b, old_elem_a, list.item_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));
}
