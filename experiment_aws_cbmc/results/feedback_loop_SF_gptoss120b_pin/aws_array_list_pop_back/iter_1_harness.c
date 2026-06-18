#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondet helpers */
size_t nondet_size_t(void);
void *nondet_ptr(void);

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;

    /* allocator – use the default allocator */
    list.alloc = aws_default_allocator();

    /* nondeterministic item size, must be > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* nondeterministic length and current size */
    list.length = nondet_size_t();
    list.current_size = nondet_size_t();

    /* ensure length * item_size does not overflow and fits within current_size */
    size_t required_size;
    __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &required_size));
    __CPROVER_assume(required_size <= list.current_size);

    /* allocate backing storage if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* enforce the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of relevant state */
    size_t orig_len = list.length;
    void *orig_data = list.data;
    size_t orig_item_size = list.item_size;
    size_t orig_current_size = list.current_size;

    /* call the function under verification */
    int ret = aws_array_list_pop_back(&list);

    /* postcondition assertions */
    /* list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* allocator, item size, current size, and data pointer are unchanged */
    assert(list.alloc == aws_default_allocator());
    assert(list.item_size == orig_item_size);
    assert(list.current_size == orig_current_size);
    assert(list.data == orig_data);

    /* length and return value follow specification */
    if (orig_len > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == orig_len - 1);
    } else {
        assert(ret != AWS_OP_SUCCESS);
        assert(list.length == orig_len);
    }

    /* frame condition: underlying buffer contents are unchanged */
    if (orig_data != NULL) {
        size_t total_bytes = orig_len * orig_item_size;
        for (size_t i = 0; i < total_bytes; ++i) {
            assert(((unsigned char *)list.data)[i] == ((unsigned char *)orig_data)[i]);
        }
    }
}
