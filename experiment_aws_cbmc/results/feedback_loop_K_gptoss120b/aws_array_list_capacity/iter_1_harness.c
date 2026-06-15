#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
void *nondet_pointer(void);

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;

    /* nondet initialization of fields */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = nondet_size_t();

    list.length = nondet_size_t();

    /* ensure length * item_size does not overflow and fits in current_size */
    size_t required_size;
    __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &required_size));
    __CPROVER_assume(required_size <= list.current_size);

    /* allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(__CPROVER_w_ok(list.data, list.current_size));
    } else {
        list.data = NULL;
    }

    /* allocator can be NULL (static list) or a valid allocator; not relevant for capacity */
    list.alloc = nondet_pointer();

    /* assume the list satisfies its invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of the whole list structure */
    struct aws_array_list old = list;

    /* snapshot of the data buffer */
    void *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* call the function under test */
    size_t cap = aws_array_list_capacity(&list);

    /* post‑condition: return value matches definition */
    assert(cap == list.current_size / list.item_size);

    /* post‑condition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* frame conditions: structural fields unchanged */
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* frame condition: data buffer unchanged */
    if (list.current_size > 0) {
        assert(__CPROVER_same_object(list.data, old.data));
        assert(memcmp(list.data, old_data, list.current_size) == 0);
    }

    return 0;
}
