class Scheduler {
    public:
        typedef int (*callback_function)(void);

        ~Scheduler();
        TaskNode* addNode(callback_function func, TaskNode* prev_node = nullptr);
        void commit();
        void assign();

    private:
        int currCrit;
        long idCounter = 1;
        StartNode* start_node = new StartNode();
};