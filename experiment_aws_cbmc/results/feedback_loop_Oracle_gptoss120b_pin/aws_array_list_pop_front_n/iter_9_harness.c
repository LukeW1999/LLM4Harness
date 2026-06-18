#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    void *old_ptr;
    uint8_t *old_data;
    size_t old_length;
    size_t old_capacity;
    size_t n;

    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    /* item size */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 256);

    /* capacity (number of items that can be stored) */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 1024);

    /* allocate backing storage */
    list.data = aws_mem_acquire(list.alloc, capacity * list.item_size);
    __CPROVER_assume(list.data != NULL);
    list.current_size = capacity * list.item_size;

    /* current length (number of items) */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= capacity);

    /* structural assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, 1024, 256));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* keep a copy of the original state */
    old_ptr = list.data;
    old_capacity = list.current_size / list.item_size;
    old_length   = list.length;
    old_data = malloc(list.current_size);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, list.data, list.current_size);

    /* nondet number of elements to pop */
    n = nondet_size_t();
    __CPROVER_assume(n <= old_length);
    __CPROVER_assume(n <= ((size_t)-1) / list.item_size); /* avoid overflow */

    /* call the function under verification */
    int rc = aws_array_list_pop_front_n(&list, n);
    assert(rc == AWS_OP_SUCCESS);

    /* -------------------------------------------------------------------- */
    /* Post‑condition checks */

    /* 1. The list must remain a valid array list */
    assert(aws_array_list_is_valid(&list));

    /* 2. Length invariant */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* 3. The backing storage pointer must not change */
    assert(list.data == old_ptr);

    /* 4. The allocated size (capacity) must stay the same */
    assert(list.current_size == old_capacity * list.item_size);

    /* 5. Elements that remain must be shifted correctly.
       Only check the first remaining element when there is at least one. */
    if (n < old_length && list.length > 0) {
        assert(memcmp(
            (uint8_t *)list.data,
            old_data + n * list.item_size,
            list.item_size) == 0);
    }

    /* clean up */
    free(old_data);
    aws_mem_release(list.alloc, list.data);
}
