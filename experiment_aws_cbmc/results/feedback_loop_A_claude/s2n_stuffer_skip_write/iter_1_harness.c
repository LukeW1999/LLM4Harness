int s2n_stuffer_skip_write(struct s2n_stuffer *stuffer, const uint32_t n);
```

```c
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* s2n struct definitions */
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

int s2n_stuffer_skip_write(struct s2n_stuffer *stuffer, const uint32_t n);

/* s2n validity predicate */
bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->high_water_mark) return false;
    if (stuffer->high_water_mark > stuffer->blob.size) return false;
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_skip_write_harness(void) {
    /* 1. Declare and set up a valid s2n_stuffer */
    struct s2n_stuffer stuffer;

    /* Set up blob with non-deterministic but bounded size */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= 1024); /* bound state space */

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size || stuffer.blob.allocated == 0);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Set up cursors with valid invariants */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Verify precondition */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Non-deterministic n */
    uint32_t n = nondet_uint32_t();
    __CPROVER_assume(n <= 1024);

    /* 4. Call function under test */
    int result = s2n_stuffer_skip_write(&stuffer, n);

    /* 5. Assert postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        /* write_cursor advanced by n */
        assert(stuffer.write_cursor == old.write_cursor + n);
        /* read_cursor unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        /* blob unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        /* alloced/growable/tainted unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        /* high_water_mark >= write_cursor */
        assert(stuffer.high_water_mark >= stuffer.write_cursor);
        /* validity */
        assert(s2n_stuffer_is_valid(&stuffer));
    } else {
        /* On failure, stuffer should remain valid */
        assert(s2n_stuffer_is_valid(&stuffer));
        /* Cursors should not have changed on failure */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
    }
}

void s2n_stuffer_skip_write_harness(void) {
    s2n_stuffer_skip_write_harness();
    return 0;
}
