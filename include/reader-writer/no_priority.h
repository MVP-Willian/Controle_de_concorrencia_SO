//
// Created by luizg on 09/10/2025.
//

#ifndef NO_PRIORITY_H
#define NO_PRIORITY_H

void no_priority_initialize();

void no_priority_destroy();

void* reader_no_priority(void* arg);

void* writer_no_priority(void* arg);

#endif //NO_PRIORITY_H