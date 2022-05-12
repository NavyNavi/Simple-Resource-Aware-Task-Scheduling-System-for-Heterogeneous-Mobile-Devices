class StartNode : public Node {
public:
    TaskNode* getNextNode();
    void addNextNode(TaskNode* node);
    int setCriticality();
    int execute();
    void findCriticalNodes();
    void cleanup();
    std::list<TaskNode*> commit();
private:
    std::vector<TaskNode *> next_nodes;
    bool critical = true;
};