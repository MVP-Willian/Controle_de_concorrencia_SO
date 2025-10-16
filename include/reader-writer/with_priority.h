//
// Created by luizg on 09/10/2025.
//

#ifndef WITH_PRIORITY_H
#define WITH_PRIORITY_H

void with_priority_initialize();

void with_priority_destroy();

void* reader_with_priority(void* arg);

void* writer_with_priority(void* arg);

#endif //WITH_PRIORITY_H