class StartNode : public Node {
public:
    TaskNode* getNextNode();
    void addNextNode(TaskNode* node);
    int setCriticality();
    int execute();
    void cleanup();
private:
    std::vector<TaskNode *> next_nodes;
};