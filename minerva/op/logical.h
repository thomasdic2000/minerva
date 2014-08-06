#pragma once

#include <vector>

#include "common/scale.h"
#include "common/nvector.h"
#include "chunk/chunk.h"
#include "op.h"
#include "context.h"

namespace minerva {

struct LogicalData;
struct LogicalOp;
class LogicalDataFn;
class LogicalComputeFn;

class LogicalDataGenFn : public BasicFn {
 public:
   virtual NVector<Chunk> Expand(const NVector<Scale>& part_sizes) = 0;
   virtual ~LogicalDataGenFn() {}
};

class LogicalComputeFn : public BasicFn {
 public:
  virtual std::vector<NVector<Chunk>> Expand(std::vector<NVector<Chunk>> inputs) = 0;
  virtual ~LogicalComputeFn() {}
};

struct LogicalData {
  Scale size;
  LogicalDataGenFn* data_gen_fn;
  NVector<PartInfo> partitions;
  int extern_rc;
  LogicalData(): data_gen_fn(NULL), extern_rc(0) {}
  LogicalData(const Scale& s, LogicalDataGenFn* fn = NULL):
    size(s), data_gen_fn(fn), extern_rc(0) {}
  //DataNodeContext context; // TODO how to set context ?
};

struct LogicalOp {
  LogicalComputeFn* compute_fn;
};

////////////////////////// Helper functions for logical op
template<int num_outputs, int num_inputs>
class LogicalComputeFnTemp : public LogicalComputeFn {
};

template<>
class LogicalComputeFnTemp<1, 1> : public LogicalComputeFn {
 public:
  std::vector<NVector<Chunk>> Expand(std::vector<NVector<Chunk>> inputs) {
    return {ExpandReal(inputs[0])};
  }
  virtual NVector<Chunk> ExpandReal(NVector<Chunk> in) = 0;
};

template<>
class LogicalComputeFnTemp<1, 2> : public LogicalComputeFn {
 public:
  std::vector<NVector<Chunk>> Expand(std::vector<NVector<Chunk>> inputs) {
    return {ExpandReal(inputs[0], inputs[1])};
  }
  virtual NVector<Chunk> ExpandReal(NVector<Chunk> in1, NVector<Chunk> in2) = 0;
};

}// end of namespace minerva
