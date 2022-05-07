#ifndef TASKNODE_INCLUDED
#define TASKNODE_INCLUDED

class TaskNode : public Node {
public:
    int dependency = 1;

    TaskNode(int funcId, int args, long id);
    TaskNode(TaskNode const &node);

    TaskNode* getNextNode();
    void addNextNode(TaskNode* node);
    int execute();
    int setCriticality();
    void cleanup();
    std::list<TaskNode*> commit();
    std::string serialize();

private:
    std::vector<TaskNode *> next_nodes;
    int funcId;
    int args;
};

#endif