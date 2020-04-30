# Informe TP1

## Detalles administrativos

* **Materia:** Taller de Programación I
* **Año y cuatrimestre:** 1°C 2020
* **Alumno:** PARAFATI, Mauro
* **Padrón:** 102749
* [**Link al repositorio**](https://github.com/mauro7x/taller_tp1)

<!-- ##################################################################### -->

# Tabla de contenidos

1. [Presentación del trabajo](#presentacion)
    * [Introducción](#introduccion)
    * [Descripción](#descripcion)
2. [Especificaciones provistas](#especificaciones)
    * [Cliente](#cliente)
    * [Servidor](#servidor)
    * [Protocolo D-BUS](#protocolo)
3. [Resolución](#resolucion)



<!-- ##################################################################### -->

# Presentación del trabajo <a name="presentacion"></a>

## Introducción <a name="introduccion"></a>

> El siguiente ejercicio consiste en una implementación parcial del protocolo utilizado por el servicio **D-Bus**. Se implementará la parte utilizada para realizar llamadas a **funciones remotas**, y la misma será implementada sobre el protocolo **TCP**.

## Descripción <a name="descripcion"></a>

> **D-BUS**​ es un sistema de comunicación entre procesos y llamadas a procedimientos remotos, utilizado principalmente por sistemas Unix. El protocolo que utiliza es parte del proyecto *freedesktop.org*.
>
> El sistema D-Bus posee un servicio corriendo en segundo plano con 2 canales (buses) para comunicarse con el resto de las aplicaciones. Estos canales, llamados **system bus**​ y **session bus​**, están implementados sobre sockets de tipo *UNIX*.
>
> En el siguiente trabajo práctico se deberá implementar una **aplicación cliente** y una **servidor** que se comuniquen sobre un **socket TCP** utilizando el mismo **protocolo** que D-BUS.

# Especificaciones provistas <a name="especificaciones"></a>

## Cliente <a name="cliente"></a>

> El cliente debe realizar 2 acciones:
> - Leer desde la entrada de datos las llamadas a procedimientos remotos,
> - Enviar el pedido al servidor con el protocolo elegido.
>
> El cliente debe conectarse a un **host** y un **puerto**, utilizando el protocolo **TCP**. La entrada de datos puede ser un archivo cuya ruta es pasada en los argumentos del programa, o la entrada standard, si no se le pasó
ningún argumento.
>
> Una vez **conectado**, el cliente **procesa la entrada** de datos leyendo linea por linea, envía la llamada a procedimiento remoto, y espera por la respuesta del servidor.

### Formato de línea de comando

> El **cliente** debe ser invocado con el **siguiente parámetro**:
>
> `./client <host> <puerto> [<archivo de entrada>]`
>
> Donde `archivo de entrada` es el nombre de un archivo de texto con las llamadas a realizar. Si este último parámetro no es incluido, se interpretará la **entrada estándar**.

### Formato de los archivos de entrada

> El **archivo de entrada** es un archivo de texto con el siguiente **formato**:
>
> `<destino> <path> <interfaz> <metodo>([<parametro1>,<parametroN>...])`
> 
> Todas las llamadas **terminan con el caracter de fin de línea**, se puede asumir que **las lineas están bien formadas**.

### Formato de los archivos de salida

> El cliente debe **imprimir** por cada procedimiento llamado **una linea con el id** seguido de la **respuesta del servidor**:
>
> `<id en hexadecimal, 8 dígitos, con prefijo "0x">: <respuesta del servidor>`
>
> El **"OK"** ​**DEBE**​ provenir del servidor, no se acepta que sea el cliente el que genera esta respuesta.


## Servidor <a name="servidor"></a>

> El servidor debe **esperar la conexión** de un cliente, luego **escuchar un petitorio** y **escuchar los mensajes** del cliente.
>
> Por cada mensaje recibido se debe **imprimir en pantalla** la información que se detalla en la sección `Formato de los archivos de salida`. Luego **devuelve al cliente** el texto `"OK"` *(3 bytes, agregar el '\n' al final)*.

### Formato de línea de comando

> El **servidor** debe ser invocado con el **siguiente parámetro**:
>
> `./server <puerto>`

### Formato de los archivos de entrada

> El servidor **no posee archivos de entrada**.

### Formato de los archivos de salida

> El servidor deberá **imprimir** por cada llamada a procedimiento remoto **las siguientes lineas**:
>
> ```
> * Id: <id en hexadecimal, 8 dígitos, con prefijo "0x">
> * Destino: <destino>
> * Path: <path>
> * Interfaz: <interfaz>
> * Método: <método>
> ```
> Si el método posee **parámetros**, se agregan las siguientes lineas:
> ```
> Parámetros:
>     <parámetro1>
>     <parámetroN>
> ```
> Por último, **una linea en blanco** después de los parámetros o el método si estos no están presentes.

## Protocolo D-BUS <a name="protocolo"></a>

# Resolución <a name="resolucion"></a>


















