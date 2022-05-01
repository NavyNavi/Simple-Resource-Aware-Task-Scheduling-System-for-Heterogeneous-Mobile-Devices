class Scheduler {
    public:
        ~Scheduler();
        TaskNode* addNode(int funcId, TaskNode* prev_node = nullptr);
        void commit();
        void assign();

    private:
        int currCrit;
        long idCounter = 1;
        StartNode* start_node = new StartNode();
};