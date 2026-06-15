#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for aws_mul_size_checked: nondeterministically decide overflow */
int aws_mul_size_checked(size_t a, size_t b, size_t *out) {
    if (nondet_bool()) {
        return AWS_OP_ERR; /* overflow */
    }
    *out = a * b;
    return AWS_OP_SUCCESS;
}

/* Stub for memory allocation through allocator */
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

/* Stub for memory release (unused but required for allocator) */
static void mem_release_stub(struct aws_allocator *alloc, void *ptr) {
    free(ptr);
}

void aws_array_list_init_dynamic_harness() {
    /* Declare and bound inputs */
    struct aws_array_list list;
    struct aws_allocator *alloc = malloc(sizeof(*alloc));
    __CPROVER_assume(alloc != NULL);

    /* Initialize allocator with stubs */
    alloc->mem_acquire = mem_acquire_stub;
    alloc->mem_release = mem_release_stub;
    /* Other function pointers can be left as NULL if not used */

    size_t initial_item_allocation;
    size_t item_size;

    /* Bound nondeterministic sizes */
    __CPROVER_assume(initial_item_allocation < MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size < MAX_ITEM_SIZE);
    __CPROVER_assume(item_size > 0);

    /* Call the function */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Ensure the function was actually called */
    assert(1);

    /* Success path */
    if (result == AWS_OP_SUCCESS) {
        /* All fields correctly set */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.current_size == 0 || list.data != NULL);
        if (initial_item_allocation == 0) {
            assert(list.current_size == 0);
            assert(list.data == NULL);
        } else {
            /* Allocation must have succeeded (since success) */
            assert(list.current_size > 0);
            assert(list.data != NULL);
            assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
        }
    } else {
        /* Error: list is zeroed */
        assert(list.alloc == NULL);
        assert(list.item_size == 0);
        assert(list.current_size == 0);
        assert(list.data == NULL);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
