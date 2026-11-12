#!/usr/bin/env bash
set -euo pipefail

# Project paths
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SIM="$ROOT/build/bin/simulator"
MODE="${1:-quick}"

if [[ ! -x "$SIM" ]]; then
    echo "ERROR: Simulator not found: $SIM"
    echo "Compile your project first using make."
    exit 1
fi

if [[ ! "$MODE" =~ ^(smoke|quick|full|b2)$ ]]; then
    echo "Usage: ./run.sh [smoke|quick|full|b2]"
    exit 2
fi

python3 - "$ROOT" "$SIM" "$MODE" <<'PY'
import gzip
import math
import os
import re
import struct
import subprocess
import sys
from pathlib import Path

root, sim, mode = Path(sys.argv[1]), sys.argv[2], sys.argv[3]

traces = root / "traces"
logs = root / "test-results"
logs.mkdir(exist_ok=True)

# Configuration:
# (pipeline width, EX forwarding, MEM forwarding, predictor)
configs = {
    "A1":      (1, 0, 0, 0),
    "A2":      (2, 0, 0, 0),
    "A3":      (2, 1, 1, 0),
    "B1":      (2, 1, 1, 1),
    "B2":      (2, 1, 1, 2),

    "A2W4":    (4, 0, 0, 0),
    "A3W4":    (4, 1, 1, 0),

    "EXonly":  (2, 1, 0, 0),
    "MEMonly": (2, 0, 1, 0),

    "B1W4":    (4, 1, 1, 1),
    "B2W4":    (4, 1, 1, 2),
    "B2W1":    (1, 1, 1, 2),
}

standard = ("A1", "A2", "A3", "B1", "B2")
extras = (
    "A2W4", "A3W4",
    "EXonly", "MEMonly",
    "B1W4", "B2W4", "B2W1"
)

# Select tests.
if mode == "smoke":
    cases = [("sml", c) for c in (*standard, *extras)]

elif mode == "b2":
    cases = [
        ("sml", "B2"),
        ("sml", "B2W1"),
        ("sml", "B2W4"),
    ]

    cases += [
        (t, "B2")
        for t in ("libq", "mcf", "bzip2", "gcc")
    ]

    cases += [("gcc", "B2W4")]

else:
    cases = [("sml", c) for c in (*standard, *extras)]

    cases += [
        (t, c)
        for t in ("libq", "mcf")
        for c in standard
    ]

    cases += [
        (t, c)
        for t in ("bzip2", "gcc")
        for c in ("A3", "B1", "B2")
    ]

    if mode == "full":
        cases += [
            (t, c)
            for t in ("libq", "mcf")
            for c in ("EXonly", "MEMonly", "B1W4", "B2W4")
        ]

        cases += [
            (t, c)
            for t in ("bzip2", "gcc")
            for c in ("A1", "A2", "A3W4", "B2W4")
        ]

# Reference cycle counts for standard configurations.
ref_cycles = {
    "sml": {
        "A1": 122,
        "A2": 77,
        "A3": 57,
        "B1": 80,
        "B2": 80,
    },
    "gcc": {
        "A1": 11412411,
        "A2": 6720461,
        "A3": 5206917,
        "B1": 6124593,
        "B2": 5251368,
    },
}

# Original Trace_Rec binary layout:
# 48 bytes per instruction on 64-bit Linux.
record = struct.Struct("<Q9B7xQ3B5xQ")


def trace_expected(name):
    """Compute instruction and prediction counts independently."""

    path = traces / f"{name}.ptr.gz"

    if not path.is_file():
        raise FileNotFoundError(f"Missing trace: {path}")

    inst_count = 0
    branches = 0
    always_wrong = 0
    gshare_wrong = 0
    ghr = 0

    # 4096 two-bit counters, initially weakly taken.
    pht = bytearray([2]) * 4096

    with gzip.open(path, "rb") as stream:
        while True:
            chunk = stream.read(record.size * 16384)

            if not chunk:
                break

            if len(chunk) % record.size:
                raise ValueError(f"Invalid trace layout: {path}")

            for inst in record.iter_unpack(chunk):
                inst_count += 1

                if inst[1] != 3:  # OP_CBR
                    continue

                branches += 1

                pc = inst[0]
                direction = inst[13]

                if direction not in (0, 1):
                    raise ValueError("Invalid branch direction")

                # B1: Always Taken
                always_wrong += (direction == 0)

                # B2: Gshare
                index = (pc & 0xFFF) ^ ghr
                predicted_taken = pht[index] >= 2

                gshare_wrong += (
                    predicted_taken != bool(direction)
                )

                if direction:
                    pht[index] = min(3, pht[index] + 1)
                else:
                    pht[index] = max(0, pht[index] - 1)

                ghr = ((ghr << 1) | direction) & 0xFFF

    return inst_count, branches, always_wrong, gshare_wrong


