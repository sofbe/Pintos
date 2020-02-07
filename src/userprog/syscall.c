#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

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
    //char* filename = *(char**)(file->esp); //ÄR DET HÄR NAMNET?
    bool filePlaced = false;
    for(int i = 2; i < 130; i++ ){
        if(thread->fds[i] == NULL){ //måste göra init_thread någonstans!!
            if(filesys_open(file) != NULL ) {
                thread->fds[i] = filesys_open(file);
                filePlaced = true;
                fdsNumber = i;
                break;
            }
        }
    }
   // file_read_at(file); //används för att läsa in, Vill typ ha file->pos.
    if(filePlaced){
        return fdsNumber;
    }
    else{
        return -1;
    }
    //else it's full and file cannot be opened

}

void close(int fd){
    struct thread *thread = thread_current();
    struct file *file = thread->fds[fd];

    file_close(file);
    //printf("closed a file yay \n");
    thread->fds[fd] = NULL;
}

int read(int fd, void *buffer, unsigned size){
    struct thread *thread = thread_current();
    //ASSERT(fd > -1 && fd < 131);


    int bytes;

    if (fd == STDIN_FILENO){
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
       // printf("Writing to console\n");
        putbuf(buffer, size);
        return size;
    }
    else if(fd > STDOUT_FILENO){
        //printf("Writing to fd: %d\n", fd);
        bytes = file_write(thread->fds[fd], buffer, size);
        return bytes;
        }
    else{
       // printf("else %d\n");
        return -1;
    }
}

void exit(int status){
    thread_exit();

}

static void
syscall_handler (struct intr_frame *f UNUSED)
{

    int sys_nr = *((int*)f->esp);
    //printf ("system call: %d!\n", sys_nr);
   // int* esp = f->esp;

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
            (f->eax) = open(*((char**)f->esp+4));
            break;
        }
        case (SYS_EXIT): {
            exit(1);
            break;
        }
        case (SYS_CLOSE):{
            close(*((int*)(f->esp+4))); //hur kommer vi åt postionen i fds[]? open returnerar fds nummer... det läggs på stacken eax
            //kan vi hämta ut numret därifrån och skicka in det i close?b
            break;
        }
        case (SYS_WRITE): {
            (f->eax) = write(*((int*)(f->esp+4)), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
            break;
        }
        case (SYS_READ): {
            (f->eax) = read(*((int*)f->esp+4), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
            break;
        }
    }

    //thread_exit ();
}
