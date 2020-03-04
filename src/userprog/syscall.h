#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>
#include <user/syscall.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

void syscall_init (void);
void halt(void);
bool create (const char*file, unsigned initialsize);
int open(const char *file);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void exit(int status);
pid_t exec (const char *cmd_file);
int wait (pid_t pid);
void seek (int fd, unsigned position);
unsigned tell (int fd);
int filesize (int fd);
bool remove (const char *file_name);
bool valid_pointer(void *pointer);
bool valid_char(char *c);
bool valid_buffer (void *buffer, unsigned size);

#endif /* userprog/syscall.h */
