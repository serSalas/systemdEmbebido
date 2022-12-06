///\file lib_user.c
///\details Archivo con las funciones a usar en usuarios.c

#include "../inc/usuarios.h"

/// Calcula marca de tiempo en formato Y-m-d H:M:S
/// \return Devuelve como String
char *marca_tiempo() {

  char *marca;
  time_t time_stamp;
  struct tm *s_time_stamp;
  size_t tamTime;

  marca = calloc(sizeof(char), 18);
  time_stamp = time(NULL);

  if (time_stamp == ((time_t) -1)) {
    fprintf(stderr, "Error al usar time");
    exit(EXIT_FAILURE);
  }

  s_time_stamp = localtime(&time_stamp);
  if (s_time_stamp == 0) {
    fprintf(stderr, "Error al usar local time");
    exit(EXIT_FAILURE);
  }

  tamTime = strftime(marca, TAM_BUF, "%Y-%m-%d %H:%M:%S", s_time_stamp);
  if (tamTime == 0) {
    fprintf(stderr, "Error al usar strftime");
    exit(EXIT_FAILURE);
  }

  return marca;
}

/// Verifica que el JSON a probar sea válido y que el nombre asociado sea string
/// \param json_a_probar Valor a verificar
/// \param key nombre del JSON
/// \return NULL si no es válido, objeto JSON válido en caso de estar ok.
json_t *es_valido(json_t *json_a_probar, const char *key) {

  json_t *objeto_json;

  objeto_json = json_object_get(json_a_probar, key);
  if (objeto_json == NULL || json_is_string(objeto_json) == 0) {
    return NULL;
  }
  return objeto_json;

}

/// Crea nuevo usuario del sistema Operativo
/// \param n_user Usuario nuevo
/// \param n_pass Contraseña nueva
/// \return retorna 0 en caso de que exista el usuario, en caso de crearlo devuelve 1
int8_t nuevo_usuario(const char *n_user, const char *n_pass) {

  char directiva[512];
  struct passwd *linea;
  char *new_pass = calloc(1, sizeof(n_pass));

  linea = getpwent();

  while (linea != NULL) {

    if (!strcmp(linea->pw_name, n_user)) {
      return 0;
    }

    linea = getpwent();

  }

  endpwent();
  sprintf(new_pass, "\"%s\"", n_pass);
  sprintf(directiva, "sudo useradd -p $(openssl passwd -1 %s) %s", new_pass, n_user);
  system(directiva);
  return 1;
}


/// Funcion Callback en Web Service /api/users para listar usuarios
///\details https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas
#pragma GCC diagnostic ignored "-Wunused-parameter"
int callback_listar_usuarios(const struct _u_request *request, struct _u_response *response, void *user_data) {

  struct passwd *linea;
  json_t *usuarios;
  usuarios = json_array();
  linea = getpwent();

  while (linea != NULL) {

    json_t *usuario;

    usuario = json_pack("{s : i, s : s}", "user_id", linea->pw_uid, "username", linea->pw_name);

    json_array_append(usuarios, usuario);

    linea = getpwent();

  }

  y_log_message(Y_LOG_LEVEL_INFO,
                "Usuario listados: %i",
                json_array_size(usuarios));

  usuarios = json_pack("{s,o}", "data", usuarios);
  ulfius_set_json_body_response(response,
                                200,
                                usuarios);

  endpwent();
  return U_CALLBACK_CONTINUE;

}

/// Funcion callback en Web Service /api/users para agregar usuarios
int callback_agregar_usuario(const struct _u_request *request, struct _u_response *response, void *user_data) {

  json_t *json_newbie = ulfius_get_json_body_request(request, NULL), *json_body = NULL;
  json_t *json_user;
  json_t *json_pass;
  const char *user;
  const char *pass;
  struct passwd *linea;
  char ts[TAM_BUF];

  json_user = es_valido(json_newbie, USER);
  json_pass = es_valido(json_newbie, PASS);

  if (es_valido(json_newbie, USER) != NULL && es_valido(json_newbie, PASS) != NULL) {

    user = json_string_value(json_user);
    pass = json_string_value(json_pass);

    if (user == NULL || pass == NULL) {
      ulfius_set_string_body_response(response, 400, "Cast con error.");
      y_log_message(Y_LOG_LEVEL_INFO,
                    "peticion error 400");
      return U_CALLBACK_CONTINUE;

    } else {

      if (!nuevo_usuario(user, pass)) {

        json_t *body;

        body = json_pack("{s:s}", "description", "usuario duplicado");

        ulfius_set_json_body_response(response, 409, body);

        y_log_message(Y_LOG_LEVEL_INFO,
                      "Usuario duplicados - error 409");

        return U_CALLBACK_CONTINUE;
      }

    }

  } else {
    json_body = json_pack("{s:s}", "description", "Json no valido.");
    ulfius_set_json_body_response(response, 400, json_body);

    y_log_message(Y_LOG_LEVEL_INFO,
                  "Usuario error 404 - Json no valido");

    return U_CALLBACK_CONTINUE;

  }

  sprintf(ts, "%s", marca_tiempo());

  linea = getpwent();

  while (linea != NULL) {

    if (strcmp(linea->pw_name, user) != 0) {

      linea = getpwent();

    } else {

      break;

    }
  }

  json_body = json_pack("{s:i, s:s, s:s}", "id", linea->pw_uid, "username", user, "created_at", ts);

  y_log_message(Y_LOG_LEVEL_INFO,
                "Usuario %i creado",
                linea->pw_uid);

  endpwent();

  ulfius_set_json_body_response(response, 200, json_body);

  return U_CALLBACK_CONTINUE;

}