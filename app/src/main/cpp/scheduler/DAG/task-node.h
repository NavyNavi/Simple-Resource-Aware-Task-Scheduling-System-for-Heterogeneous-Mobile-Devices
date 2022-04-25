#ifndef TASKNODE_INCLUDED
#define TASKNODE_INCLUDED

class TaskNode : public Node {
public:
    typedef int (*callback_function)(int);
    TaskNode(callback_function f, int args, long id);
    TaskNode(TaskNode const &node);

    TaskNode* getNextNode();
    void addNextNode(TaskNode* node);
    int execute();
    int setCriticality();
    void cleanup();

private:
    std::vector<TaskNode *> next_nodes;
    callback_function func;
    int args;
};

#endif