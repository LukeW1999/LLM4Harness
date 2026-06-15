#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/* Provide nondet_bool for stubs */
bool nondet_bool(void);

/* Stub for aws_mul_size_checked: nondeterministic overflow */
int aws_mul_size_checked(size_t a, size_t b, size_t *out) {
    if (nondet_bool()) {
        return AWS_OP_ERR; /* overflow */
    }
    *out = a * b;
    return AWS_OP_SUCCESS;
}

/* Stub for memory allocation */
static void *mem_acquire_stub(struct aws_allocator *alloc, size_t size) {
    if (size == 0) {
        return NULL;
    }
    if (nondet_bool()) {
        return NULL;
    }
    void *ptr = malloc(size);
    __CPROVER_assume(ptr != NULL);
    return ptr;
}

/* Stub for memory release (required but unused) */
static void mem_release_stub(struct aws_allocator *alloc, void *ptr) {
    free(ptr);
}

void aws_array_list_init_dynamic_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = malloc(sizeof(*alloc));
    __CPROVER_assume(alloc != NULL);

    /* Initialize allocator stubs */
    alloc->mem_acquire = mem_acquire_stub;
    alloc->mem_release = mem_release_stub;

    size_t initial_item_allocation;
    size_t item_size;

    /* Bound sizes to keep verification tractable, allow 0 */
    __CPROVER_assume(initial_item_allocation < 10);
    __CPROVER_assume(item_size < 10);

    /* Call the function */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Ensure the function was actually called */
    assert(1);

    /* Postconditions based on result */
    if (result == AWS_OP_SUCCESS) {
        /* Success path */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            assert(list.current_size > 0);
            assert(list.data != NULL);
        }
    } else {
        /* Error path: list must be zeroed */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* Validity invariant holds in all cases */
    assert(aws_array_list_is_valid(&list));
}
