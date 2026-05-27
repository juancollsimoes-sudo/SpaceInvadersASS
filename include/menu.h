#ifndef MENU_H
#define MENU_H

#include <stdint.h>

/**
 * @brief Ejecuta el menú principal (pantalla de inicio) y bloquea hasta que 
 * el usuario decida iniciar el juego o salir.
 * 
 * @return 0 si se debe iniciar el juego, -1 si el usuario seleccionó salir.
 */
int32_t ejecutar_menu(void);

#endif // MENU_H
