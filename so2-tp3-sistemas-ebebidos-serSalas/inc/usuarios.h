

#ifndef TP3_SSALAS_34988049_INC_USUARIOS_H
#define TP3_SSALAS_34988049_INC_USUARIOS_H

#define TAM_BUF 512
#define PORT 8178
#define USER "username"
#define PASS "password"

#include "../src/lib_user.c"

char *marca_tiempo();
json_t *es_valido(json_t *, const char *);
int8_t nuevo_usuario(const char *, const char *);
int callback_listar_usuarios(const struct _u_request *, struct _u_response *, void *);
int callback_agregar_usuario(const struct _u_request *, struct _u_response *, void *);
int callback_default(const struct _u_request *, struct _u_response *, void *);

#endif //TP3_SSALAS_34988049_INC_USUARIOS_H