expected = {
    t: trace_expected(t)
    for t, _ in cases
}

print(
    f'{"TRACE":6} {"CONFIG":8} {"CYCLES":>12} '
    f'{"STATUS":6} DETAILS',
    flush=True
)

errors = 0
seen = {}

for name, label in cases:
    width, exe, mem, predictor = configs[label]

    command = [
        sim,
        "-pipewidth", str(width)
    ]

    if exe:
        command.append("-enableexefwd")

    if mem:
        command.append("-enablememfwd")

    if predictor:
        command += ["-bpredpolicy", str(predictor)]

    command.append(str(traces / f"{name}.ptr.gz"))

    logfile = logs / f"{name}_{label}.log"

    try:
        with logfile.open("w") as output:
            result = subprocess.run(
                command,
                stdout=output,
                stderr=subprocess.STDOUT,
                timeout=int(os.getenv("LAB2_TIMEOUT", "600"))
            )

        content = logfile.read_text(errors="replace")

        fields = {
            key: float(value)
            for key, value in re.findall(
                r"(?m)^\s*LAB2_([A-Z_]+)\s*:\s*(\d+(?:\.\d+)?)",
                content
            )
        }

        instructions, branches, misses_b1, misses_b2 = expected[name]

        reasons = []

        if result.returncode:
            reasons.append(f"exit={result.returncode}")

        for key in ("NUM_INST", "NUM_CYCLES", "CPI"):
            if key not in fields:
                reasons.append(f"missing {key}")

        if fields.get("NUM_INST") != instructions:
            reasons.append(
                f"expected {instructions} instructions"
            )

        cycles = fields.get("NUM_CYCLES")

        if cycles is not None:
            minimum = math.ceil(instructions / width) + 4

            if cycles < minimum:
                reasons.append("cycles below pipeline minimum")

            actual_cpi = cycles / instructions

            if abs(fields.get("CPI", -1) - actual_cpi) > 0.001:
                reasons.append("incorrect CPI")

        # Exact reference check, where available.
        golden = ref_cycles.get(name, {}).get(label)

        if golden is not None and cycles != golden:
            reasons.append(
                f"expected {golden} cycles"
            )

        # Branch-predictor checks.
        if predictor:
            if fields.get("BPRED_BRANCHES") != branches:
                reasons.append(
                    f"expected {branches} branches"
                )

            expected_misses = (
                misses_b1 if predictor == 1 else misses_b2
            )

            if fields.get("BPRED_MISPRED") != expected_misses:
                reasons.append(
                    f"expected {expected_misses} mispredictions"
                )

            if branches and "MISPRED_RATE" in fields:
                expected_rate = 100 * expected_misses / branches

                if abs(fields["MISPRED_RATE"] - expected_rate) > 0.002:
                    reasons.append("incorrect misprediction rate")

        seen[name, label] = cycles

        if reasons:
            errors += 1

        status = "FAIL" if reasons else "PASS"
        value = "-" if cycles is None else str(int(cycles))

        print(
            f"{name:6} {label:8} {value:>12} "
            f"{status:6} {'; '.join(reasons)}",
            flush=True
        )

    except subprocess.TimeoutExpired:
        errors += 1

        print(
            f"{name:6} {label:8} {'-':>12} "
            f"FAIL   timeout; see {logfile}",
            flush=True
        )

print("\nPerformance sanity checks:")

for name in {t for t, _ in cases}:
    for older, newer in (
        ("A2", "A3"),
        ("A3", "A3W4")
    ):
        a = seen.get((name, older))
        b = seen.get((name, newer))

        if a is not None and b is not None and b > a:
            print(
                f"WARNING: {name}: {newer} took {b:.0f} "
                f"cycles vs {older} {a:.0f}"
            )

print(
    f"\nRESULT: {len(cases) - errors}/{len(cases)} passed"
)

print(f"Logs: {logs}")

print(
    "Exact cycle-count references are available only "
    "for standard sml/gcc configurations."
)

print(
    "Other tests verify instruction and prediction counts, "
    "not exact cycle timing."
)

sys.exit(1 if errors else 0)
PY