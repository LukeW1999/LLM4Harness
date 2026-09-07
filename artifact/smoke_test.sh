#!/usr/bin/env bash
# Five-minute functional check: recompute every number in the paper.
set -uo pipefail
cd "$(dirname "$0")/../experiment_aws_cbmc" || exit 1

fail=0
note() { printf '%-58s %s\n' "$1" "$2"; }

python3 -c "import numpy, scipy" 2>/dev/null \
  && note "python deps (numpy, scipy)" "ok" \
  || { note "python deps (numpy, scipy)" "MISSING -> pip install -r requirements.txt"; fail=1; }

for f in evaluation/gtfail_640.json evaluation/silenced_640.json \
         evaluation/adjudicated_mechanism.json; do
  [ -s "$f" ] && note "released verdicts: $f" "ok" || { note "released verdicts: $f" "MISSING"; fail=1; }
done

out=$(python3 scripts/paper_numbers_640.py 2>&1)
echo "$out" | tail -1
if echo "$out" | grep -q "0 mismatch"; then
  note "audit registry" "ok"
else
  note "audit registry" "MISMATCH -- see the rows marked **MISMATCH** above"
  echo "$out" | grep "MISMATCH"
  fail=1
fi

echo
if [ "$fail" -eq 0 ]; then
  echo "PASS: every number in the paper was recomputed from the released verdicts."
else
  echo "FAIL: see the lines above."
fi
exit "$fail"
