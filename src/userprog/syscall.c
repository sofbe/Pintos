#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <file.h>
#include <filesys.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"


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
    int sys_nr = *(int*)(f->esp);
    switch (sys_nr)
        case (SYS_HALT) {
                    halt();
                    break;
            }
        case (SYS_CREATE) {
                    f->eax = create(.....)
            }
        case (SYS_EXIT){

    }



  thread_exit ();
}

void halt(void) {
    power_off();
}

bool create (const char*file, unsigned initialsize){
    file = file->esp+4;
    initialsize = initialsize->esp+8;
    return filesys_create(file, initialsize);
}

int open(const char *file){
    int index = thread_current()->position; //om vi vill komma åt fil 7 ska vi skicka in 7 här på något sätt för att komma åt plats 7 i arrayen *fds.
    file_read_at(file); //används för att läsa in, Vill typ ha file->pos.
    filesys_open(file);
}

void close (int fd){

}

int read(int fd, void *buffer, unsigned size){

}

int write(int fd, const void *buffer, unsigned size){

}

void exit (int status){

}