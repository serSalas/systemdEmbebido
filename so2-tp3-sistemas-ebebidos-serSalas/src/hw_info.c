///\file hw_info.c
///\details Contiene las funciones implementadoas en status.c

#include "../inc/status.h"

/// Calcula marca de tiempo en formato Y-m-d H:M:S
/// \return Devuelve como String
char *marca_tiempo() {

  char *marca;
  time_t time_stamp;
  struct tm *s_time_stamp;
  size_t tamTime;

  marca = calloc(sizeof(char), 18);

  time_stamp = time(NULL);
  if (time_stamp == ((time_t) - 1)) {
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

/// Devuelve la versión de Kernel
/// \return Devuelve como String
char *kernel_version() {
  char *version;
  struct utsname unameData;

  if (uname(&unameData) == -1) {
    fprintf(stderr, "Error al usar uname");
    exit(EXIT_FAILURE);
  }

  version = calloc(1, sizeof(unameData.release));

  sprintf(version, "%s", unameData.release);

  return version;

}

/// Devuelve nombre de CPU y n de cores de /proc/cpuinfo
/// \param array_cpu arreglo que almacena informacion requerida de CPU
/// \return Devuelve como string
void cpu_info(char *array_cpu[]) {

  char *token_name;
  char *token_cant;
  int32_t contador;

  char *linea = calloc(64, sizeof(char));
  char *linea2 = calloc(64, sizeof(char));

  FILE *output = fopen("/proc/cpuinfo", "r");

  if (output == NULL) {
    fprintf(stderr, "fopen error");
    exit(EXIT_FAILURE);
  }

  linea = fgets(linea, 64, output);

  contador = 1;
  while (contador < 5) {

    contador = contador + 1;
    linea = fgets(linea, 64, output);

  }

  token_name = strtok(linea, ":");
  token_name = strtok(NULL, "\n");

  array_cpu[0] = token_name;

  while (contador < 13) {

    contador = contador + 1;
    linea2 = fgets(linea2, 64, output);

  }

  token_cant = strtok(linea2, ":");
  token_cant = strtok(NULL, "\n");

  array_cpu[1] = token_cant;

  fclose(output);

}


///\brief Función Callback para la Web App /api/servers/hardwareinfo
///\details https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html#Diagnostic-Pragmas
#pragma GCC diagnostic ignored "-Wunused-parameter"
int callback_hw_info(const struct _u_request *request, struct _u_response *response, void *user_data) {

  char *version_kernel = kernel_version();
  char *info_cpu[2];

  info_cpu[0] = calloc(20, sizeof(char));
  info_cpu[1] = calloc(20, sizeof(char));
  char *total_ram = calloc(20, sizeof(char));
  char *free_ram = calloc(20, sizeof(char));
  char *uptime = calloc(20, sizeof(char));
  char *load_avg_5_min = calloc(20, sizeof(char));
  char *disk_total = calloc(20, sizeof(char));
  char *disk_free = calloc(20, sizeof(char));

  cpu_info(info_cpu);

  struct sysinfo sys_info;
  struct statvfs buffer;

  if (sysinfo(&sys_info) == -1) {
    fprintf(stderr, "sysinfo error");
    exit(EXIT_FAILURE);
  }
  int ret = statvfs("/", &buffer);
  if (ret) {
    fprintf(stderr, "statvfs error");
    exit(EXIT_FAILURE);
  }

  const double total = (double) (buffer.f_blocks * buffer.f_frsize) / 1024 / 1024 / 1024;
  const double disponible = (double) (buffer.f_bfree * buffer.f_frsize) / 1024 / 1024 / 1024;

  sprintf(total_ram, "%lu [MB]", sys_info.totalram / 1024 / 1024);
  sprintf(free_ram, "%lu [MB]", sys_info.freeram / 1024 / 1024);
  sprintf(uptime, "%lu [s]", sys_info.uptime);
  sprintf(load_avg_5_min, "%lu", sys_info.loads[1]);
  sprintf(disk_total, "%.2f [GB]", total);
  sprintf(disk_free, "%.2f [GB]", disponible);

  json_t *body = json_pack("{s:s, s:s, s:s, s:s, s:s, s:s, s:s, s:s, s:s}",
                           "kernelVersion", version_kernel,
                           "processorName", info_cpu[0],
                           "totalCPUCore", info_cpu[1],
                           "totalMemory", total_ram,
                           "freeMemory", free_ram,
                           "diskTotal", disk_total,
                           "diskFree", disk_free,
                           "LoadAvg", load_avg_5_min,
                           "uptime", uptime
  );

  ulfius_set_json_body_response(response, 200, body);

  y_log_message(Y_LOG_LEVEL_INFO,
                "Estadisticas requeridas desde el %s",
                u_map_get(request->map_header, "X-Real-IP"));

  return U_CALLBACK_CONTINUE;
}