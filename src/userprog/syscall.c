#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    printf ("system call!\n");
    printf("1 - Halt\n");
    printf("2 - \n");
    printf("3 - \n");
    printf("4 - \n");
    scanf("%d", &opt);
    switch (opt)
        case 1: halt();
        case 2:



  thread_exit ();
}

void halt(void) {
    power_off();
}

bool create (const char*file, unsigned initialsize){

}

int open(const char *file){

}
void close (int fd){

}

int read(int fd, void *buffer, unsigned size){

}

int write(int fd, const void *buffer, unsigned size){

}

void exit (int status){

}