//
// Created by skaliver on 14/9/20.
//

#ifndef TP3_SSALAS_34988049_INC_STATUS_H
#define TP3_SSALAS_34988049_INC_STATUS_H

#define PORT 9127
#define TAM_BUF 256

int callback_default(const struct _u_request *, struct _u_response *, void *);
char *kernel_version();
void cpu_info(char* []);
int callback_hw_info(const struct _u_request *, struct _u_response *, void *);

#include "../src/hw_info.c"

#endif //TP3_SSALAS_34988049_INC_STATUS_H
