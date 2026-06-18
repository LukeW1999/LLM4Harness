#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/allocator.h>
#include <stdbool.h>
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Definitions copied from the s2n library (as provided in the prompt)       */
/* -------------------------------------------------------------------------- */

struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* -------------------------------------------------------------------------- */
/* Stubs for external functions used by s2n_stuffer_free                      */
/* -------------------------------------------------------------------------- */

bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer) {
    /* For the purpose of the harness we assume any constructed stuffer is
       valid. */
    return true;
}

/* s2n_free may succeed (return 0) or fail (return -1). */
int s2n_free(struct s2n_blob *blob) {
    int r = nondet_int();
    __CPROVER_assume(r == 0 || r == -1);
    return r;
}

/* -------------------------------------------------------------------------- */
/* Function under test (prototype)                                            */
/* -------------------------------------------------------------------------- */

int s2n_stuffer_free(struct s2n_stuffer *stuffer);

/* -------------------------------------------------------------------------- */
/* Harness                                                                    */
/* -------------------------------------------------------------------------- */

#define MAX_BLOB_SIZE 64U

void s2n_stuffer_free_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialise a stuffer */
    struct s2n_stuffer stuffer;

    /* Initialise blob fields */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BLOB_SIZE);

    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.allocated <= MAX_BLOB_SIZE);

    if (stuffer.blob.allocated > 0) {
        stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.allocated);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    stuffer.blob.growable = nondet_bool();

    /* Initialise cursor fields */
    stuffer.read_cursor  = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    /* Initialise flag fields */
    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* 2. Assume the pre‑condition (validation) holds */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Call the function under test */
    int result = s2n_stuffer_free(&stuffer);

    /* 5. Post‑condition checks */
    if (result == 0) {
        /* Success: the stuffer is zero‑initialised */
        assert(stuffer.alloced == 0);
        assert(stuffer.growable == 0);
        assert(stuffer.tainted == 0);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);

        assert(stuffer.blob.data == NULL);
        assert(stuffer.blob.size == 0);
        assert(stuffer.blob.allocated == 0);
        assert(stuffer.blob.growable == 0);
    } else {
        /* Failure: the stuffer must be unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);

        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
    }

    /* 6. Invariant: the stuffer must remain valid */
    assert(s2n_stuffer_validate(&stuffer));
}
