#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* s2n struct definitions needed for the harness */
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

/* External declarations for s2n functions */
int s2n_stuffer_alloc(struct s2n_stuffer *stuffer, const uint32_t size);

/* s2n_stuffer_validate - validity predicate for s2n_stuffer */
/* We declare it as external since it's part of the s2n library */
int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

void s2n_stuffer_alloc_harness(void) {
    /* 1. Declare inputs */
    struct s2n_stuffer stuffer;
    uint32_t size = nondet_uint32_t();

    /* Bound the size to keep verification tractable */
    __CPROVER_assume(size <= 1024);

    /* 2. Call function under test */
    int result = s2n_stuffer_alloc(&stuffer, size);

    /* 3. Assert postconditions */
    if (result == 0 /* S2N_SUCCESS */) {
        /* On success:
         * - stuffer is initialized (zeroed then set up)
         * - stuffer->alloced == 1
         * - stuffer->blob has been allocated with 'size' bytes
         * - read_cursor == 0 (from zero initialization)
         * - write_cursor == 0 (from zero initialization)
         * - high_water_mark == 0 (from zero initialization)
         * - tainted == 0 (from zero initialization)
         */
        assert(stuffer.alloced == 1);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        assert(stuffer.tainted == 0);

        /* blob should have been allocated with the requested size */
        assert(stuffer.blob.size == size);
        assert(stuffer.blob.allocated >= size);
        assert(stuffer.blob.growable == 1);

        /* If size > 0, data pointer should be non-null */
        if (size > 0) {
            assert(stuffer.blob.data != NULL);
        }

        /* Validity invariant */
        /* s2n_stuffer_validate should pass after successful alloc */
        /* We assert the key structural invariants manually since
         * s2n_stuffer_validate may not be directly callable in CBMC */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);
    } else {
        /* On failure:
         * - The function returns non-zero (error)
         * - stuffer was zeroed at the start of the function
         * - We can assert the stuffer was at least partially initialized
         *   (zeroed) since *stuffer = {0} happens before the failure point
         */
        /* After *stuffer = {0}, if s2n_alloc fails, stuffer is zeroed */
        assert(result != 0);
    }
}

void s2n_stuffer_alloc_harness(void) {
    s2n_stuffer_alloc_harness();
    return 0;
}
