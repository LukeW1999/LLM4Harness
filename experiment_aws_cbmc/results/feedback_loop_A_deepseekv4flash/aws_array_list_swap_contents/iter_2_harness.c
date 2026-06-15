#include <aws/common/array_list.h>
#include <stddef.h>

void aws_array_list_swap_contents_harness() {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    // Initialize fields nondeterministically
    list_a.alloc = malloc(sizeof(struct aws_allocator));
    list_b.alloc = list_a.alloc;
    __CPROVER_assume(list_a.alloc != NULL);

    list_a.item_size = nondet_size_t();
    __CPROVER_assume(list_a.item_size > 0);
    list_b.item_size = list_a.item_size;

    list_a.length = nondet_size_t();
    list_b.length = nondet_size_t();
    list_a.current_size = nondet_size_t();
    list_b.current_size = nondet_size_t();

    // Ensure consistency: current_size >= length * item_size
    __CPROVER_assume(list_a.current_size >= list_a.length * list_a.item_size);
    __CPROVER_assume(list_b.current_size >= list_b.length * list_b.item_size);

    // Allocate data pointers (or set to NULL if size is 0)
    list_a.data = malloc(list_a.current_size > 0 ? list_a.current_size : 1);
    list_b.data = malloc(list_b.current_size > 0 ? list_b.current_size : 1);
    __CPROVER_assume(list_a.data != NULL || list_a.current_size == 0);
    __CPROVER_assume(list_b.data != NULL || list_b.current_size == 0);

    // Ensure both lists are valid and distinct
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));
    __CPROVER_assume(&list_a != &list_b);

    // Save old states
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    // Call the function under test
    aws_array_list_swap_contents(&list_a, &list_b);

    // Postcondition assertions
    __CPROVER_assert(list_a.current_size == old_b.current_size, "a.current_size == old_b.current_size");
    __CPROVER_assert(list_a.length == old_b.length, "a.length == old_b.length");
    __CPROVER_assert(list_a.item_size == old_b.item_size, "a.item_size == old_b.item_size");
    __CPROVER_assert(list_a.data == old_b.data, "a.data == old_b.data");
    __CPROVER_assert(list_a.alloc == old_b.alloc, "a.alloc == old_b.alloc");
    __CPROVER_assert(list_b.current_size == old_a.current_size, "b.current_size == old_a.current_size");
    __CPROVER_assert(list_b.length == old_a.length, "b.length == old_a.length");
    __CPROVER_assert(list_b.item_size == old_a.item_size, "b.item_size == old_a.item_size");
    __CPROVER_assert(list_b.data == old_a.data, "b.data == old_a.data");
    __CPROVER_assert(list_b.alloc == old_a.alloc, "b.alloc == old_a.alloc");
    __CPROVER_assert(aws_array_list_is_valid(&list_a), "list_a valid after swap");
    __CPROVER_assert(aws_array_list_is_valid(&list_b), "list_b valid after swap");
}
