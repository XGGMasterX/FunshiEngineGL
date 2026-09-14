#ifndef PATHUTILS_H
#define PATHUTILS_H

// Separador de rutas dependiente de la plataforma, compartido por los
// modulos que construyen rutas de archivo a mano (explorador de archivos).
// Antes cada .cpp definia su propio PATH_SEP, y era facil divergir.
#ifdef _WIN32
inline constexpr char PATH_SEP = '\\';
#else
inline constexpr char PATH_SEP = '/';
#endif

#endif