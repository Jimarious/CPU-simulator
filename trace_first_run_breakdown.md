# First-run trace: instruction-by-instruction breakdown

**Companion file:** `trace_first_run.bin` (240 bytes, five 48-byte records). These are synthetic test records, **not** an original university trace. The field names and opcode values match the assignment’s `Trace_Rec` in `lab2/src/trace.h`. Binary records are little-endian with the usual 64-bit x86 alignment/padding.

## Conceptual instruction sequence

```text
1. ALU R1, R2, R3         -- R1 gets the result of an ALU operation on R2 and R3
2. ALU R4, R1, R5         -- Reads R1 produced by instruction 1
3. LD  R6, [0x1000]       -- Uses R4 as an address source; effective address = 0x1000
4. ALU R7, R6, R8         -- Reads R6 produced by instruction 3; writes condition codes
5. CBR 0x400020           -- Reads condition codes; branch outcome = not taken
```

**Note:** These are pseudo-instructions. `OP_ALU` does not specify ADD versus SUB or another arithmetic operation; the trace does not contain exact assembly syntax or register values.

## All register/identity fields

| # | inst_addr | op_type | dest | dest_needed | src1_reg | src2_reg | src1_needed | src2_needed |
|---|---|---|---|---|---|---|---|---|
| 1 | `0x400000` | 0 (`OP_ALU`) | R1 | 1 (yes) | R2 | R3 | 1 (yes) | 1 (yes) |
| 2 | `0x400004` | 0 (`OP_ALU`) | R4 | 1 (yes) | R1 | R5 | 1 (yes) | 1 (yes) |
| 3 | `0x400008` | 1 (`OP_LD`) | R6 | 1 (yes) | R4 | — (unused; stored 0) | 1 (yes) | 0 (no) |
| 4 | `0x40000C` | 0 (`OP_ALU`) | R7 | 1 (yes) | R6 | R8 | 1 (yes) | 1 (yes) |
| 5 | `0x400010` | 3 (`OP_CBR`) | — (unused; stored 0) | 0 (no) | — (unused; stored 0) | — (unused; stored 0) | 0 (no) | 0 (no) |

## All condition-code, memory, and branch fields

| # | cc_read | cc_write | mem_addr | mem_write | mem_read | br_dir | br_target |
|---|---|---|---|---|---|---|---|
| 1 | 0 (no) | 0 (no) | 0 (unused) | 0 (no) | 0 (no) | 0 (not applicable) | 0 (unused) |
| 2 | 0 (no) | 0 (no) | 0 (unused) | 0 (no) | 0 (no) | 0 (not applicable) | 0 (unused) |
| 3 | 0 (no) | 0 (no) | `0x1000` | 0 (no) | 1 (yes) | 0 (not applicable) | 0 (unused) |
| 4 | 0 (no) | 1 (yes) | 0 (unused) | 0 (no) | 0 (no) | 0 (not applicable) | 0 (unused) |
| 5 | 1 (yes) | 0 (no) | 0 (unused) | 0 (no) | 0 (no) | 0 (not taken) | `0x400020` |

## What each column means

| Attribute | Explanation |
|---|---|
| `inst_addr` | Program address of this instruction (not its byte offset within the trace file). |
| `op_type` | Instruction category: `0=OP_ALU`, `1=OP_LD`, `2=OP_ST`, `3=OP_CBR`, `4=OP_OTHER`. |
| `dest` | Number of the destination register; valid only when `dest_needed=1`. |
| `dest_needed` | 1 means this instruction writes a destination register; 0 means no destination register. |
| `src1_reg` | Number of the first input register; valid only when `src1_needed=1`. |
| `src2_reg` | Number of the second input register; valid only when `src2_needed=1`. |
| `src1_needed` | 1 means `src1_reg` is an input to this instruction. |
| `src2_needed` | 1 means `src2_reg` is an input to this instruction. |
| `cc_read` | 1 means the instruction consumes condition codes/flags. |
| `cc_write` | 1 means the instruction produces or updates condition codes/flags. |
| `mem_addr` | Effective memory address for a load or store; unrelated to instruction address. |
| `mem_write` | 1 means the instruction writes memory (a store). |
| `mem_read` | 1 means the instruction reads memory (a load). |
| `br_dir` | Actual branch direction on conditional branch: `0=not taken`, `1=taken`; ignore for other instructions. |
| `br_target` | Branch target address; ignore for nonbranch instructions. |

## Dependencies this test is designed to exercise

| Producer | Consumer | Dependency | Why |
|---|---|---|---|
| 1 | 2 | RAW on R1 | Instruction 1 writes R1; instruction 2 reads R1. |
| 2 | 3 | RAW on R4 | Instruction 2 writes R4; load instruction 3 uses R4 as input. |
| 3 | 4 | RAW on R6 | Load instruction 3 writes R6; instruction 4 reads R6. |
| 4 | 5 | Condition codes | Instruction 4 sets flags; conditional branch 5 reads them. |

## Hex bytes for each complete record

Each line below corresponds to one 48-byte instruction (hex byte pairs). No file header or separators are stored: the records are concatenated directly.

```text
Instruction 1:
  +0x00: 00 00 40 00 00 00 00 00 00 01 01 02 03 01 01 00
  +0x10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  +0x20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Instruction 2:
  +0x00: 04 00 40 00 00 00 00 00 00 04 01 01 05 01 01 00
  +0x10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  +0x20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Instruction 3:
  +0x00: 08 00 40 00 00 00 00 00 01 06 01 04 00 01 00 00
  +0x10: 00 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00
  +0x20: 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Instruction 4:
  +0x00: 0C 00 40 00 00 00 00 00 00 07 01 06 08 01 01 00
  +0x10: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  +0x20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Instruction 5:
  +0x00: 10 00 40 00 00 00 00 00 03 00 00 00 00 00 00 01
  +0x10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  +0x20: 00 00 00 00 00 00 00 00 20 00 40 00 00 00 00 00
```

**Field layout within each 48-byte record:**

```text
Bytes 00–07  inst_addr (uint64_t)
Byte  08     op_type
Byte  09     dest
Byte  0A     dest_needed
Byte  0B     src1_reg
Byte  0C     src2_reg
Byte  0D     src1_needed
Byte  0E     src2_needed
Byte  0F     cc_read
Byte  10     cc_write
Bytes 11–17  alignment padding (seven bytes)
Bytes 18–1F  mem_addr (uint64_t)
Byte  20     mem_write
Byte  21     mem_read
Byte  22     br_dir
Bytes 23–27  alignment padding (five bytes)
Bytes 28–2F  br_target (uint64_t)
```

Offsets above are **hexadecimal**. The padding bytes in these sample records are all zero. All multibyte integers use little-endian byte order. For example, the `0x400000` address starts as `00 00 40 00 00 00 00 00` in the file.

## Test it in C++

```cpp
std::ifstream file("trace_first_run.bin", std::ios::binary);
Trace_Rec record;
while (file.read(reinterpret_cast<char*>(&record), sizeof(record))) {
    // inspect record.inst_addr, record.op_type, and the remaining fields
}
```

Check `sizeof(Trace_Rec) == 48` on the machine running the test. The `.bin.gz` file must be decompressed before you can read it with a normal `std::ifstream`.
