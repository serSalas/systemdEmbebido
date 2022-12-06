///\file usuarios.c
///\details Función principal para el manejo del Servicio de Usuarios

#include <stdio.h>
#include <ulfius.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <yder.h>
#include <string.h>

#include "../inc/usuarios.h"

/**
 * main function
 */
int main(void) {
  struct _u_instance instance;

  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return (1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "", NULL, 0, &callback_listar_usuarios, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", "", NULL, 0, &callback_agregar_usuario, NULL);

  ulfius_set_default_endpoint(&instance, &callback_default, NULL);
  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    y_init_logs("api/users",
                Y_LOG_MODE_FILE,
                Y_LOG_LEVEL_INFO,
                "/var/log/tp3/users.log",
                "Inicializando servicio...");
    pause();

  } else {
    fprintf(stderr, "Error starting framework\n");
  }

  printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
}

/**
 * Default callback function called if no endpoint has a match
 */
int callback_default(const struct _u_request *request, struct _u_response *response, void *user_data) {
  ulfius_set_string_body_response(response, 404, "Pagina no encontrada");
  return U_CALLBACK_CONTINUE;
}