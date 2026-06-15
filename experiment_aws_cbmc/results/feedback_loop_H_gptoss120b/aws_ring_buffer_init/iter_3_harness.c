#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>

/* Maximum size for the ring buffer in the harness */
#define MAX_BUFFER_SIZE 1024U

/* Nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);

/* Stub allocator functions */
static void *harness_allocate(struct aws_allocator *allocator, size_t size) {
    void *ptr = NULL;
    if (nondet_bool()) {
        /* Return a fresh non‑NULL block */
        ptr = __CPROVER_allocate(size, 0);
        __CPROVER_assume(__CPROVER_is_fresh(ptr, size));
    }
    return ptr;
}

static void harness_deallocate(struct aws_allocator *allocator, void *ptr) {
    /* No‑op for the harness */
    (void)allocator;
    (void)ptr;
}
