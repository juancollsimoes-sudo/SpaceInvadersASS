#ifndef INPUT_TIME_H
#define INPUT_TIME_H

#include <stdint.h>

// Máscara de bits para el estado compacto de las teclas.
// Facilita la lectura en ASM: una sola instrucción 'test eax, KEY_LEFT' para verificar la entrada.
#define KEY_LEFT   (1 << 0)   // Bit 0: Izquierda (Flecha Izquierda o tecla 'A')
#define KEY_RIGHT  (1 << 1)   // Bit 1: Derecha (Flecha Derecha o tecla 'D')
#define KEY_SPACE  (1 << 2)   // Bit 2: Disparo (Barra Espaciadora)
#define KEY_QUIT   (1 << 3)   // Bit 3: Salir o Cerrar Ventana (Tecla 'Esc')

/**
 * @brief Captura de forma no bloqueante (Non-blocking input) el mapa de estado del teclado.
 * Compara las teclas de interés y empaqueta el resultado en una máscara de bits de 32 bits.
 * @return Entero de 32 bits (retornado en EAX) con los bits correspondientes activos.
 */
int32_t verificar_teclado(void);

/**
 * @brief Regulador dinámico de fotogramas por segundo (Frame Rate Regulator).
 * Utiliza temporizadores del kernel de alta resolución de POSIX para calcular
 * el tiempo que demoró en ejecutarse el frame actual, durmiendo (sleep) de forma pasiva
 * el resto de tiempo exacto para lograr los FPS objetivo.
 * @param fps Tasa de FPS deseada (ej. 60). Se pasa en el registro RDI.
 */
void regular_framerate(int32_t fps);

#endif // INPUT_TIME_H
