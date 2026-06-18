#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

/* External function under test */
extern int s2n_stuffer_rewrite(struct s2n_stuffer *stuffer);

/* Nondeterministic helpers (provided by proof environment) */
extern bool nondet_bool(void);
extern uint32_t nondet_uint32_t(void);

/* Bounding macro (provided by the build system) */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

/* Success code (if not defined elsewhere) */
#ifndef S2N_SUCCESS
#define S2N_SUCCESS 0
#endif

void s2n_stuffer_rewrite_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct s2n_stuffer stuffer;

    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    stuffer.blob.growable = nondet_bool();

    if (blob_size > 0) {
        stuffer.blob.data = aws_mem_acquire(allocator, blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    struct s2n_stuffer old = stuffer;

    int result = s2n_stuffer_rewrite(&stuffer);

    if (result == S2N_SUCCESS) {
        assert(stuffer.write_cursor == 0);
        assert(stuffer.read_cursor == 0);
    } else {
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.read_cursor == old.read_cursor);
    }

    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.high_water_mark == old.high_water_mark);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);

    assert(s2n_stuffer_validate(&stuffer));

    if (stuffer.blob.data != NULL) {
        aws_mem_release(allocator, stuffer.blob.data);
    }
}
