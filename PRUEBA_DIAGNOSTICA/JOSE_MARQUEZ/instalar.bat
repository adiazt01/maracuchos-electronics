@echo off
chcp 65001 >nul
title Prueba Diagnostica - Verificacion de entorno
cd /d "%~dp0"

echo ============================================
echo  Prueba Diagnostica - Lenguajes y Compiladores
echo  Verificacion e instalacion de componentes
echo ============================================

echo.
echo [1/3] Python
python --version >nul 2>nul
if errorlevel 1 goto sin_python
python --version
if not exist requirements.txt goto gcc
echo   Instalando dependencias de requirements.txt...
python -m pip install --disable-pip-version-check -q -r requirements.txt
goto gcc

:sin_python
echo   Python no se encontro en el PATH.
where winget >nul 2>nul
if errorlevel 1 goto sin_winget
echo   Instalando Python 3.12 con winget...
winget install -e --id Python.Python.3.12
echo   Cierra esta ventana y vuelve a ejecutar el script para actualizar el PATH.
goto gcc

:sin_winget
echo   Descarga Python desde https://www.python.org/downloads/
echo   y marca la opcion "Add python.exe to PATH" al instalar.

:gcc
echo.
echo [2/3] Compilador de C (gcc)
gcc --version >nul 2>nul
if not errorlevel 1 goto gcc_ok
set "DEV1=%ProgramFiles(x86)%\Embarcadero\Dev-Cpp\TDM-GCC-64\bin"
set "DEV2=%ProgramFiles%\Embarcadero\Dev-Cpp\TDM-GCC-64\bin"
if exist "%DEV1%\gcc.exe" goto dev1
if exist "%DEV2%\gcc.exe" goto dev2
echo   gcc no se encontro. Instala Embarcadero Dev-C++ (incluye TDM-GCC)
echo   o MinGW-w64, y agrega su carpeta bin al PATH.
goto carpetas

:dev1
set "PATH=%DEV1%;%PATH%"
goto gcc_ok

:dev2
set "PATH=%DEV2%;%PATH%"

:gcc_ok
gcc --version | findstr /i "gcc"

:carpetas
echo.
echo [3/3] Estructura de carpetas segun pautas actualizadas
if not exist "ejercicio_1" mkdir "ejercicio_1"
if not exist "ejercicio_2\c" mkdir "ejercicio_2\c"
if not exist "ejercicio_2\python" mkdir "ejercicio_2\python"
if not exist "ejercicio_3" mkdir "ejercicio_3"
echo   Listo. Estructura creada correctamente.

echo.
pause
