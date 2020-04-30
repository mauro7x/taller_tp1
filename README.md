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
    * [Penalizaciones](#penalizaciones)
    * [Restricciones](#restricciones)
3. [Resolución](#resolucion)



<!-- ##################################################################### -->

<hr>

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

> El **protocolo D-Bus** es parte en texto plano (autenticación) y parte en binario (mensajes). En este trabajo, nos centraremos en los **mensajes de llamadas a procedimientos remotos** únicamente.
>
> Un **mensaje** consiste de una **cabecera** (header) y **cuerpo** (body). La cabecera es un **conjunto de valores binarios con un formato fijo**. En el header se incluyen varios parámetros que definen el mensaje a enviar. En
el **body** se envían los **argumentos de los métodos** que ejecutamos.

### Formato del header

> El formato de la cabecera es:
>
> `BYTE, BYTE, BYTE, BYTE, UINT32, UINT32, ARRAY of STRUCT of (BYTE,VARIANT)`
>
> La **longitud de la cabecera debe ser múltiplo de 8**, en caso de no serlo, se rellenan los bytes faltantes con **ceros**. El cuerpo no posee esta restricción. Los parámetros ubicados en el array de parámetros también tienen que tener longitud múltiplo de 8.
>
> - **1er byte**: 'l' para little-endian, 'b' para big-endian. Utilizaremos 'l' **siempre**.
> - **2do byte**: tipo de mensaje, utilizaremos el valor **0x01 siempre**, que es el valor para llamadas a métodos.
> - **3er byte**: flags varios, utilizaremos **0x0 siempre**.
> - **4to byte**: versión del protocolo, utilizaremos **0x01 siempre**.
> - **1er entero**: **longitud** en bytes del **cuerpo**​.
> - **2do entero**: un número serie para identificar el mensaje. Utilizaremos un valor incremental por cada mensaje enviado por el cliente. El primer mensaje tendrá el número 0x0001, el segundo 0x0002, etc.
> Por último un **array de longitud variable** con los parámetros necesarios según el tipo de mensaje. Posee el siguiente formato:
>   - Un entero **UINT32** con la longitud del array.
>   - Por cada parámetro:
>       - Un byte indicando el *tipo de parámetro*.
>       - Un byte en 1.
>       - Un byte indicando el *tipo de dato* (utilizaremos **sólo strings** o equivalentes).
>       - *Longitud del dato* en bytes. La longitud no toma en cuenta el padding del último elemento.
> 
> Los parámetros se identifican por los siguientes tipos:
> - **Ruta del objeto:** se identifica por el tipo 1, y su tipo de dato es "o" (a fines prácticos es un string).
> - **Destino:** se identifica por el tipo 6, y su tipo de dato es "s" (string UTF-8).
> - **Interfaz:** se identifica por el tipo 2, y su tipo de dato es "s".
> - **Método:** se identifica por el tipo 3 y su tipo de dato es "s".
> - **Firma:** opcional, se identifica por el tipo 9 y su tipo de dato es "g" (a fines prácticos también es un string). Los métodos invocados sólo tendrán argumentos de tipo string, por lo que la firma será una cadena formada por caracteres 's' y de longitud igual a la cantidad de argumentos utilizados por el
método. Si el método no utiliza parámetros, no se envía. A diferencia de los otros tipos, no es seguido de un string.


### Formato del body

> Una vez leída la cabecera, si la misma posee una firma tenemos que **leer el cuerpo** con los **parámetros** que utiliza. Como mencionamos previamente, **sólo utilizaremos strings**. Al igual que en la cabecera los strings **deben terminar en '\0’**.
>
> El formato será, por cada parámetro:
> - Un **UINT32** con la longitud del parámetro, la cadena de bytes correspondientes, y el **\0**.


## Penalizaciones <a name="penalizaciones"></a>

> La siguiente es una lista de **restricciones técnicas** exigidas **no obligatorias** para la aprobación:
> 1. Las funciones de más de 20 líneas requieren una justificación clara de su exensión.
> 2. La lectura del archivo de entrada debe ser en **bloques de 32 bytes** (es decir, utilizar un buffer de lectura). No se puede trabajar con todo el contenido del archivo en memoria.

## Restricciones <a name="reestricciones"></a>

> La siguiente es una lista de **restricciones técnicas** exigidas de caracter **obligatorio**:
> 1. El sistema debe desarrollarse en **ISO C (C99)**. No se puede usar macros que alteren el standard del código excepto en el “.c” de sockets.
> 2. Está prohibido el uso de **variables y funciones globales**. La función ​ main no puede contener lógica del negocio, solamente debe servir como punto de entrada del flujo del programa.
> 3. El informe debe contener al menos un **diagrama** que represente alguna parte del diseño. No hace falta que sea UML, pero sí que sea **descriptivo**.
> 4. **El protocolo** de comunicación es **obligatorio**, no sugerido.

<hr>

# Resolución <a name="resolucion"></a>

