#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_contents_harness() {
    /* Allocate two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* Use the default allocator for both */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize with bounded sizes to avoid timeout */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 2);

    size_t length_a;
    size_t current_size_a;
    __CPROVER_assume(length_a <= 2);
    __CPROVER_assume(current_size_a <= 2 * item_size);
    __CPROVER_assume(current_size_a >= length_a * item_size);

    size_t length_b;
    size_t current_size_b;
    __CPROVER_assume(length_b <= 2);
    __CPROVER_assume(current_size_b <= 2 * item_size);
    __CPROVER_assume(current_size_b >= length_b * item_size);

    list_a.alloc = allocator;
    list_a.item_size = item_size;
    list_a.length = length_a;
    list_a.current_size = current_size_a;
    if (current_size_a > 0) {
        list_a.data = malloc(current_size_a);
        __CPROVER_assume(list_a.data != NULL);
    } else {
        list_a.data = NULL;
    }

    list_b.alloc = allocator;
    list_b.item_size = item_size;
    list_b.length = length_b;
    list_b.current_size = current_size_b;
    if (current_size_b > 0) {
        list_b.data = malloc(current_size_b);
        __CPROVER_assume(list_b.data != NULL);
    } else {
        list_b.data = NULL;
    }

    /* Ensure both lists are valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* Verify frame conditions: list_a now has list_b's old contents */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    /* Verify frame conditions: list_b now has list_a's old contents */
    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    /* Verify validity invariants after the call */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
