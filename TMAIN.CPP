//---------------------------------------------------------------------------
//   TMAIN.CPP
//   
//   Borland C++ v2.00
//
//   "Dr.Dobb's journal"#148 February 1989., by Tom Green,
//   26 May 1993 
//
//
//---------------------------------------------------------------------------
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#include "task.hpp"
//---------------------------------------------------------------------------
void task0(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
//---------------------------------------------------------------------------
// возможен только один экземпляр класса task_control
task_control tasker;
// счетчики задач
unsigned long counter[5]={0L,0L,0L,0L};
// сигналы о передаче управления
signal sig2,sig3;
//---------------------------------------------------------------------------
void main(void){

   // определяем задачи
   task t0((far_func_ptr)task0,1024*4);
   task t1((far_func_ptr)task1,1024*1);
   task t2((far_func_ptr)task2,1024*1);
   task t3((far_func_ptr)task3,1024*1);
   task t4((far_func_ptr)task4,1024*4);

   // подключаем задачи к ядру
   tasker.add_new_task(&t0);
   tasker.add_new_task(&t1);
   tasker.add_new_task(&t2);
   tasker.add_new_task(&t3);
   tasker.add_new_task(&t4);

   clrscr();

   // начинаем многозадачную работу
   tasker.start_tasks();

}// main
//---------------------------------------------------------------------------
void task0(void){
   while(1){
      // печать состояния задач
      counter[3]++;
      // блокируем переключение во время вызова DOS
      tasker.lock();
      gotoxy(1,1); printf("Display: %ld",counter[3]);
      gotoxy(1,3); printf("Task 2 - %ld",counter[1]);
      gotoxy(1,4); printf("Task 3 - %ld",counter[2]);
      // разрешаем переключение задач
      tasker.unlock();
      // передать управление следующей задаче
      tasker.block();
   }// while
}// task0
//---------------------------------------------------------------------------
void task1(void){
   // задача сама печатает свое состояние
   while(1){
      tasker.lock();
      gotoxy(1,2); printf("Task 1 - %ld",counter[0]++);
      tasker.unlock();
      tasker.block();
   }// while
}// task1
//---------------------------------------------------------------------------
void task2(void){
   while(1){
      // ожидаем сигнала (передачи управления)
      tasker.wait(&sig2);
      // подсчет обработанных нажатий
      counter[1]++;
   }// while
}// task2
//---------------------------------------------------------------------------
void task3(void){
   while(1){
      tasker.lock();
      if(kbhit() != 0)
         // если нажата клавиша то послать сигнал (передать управление)
         // ожидающей задаче
         tasker.send(&sig3),
         // подсчет нажатых клавиш
         counter[2]++;
      tasker.unlock();
      tasker.block();
   }// while
}// task3
//---------------------------------------------------------------------------
void task4(void){

   char ch;
   static int  cx=1,cy=6;

   while(1){
      // ждать сигнал(клавиша нажата, из task3)
      tasker.wait(&sig3);
      tasker.lock();
      if((ch = getch()) == 27)
         // если нажата ESC то остановить многозадачность
         tasker.stop_tasks();
      // печать принятого кода клавиши
      gotoxy(cx,cy);
      cprintf("%c",ch);
      tasker.unlock();
      if(cx >= 80){
         cx=1;
         if(cy >= 25){
            tasker.lock();
            clrscr();
            tasker.unlock();
            cy=6;
         }// if
         else{
            cy++;
         }// elif
      }// if
      else{
         cx++;
      }// elif
      // передать сигнал об обработке нажатия
      tasker.send(&sig2);
   }// while
}// task4
//---------------------------------------------------------------------------
