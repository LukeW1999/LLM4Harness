#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_capacity_harness(void) {
    struct aws_array_list list;

    /* nondeterministic initialization of fields */
    list.item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.item_size > 0);

    list.current_size = (size_t)nondet_uint64_t();

    list.length = (size_t)nondet_uint64_t();

    /* ensure length * item_size does not overflow and fits in current_size */
    __CPROVER_assume(list.item_size != 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    /* allocate data buffer if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(__CPROVER_w_ok(list.data, list.current_size));
    } else {
        list.data = NULL;
    }

    /* use a valid allocator (static list not exercised here) */
    list.alloc = aws_default_allocator();

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
}
