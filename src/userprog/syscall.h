#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <stdbool.h>
#include <user/syscall.h>

void syscall_init (void);
void halt(void);
bool create (const char*file, unsigned initialsize);
int open(const char *file);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void exit(int status);
pid_t exec (const char *cmd_file);

#endif /* userprog/syscall.h */
