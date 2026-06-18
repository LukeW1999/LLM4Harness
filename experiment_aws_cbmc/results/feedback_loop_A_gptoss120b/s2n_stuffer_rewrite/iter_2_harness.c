#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

/* External function under test */
extern int s2n_stuffer_rewrite(struct s2n_stuffer *stuffer);

/* Nondeterministic helpers (provided by proof environment) */
extern bool nondet_bool(void);
extern uint32_t nondet_uint32_t(void);

/* Bounding macro (provided by the build system) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void s2n_stuffer_rewrite_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialise a stuffer */
    struct s2n_stuffer stuffer;

    /* Initialise the blob */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;               /* allocated >= size */
    stuffer.blob.growable = nondet_bool();

    /* Allocate memory for the blob data */
    if (blob_size > 0) {
        stuffer.blob.data = aws_mem_acquire(allocator, blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Initialise cursors and other fields */
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* 2. Precondition: the stuffer must be valid */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Call the function under test */
    int result = s2n_stuffer_rewrite(&stuffer);

    /* 5. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* Changed fields */
        assert(stuffer.write_cursor == 0);
        assert(stuffer.read_cursor == 0);
    } else {
        /* On failure the stuffer must remain unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.read_cursor == old.read_cursor);
    }

    /* Unchanged fields (regardless of success or failure) */
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.high_water_mark == old.high_water_mark);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);

    /* 6. Validity invariant must hold after the call */
    assert(s2n_stuffer_validate(&stuffer));

    /* Clean up */
    if (stuffer.blob.data != NULL) {
        aws_mem_release(allocator, stuffer.blob.data);
    }
}
