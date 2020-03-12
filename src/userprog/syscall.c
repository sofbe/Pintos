#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"
#include "lib/string.h"

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
    if(!valid_char(file)){
        exit(-1);
    }
    return filesys_create(file, initialsize);
}

int open(const char *file){

    if(!valid_char(file)){
        exit(-1);
    }
    int fdsNumber = 0;
    struct file *f = filesys_open(file);
    struct thread *thread = thread_current();
    bool filePlaced = false;
    for(int i = 2; i < 130; i++ ){
        if(thread->fds[i] == NULL){
            //struct file file = filesys_open(file);
            if(f != NULL ) {
                thread->fds[i] = f;
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
    if(fd > -1 && fd < 130) {
        struct thread *thread = thread_current();
        struct file *file = thread->fds[fd];

        file_close(file);
        thread->fds[fd] = NULL;
    }

}

int read(int fd, void *buffer, unsigned size){
    struct thread *thread = thread_current();
    if(!valid_buffer(buffer, size)) {
        exit(-1);
    }

    int bytes;

    if (fd == STDIN_FILENO) {
        for (unsigned i = 0; i < size; i++) {
            *((uint8_t *) buffer) = input_getc();
            buffer++;
        }
        return size;
    } else if (fd > STDOUT_FILENO && fd < 130) {
        struct file *file = thread->fds[fd];
        if (file != NULL) {
            bytes = file_read(file, buffer, size);
            return bytes;
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }

}

int write(int fd, const void *buffer, unsigned size){
    struct thread *thread = thread_current();
    if(!valid_buffer(buffer,size)){
        exit(-1);
    }

    if(fd > -1 && fd < 130) {
        int bytes;

        if (fd == STDOUT_FILENO) {
            putbuf(buffer, size);
            return size;
        } else if (fd > STDOUT_FILENO) {
            if (thread->fds[fd] != NULL) {
                bytes = file_write(thread->fds[fd], buffer, size);
                return bytes;
            }
        } else {
            return -1;
        }
    }
    else {
        return -1;
    }
}

void exit(int status){
    struct thread *thread = thread_current();
    thread->pChild->exit_status = status;
    printf("%s: exit(%d)\n", thread->name, thread->pChild->exit_status);

    thread_exit();

}

pid_t exec (const char *cmd_file) {
    if(!valid_char(cmd_file)){
        exit(-1);
    }

    const char *file_name = cmd_file;
    pid_t pid;
    pid = process_execute(file_name);
    return pid;

}

int wait(pid_t pid){
    return process_wait(pid);
}

void seek (int fd, unsigned position){
    if((fd > -1 && fd < 130)) {
        struct thread *thread = thread_current();
        struct file *f = thread->fds[fd];
        if(filesize(fd) < position){
            file_seek(f, filesize(fd));
        }
        else{
            file_seek(f, position);
        }

    }
}

unsigned tell (int fd){
    if(!(fd > -1 && fd < 130)) {
        return -1;
    }
    struct thread *thread = thread_current();
    struct file *f = thread->fds[fd];
    return file_tell(f);
}

int filesize (int fd){
    if(!(fd > -1 && fd < 130)) {
        return -1;
    }
    struct thread *thread = thread_current();
    struct file *f = thread->fds[fd];
    if(f != NULL) {
        return file_length(f);
    }
    else{
        return -1;
    }
}

bool remove (const char *file_name){
    if(!valid_char(file_name)){
        exit(-1);
    }
    return filesys_remove(file_name);
}

bool valid_pointer(void *pointer){
    struct thread *thread = thread_current();

    if(pointer != NULL ){
        if(is_user_vaddr(pointer)){
            if(pagedir_get_page(thread->pagedir, pointer) != NULL){
                return true;
        }
    }

    }
    return false;

}

bool valid_char(char *c){
    if(!valid_pointer(c)){
        return false;
    }
    while(*(c) != '\0'){
        if(!valid_pointer(c)){
            return false;
        }
        c++;
    }
    return true;
}

bool valid_buffer(void *buffer, unsigned size){
   if(!valid_pointer(buffer)){
        return false;
    }
    unsigned current = buffer;
    while(current < (buffer+size)){
        if(!valid_pointer(current)){
            return false;
        }
        current++;
    }
    return true;
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
    if(!valid_pointer(f->esp)){
        exit(-1);
    }
    int sys_nr = *((int*)f->esp);


    switch (sys_nr) {
            case (SYS_HALT): {
                halt();
                break;
            }
            case (SYS_CREATE): {
                if(!valid_pointer(f->esp+4) || !valid_pointer(f->esp+8)){
                    exit(-1);
                }
                (f->eax) = create(*((char**)(f->esp+4)), *((int*)(f->esp+8)));
                break;
            }
            case (SYS_OPEN): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = open(*((char**)(f->esp+4)));
                break;
            }
            case (SYS_EXIT): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                else{
                    exit(*((int*)(f->esp+4)));
                }
                break;
            }
            case (SYS_CLOSE):{
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                close(*((int*)(f->esp+4)));
                break;
            }
            case (SYS_WRITE): {
                if(!valid_pointer(f->esp+4) || !valid_pointer(f->esp+8) || !valid_pointer(f->esp+12)){
                    exit(-1);
                }
                (f->eax) = write(*((int*)(f->esp+4)), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
                break;
            }
            case (SYS_READ): {
                if(!valid_pointer(f->esp+4) || !valid_pointer(f->esp+8) || !valid_pointer(f->esp+12)){
                    exit(-1);
                }
                (f->eax) = read(*((int*)(f->esp+4)), *((void**)(f->esp+8)), *((unsigned*)(f->esp+12)));
                break;
            }
            case (SYS_EXEC): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = exec(*((char**)(f->esp+4)));
                break;
            }
            case (SYS_WAIT): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = wait(*((int*)(f->esp+4)));
                break;
            }
            case(SYS_SEEK): {
                if(!valid_pointer(f->esp+4) || !valid_pointer(f->esp+8)){
                    exit(-1);
                }
               seek(*((int*)(f->esp+4)),*((unsigned*)(f->esp+8)));
                break;
            }
            case(SYS_TELL): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = tell(*((int*)(f->esp+4)));
                break;
            }
            case(SYS_FILESIZE): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = filesize(*((int*)(f->esp+4)));
                break;
            }
            case(SYS_REMOVE): {
                if(!valid_pointer(f->esp+4)){
                    exit(-1);
                }
                (f->eax) = remove(*((char**)(f->esp+4)));
                break;
            }
        }
     //}

    //thread_exit ();
}

