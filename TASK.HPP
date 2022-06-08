//--------------------------------------------------------------------------
//
//   TASK.HPP
//   
//   Borland C++ v2.00
//
//   "Dr.Dobb's journal"#148 February 1989., by Tom Green,
//   26 May 1993 
//
//
//---------------------------------------------------------------------------
#ifndef _TASK_
#define _TASK_
#ifdef __BCPLUSPLUS__
#   ifndef __LARGE__
#      error Build TASK to Large memory model
#   endif
#endif
//---------------------------------------------------------------------------
typedef void (far *far_func_ptr)(void);
//---------------------------------------------------------------------------
typedef struct task_image{
   unsigned int bp;
   unsigned int di;
   unsigned int si;
   unsigned int ds;
   unsigned int es;
   unsigned int dx;
   unsigned int cx;
   unsigned int bx;
   unsigned int ax;
   unsigned int ip;
   unsigned int cs;
   unsigned int flags;
}task_image;
//---------------------------------------------------------------------------
class task{

   private:
      friend class task_control;
      friend class signal;
      task_image *stack_ptr;
      unsigned char task_state;
      unsigned char *workspace;
      task *next_task;
   public:
      task(far_func_ptr func,unsigned int workspace_size);// constructor
      ~task();// destructor

};//class task
//---------------------------------------------------------------------------
class signal{

   private:
      friend class task_control;
      task *head;
      task *tail;
      task *get_task_q(void);
      void put_task_q(task *tptr);
   public:
      signal(void){head=tail=0;};//constructor

};//class signal
//---------------------------------------------------------------------------
class task_control{

   private:
      signal ready_q;
      task *current_task;
      task_image far *old_stack_ptr;
      unsigned int  task_running;
      unsigned long timer_ticks;
      unsigned int  task_lock;
   public:
      task_control(void);//constructor
      task_image far *task_switch(task_image far *stk_ptr,
                                  unsigned int flag,
                                  signal *sig);
      void add_new_task(task *new_task);
      void start_tasks(void);
      void stop_tasks(void);
      unsigned long get_timer_ticks(void){return (timer_ticks);};
      void lock(void){task_lock=1;};
      void unlock(void){task_lock=0;};
      void send(signal *sig);
      void wait(signal *sig);
      void block(void);

};//class task_control
//---------------------------------------------------------------------------
#endif /* ifndef _TASK_ */