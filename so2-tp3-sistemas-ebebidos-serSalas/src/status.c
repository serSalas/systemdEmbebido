///\file status.c
/// Función principal para el servicio de Estado del WS

#include <stdio.h>
#include <ulfius.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <string.h>
#include <sys/statvfs.h>
#include <yder.h>
#include <sys/utsname.h>
#include <time.h>

#include "../inc/status.h"


int main(void) {
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return (1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/api/servers/hardwareinfo", NULL, 0, &callback_hw_info, NULL);

  ulfius_set_default_endpoint(&instance, &callback_default, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    y_init_logs("api/servers/hardwareinfo",
                Y_LOG_MODE_FILE,
                Y_LOG_LEVEL_INFO,
                "/var/log/tp3/hw_info.log",
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