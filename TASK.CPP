//--------------------------------------------------------------------------
//
//   TASK.CPP
//   
//   Borland C++ v2.00
//
//   "Dr.Dobb's journal"#148 February 1989., by Tom Green,
//   26 May 1993 
//
//
//--------------------------------------------------------------------------
#include "task.hpp"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
//---------------------------------------------------------------------------
#define TASK_INACTIVE   0
#define TASK_ACTIVE     1
#define TASK_READY      2
#define TASK_WAITING    3
#define TASK_ERROR      0xFF
//---------------------------------------------------------------------------
#define TASK_TIMER_INTR 0
#define TASK_SEND       1
#define TASK_WAIT       2
#define TASK_BLOCK      3
//---------------------------------------------------------------------------
#define TIMER_INT       0x08
//---------------------------------------------------------------------------
void save_image(unsigned int flag,signal *sig);
//---------------------------------------------------------------------------
void interrupt timer_handler(...);
void interrupt (*old_timer_handler)(...);
//---------------------------------------------------------------------------
task_control *gl_tptr;
//---------------------------------------------------------------------------
task::task(far_func_ptr func,unsigned int  workspace_size){// constructor

   task_image *ptr;

   if((workspace = (unsigned char *)malloc(workspace_size)) == NULL){
      task_state = TASK_ERROR;
      return;
   }//if

   ptr = (task_image*)(workspace + workspace_size - sizeof(task_image));

   stack_ptr = ptr;

   ptr->cs = FP_SEG(func);
   ptr->ip = FP_OFF(func);
   ptr->ds = _DS;
   ptr->es = _ES;
   ptr->flags = 0x200;
   task_state = TASK_INACTIVE;
   next_task  = 0;

}//task::task
//---------------------------------------------------------------------------
task::~task(void){// destrustor
   free(workspace);
}//task::~task
//---------------------------------------------------------------------------
task *signal::get_task_q(void){

   task *temp;

   temp = head;
   if(head)
      head = head->next_task;
   return (temp);

}//task *signal::get_task_q
//---------------------------------------------------------------------------
void signal::put_task_q(task *tptr){

   if(head)
      tail->next_task = tptr;
   else
      head = tptr;
   tail = tptr;
   tptr->next_task = 0;

}//signal::put_task_q
//---------------------------------------------------------------------------
task_control::task_control(void){

   gl_tptr      = this;
   task_running = 0;
   current_task = 0;
   timer_ticks  = 0L;
   task_lock    = 0;

}//task_control::task_control
//---------------------------------------------------------------------------
void task_control::add_new_task(task *new_task){

   if(new_task->task_state != TASK_ERROR){
      new_task->task_state = TASK_READY;
      ready_q.put_task_q(new_task);
   }//if

}//task_control::add_new_task
//---------------------------------------------------------------------------
void task_control::start_tasks(void){

   task_running = 1;

   disable();
   old_timer_handler = getvect(TIMER_INT);
   setvect(TIMER_INT,timer_handler);
   enable();

   while(task_running);

}//task_control::start_tasks
//---------------------------------------------------------------------------
void task_control::stop_tasks(void){
   task_running = 0;
}//task_control::stop_tasks
//---------------------------------------------------------------------------
void task_control::send(signal *sig){
   save_image(TASK_SEND,sig);
}// task_control::send
//---------------------------------------------------------------------------
void task_control::wait(signal *sig){
   save_image(TASK_WAIT,sig);
}// task_control::wait
//---------------------------------------------------------------------------
void task_control::block(void){
   save_image(TASK_BLOCK,(signal*)0);
}// task_control::block
//---------------------------------------------------------------------------
task_image far *task_control::task_switch(task_image far *stk_ptr,
                                          unsigned int   flag,
                                          signal         *sig){

   task_image far *temp;
   task *tptr;

   if(flag == TASK_TIMER_INTR)
      timer_ticks++;

   if(!current_task){
      old_stack_ptr = stk_ptr;
      current_task = ready_q.get_task_q();
   }//if

   if(current_task->task_state == TASK_ACTIVE){
      current_task->stack_ptr = stk_ptr;
      current_task->task_state = TASK_READY;

      if(!task_lock || flag != TASK_TIMER_INTR){
         switch(flag){
            case TASK_WAIT:
               current_task->task_state = TASK_WAITING;
               sig->put_task_q(current_task);
               break;
            case TASK_SEND:
               if((tptr = sig->get_task_q()) != 0)
                  ready_q.put_task_q(tptr);
               /* !break; */
            case TASK_BLOCK:
            case TASK_TIMER_INTR:
               current_task->task_state = TASK_READY;
               ready_q.put_task_q(current_task);
               break;
         }// switch flag
         current_task = ready_q.get_task_q();
      }//if !task_lock
   }//if TASK ACTIVE

   if(task_running){
      current_task->task_state = TASK_ACTIVE;
      temp = current_task->stack_ptr;
   }//if task running
   else{
      disable();
      setvect(TIMER_INT,old_timer_handler);
      temp = old_stack_ptr;
      enable();
   }//elif task !running

   return (temp);

}// task_control::task_switch
//---------------------------------------------------------------------------
