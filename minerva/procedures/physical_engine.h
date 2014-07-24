#pragma once
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <condition_variable>
#include <thread>
#include <mutex>

#include "procedures/dag_procedure.h"
#include "procedures/thread_pool.h"
#include "common/common.h"

namespace minerva {

/*
 * PhysicalEngine makes following assumptions:
 * 1. Each PhysicalDataNode has one or zero predecessor.
 * 2. User is only allowed to add new nodes to the dag.
 * 3. PhysicalEngine is responsible for GC.
 */

struct NodeState {
  enum State {
    kNoNeed, // No need to execute
    kReady, // Need to execute
    kComplete
  } state;
  size_t dependency_counter;
  std::condition_variable* on_complete;
};

class PhysicalEngine: public PhysicalDagProcedure {
  friend class ThreadPool;

 public:
  typedef DagNode* Task;
  typedef std::function<void(Task)> Callback;
  typedef std::pair<Task, Callback> TaskPair;
  // TODO use reference to reduce overhead
  PhysicalEngine();
  ~PhysicalEngine();
  void Process(PhysicalDag&, std::vector<uint64_t>&);

 private:
  DISALLOW_COPY_AND_ASSIGN(PhysicalEngine);
  void Init();
  void CommitDagChanges();
  std::unordered_set<DagNode*> FindRootNodes(const std::vector<uint64_t>&);
  void NodeRunner(DagNode*);
  void AppendTask(Task, Callback);
  bool GetNewTask(std::thread::id, TaskPair&);

  std::unordered_map<uint64_t, NodeState> node_states_;
  std::mutex node_states_mutex_;
  ConcurrentBlockingQueue<TaskPair> task_queue_;
  ThreadPool thread_pool_;
};

}

