import pytest
import re
import subprocess


def get_stat(name):
        match = re.search(
            rf"{name}\s*:\s*([\d.]+)",
            result.stdout
        )
        assert match is not None, f"Missing {name} in simulator output"
        return match.group(1)


@pytest.mark.parametrize(
    "trace_file, pipe_width, forwardings_enable, br_pred_policy, "
    "expected_inst, expected_cycles, expected_cpi, "
    "expected_branches, expected_mispred", [
        ("sml.ptr.gz", 1, 0, 0, 100, 122, 1.220, 15, 0),
        ("sml.ptr.gz", 2, 0, 0, 100,  77, 0.770, 15, 0),
        ("sml.ptr.gz", 2, 1, 0, 100,  57, 0.570, 15, 0),
        ("sml.ptr.gz", 2, 1, 1, 100,  80, 0.800, 15, 7),
        ("sml.ptr.gz", 2, 1, 2, 100,  80, 0.800, 15, 7),
        ("gcc.ptr.gz", 1, 0, 0, 10_000_000, 11_412_411, 1.141, 964_441,       0),
        ("gcc.ptr.gz", 2, 0, 0, 10_000_000,  6_720_461, 0.672, 964_441,       0),
        ("gcc.ptr.gz", 2, 1, 0, 10_000_000,  5_206_917, 0.521, 964_441,       0),
        ("gcc.ptr.gz", 2, 1, 1, 10_000_000,  6_124_593, 0.612, 964_441, 265_253),
        ("gcc.ptr.gz", 2, 1, 2, 10_000_000,  5_251_368, 0.525, 964_441,  12_831),
    ], ids=[
        "sml-A1", "sml-A2", "sml-A3", "sml-B1", "sml-B2",
        "gcc-A1", "gcc-A2", "gcc-A3", "gcc-B1", "gcc-B2"
    ]
)
def test_golden(
    trace_file,
    pipe_width,
    forwardings_enable,
    br_pred_policy,
    expected_inst,
    expected_cycles,
    expected_cpi,
    expected_branches,
    expected_mispred
):    
    command = ["./build/bin/simulator", "traces/"+trace_file, "-pipewidth", str(pipe_width), "-bpredpolicy", str(br_pred_policy)]
    if forwardings_enable:
          command += ["-enableexefwd", "-enablememfwd"]
    result = subprocess.run(command, capture_output=True, text=True, timeout=1000)
    def get_stat(name):
        match = re.search(
            rf"{name}\s*:\s*([\d.]+)",
            result.stdout
        )
        assert match is not None, f"Missing {name} in simulator output"
        return match.group(1)

    assert int(get_stat("LAB2_NUM_INST")) == expected_inst
    assert int(get_stat("LAB2_NUM_CYCLES")) == expected_cycles
    assert float(get_stat("LAB2_CPI")) == pytest.approx(expected_cpi, abs=0.001)
    assert int(get_stat("LAB2_BPRED_BRANCHES")) == expected_branches
    assert int(get_stat("LAB2_BPRED_MISPRED")) == expected_mispred