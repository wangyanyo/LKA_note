
struct sched_entity {
        struct rb_node run_node;

        u64 vruntime;
};

struct task_struct {
        struct sched_entity se;
};