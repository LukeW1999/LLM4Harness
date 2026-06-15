#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    /* nondeterministically initialize list */
    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    size_t length = nondet_size_t();
    size_t current_size = nondet_size_t();
    size_t n = nondet_size_t();

    /* assume valid constraints */
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(length <= current_size / item_size);
    __CPROVER_assume(current_size <= MAX_MALLOC_SIZE);
    list.length = length;
    list.item_size = item_size;
    list.current_size = current_size;
    list.alloc = NULL; /* no custom allocator */
    list.data = bounded_malloc(current_size);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old = list;
    size_t old_length = old.length;

    /* call the function */
    aws_array_list_pop_front_n(&list, n);

    /* postconditions */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* data contents: if n > 0 and n < old_length, the first (old_length - n) items match old data starting at offset n */
    if (n > 0 && n < old_length) {
        size_t remaining_bytes = (old_length - n) * item_size;
        /* ensure no overflow in pointer arithmetic */
        __CPROVER_assume(n * item_size <= old.current_size);
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data + n * item_size, remaining_bytes);
    }

    assert(aws_array_list_is_valid(&list));
}
