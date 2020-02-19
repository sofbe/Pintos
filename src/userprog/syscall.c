#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"

#define MAX_SIZE 130

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void halt(void) {
    power_off();
}

bool create (const char*file, unsigned initialsize){
    return filesys_create(file, initialsize);
}

int open(const char *file){
    int fdsNumber = 0;
    struct thread *thread = thread_current();
    bool filePlaced = false;
    for(int i = 2; i < 130; i++ ){
        if(thread->fds[i] == NULL){
            if(filesys_open(file) != NULL ) {
                thread->fds[i] = filesys_open(file);
                filePlaced = true;
                fdsNumber = i;
                break;
            }
        }
    }
    if(filePlaced){
        return fdsNumber;
    }
    else{
        return -1;
    }
}

void close(int fd){
    struct thread *thread = thread_current();
    struct file *file = thread->fds[fd];

    file_close(file);
    thread->fds[fd] = NULL;
}

int read(int fd, void *buffer, unsigned size){
    struct thread *thread = thread_current();

    int bytes;

    if (fd == STDIN_FILENO){
        printf("entered first if \n");
        for(unsigned i = 0; i < size; i++) {
            *((uint8_t*)buffer) = input_getc();
            buffer++;
        }
        return size;
    }
    else if(fd > STDOUT_FILENO && fd < 131){
        struct file *file = thread->fds[fd];
        if(file != NULL) {
            bytes = file_read(file, buffer, size);
            return bytes;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }

}

int write(int fd, const void *buffer, unsigned size){
    struct thread *thread = thread_current();

   ASSERT(fd > -1 && fd < 131);

    int bytes;
    if (fd == STDOUT_FILENO){
        putbuf(buffer, size);
        return size;
    }
    else if(fd > STDOUT_FILENO){
        bytes = file_write(thread->fds[fd], buffer, size);
        return bytes;
        }
    else{
        return -1;
    }
}

void exit(int status){
    struct thread *thread = thread_current();
    printf("%s: exit(%d)\n", thread->name, status);

    thread->pChild->exit_status = status;
    thread_exit();

}

pid_t exec (const char *cmd_file) {
    const char *file_name = cmd_file;
    pid_t pid;
    pid = process_execute(file_name);
    return pid;

}

static void
syscall_handler (struct intr_frame *f UNUSED)
{

    int sys_nr = *((int*)f->esp);

    switch (sys_nr) {
        case (SYS_HALT): {
            halt();
            break;
        }
        case (SYS_CREATE): {
            (f->eax) = create(*((char**)(f->esp+4)), *((int*)(f->esp+8)));
            break;
        }
        case (SYS_OPEN): {
            (f->eax) = open(*((char**)(f->esp+4)));
            break;
        }
        case (SYS_EXIT): {
            exit(*((int*)(f->esp+4)));
            break;
        }
        case (SYS_CLOSE):{
            close(*((int*)(f->esp+4)));
            break;
        }
        case (SYS_WRITE): {
            (f->eax) = write(*((int*)(f->esp+4)), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
            break;
        }
        case (SYS_READ): {
            (f->eax) = read(*((int*)(f->esp+4)), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
            break;
        }
        case (SYS_EXEC): {
            (f->eax) = exec(*((char**)(f->esp+4)));
            break;
        }
    }

    //thread_exit ();
}
