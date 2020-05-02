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
3. [Modelo propuesto](#modelo)
    * [Primeras ideas](#primeras_ideas)
    * [Procesamiento de datos de entrada](#procesamiento)
        * [Armando las *calls*](#armando)
    * [Implementando el protocolo](#protocolo)
    * [Diseño final](#diseno_final)
4. [Detalles de implementación](#detalles)
    * [Procesamiento de datos de entrada](#stdin)
    * [Consideración del endianness](#endianness)
    * [Códigos de retorno](#retorno)
5. [Conclusiones](#conclusiones)


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

## Restricciones <a name="restricciones"></a>

> La siguiente es una lista de **restricciones técnicas** exigidas de caracter **obligatorio**:
> 1. El sistema debe desarrollarse en **ISO C (C99)**. No se puede usar macros que alteren el standard del código excepto en el “.c” de sockets.
> 2. Está prohibido el uso de **variables y funciones globales**. La función ​ main no puede contener lógica del negocio, solamente debe servir como punto de entrada del flujo del programa.
> 3. El informe debe contener al menos un **diagrama** que represente alguna parte del diseño. No hace falta que sea UML, pero sí que sea **descriptivo**.
> 4. **El protocolo** de comunicación es **obligatorio**, no sugerido.

<hr>

# Modelo propuesto <a name="modelo"></a>

## Primeras ideas <a name="primeras_ideas"></a>

Para resolver el trabajo planteado, se encaró el problema pensando primero en qué **TDAs** sería conveniente diseñar, para que nuestra solución sea **modular**, y al mismo tiempo intentando evitar, en la mayor medida posible, el **acoplamiento** entre los distintos TDAs diseñados.

En este contexto, y teniendo en cuenta que debíamos generar dos aplicativos independientes: **cliente** y **servidor** que puedan comunicarse entre sí utilizando **sockets**, en un primer boceto pensé en los siguiente TDAS:

- **Cliente**: sería el TDA que se instancia en la entrada del aplicativo `Cliente`, y sería el encargado de manejar el flujo del programa. Este TDA tendría un `socket` propio, con el que se conectaría al `servidor`.

- **Servidor**: análogo al cliente, este TDA sería el que maneja el flujo del programa `Servidor`, siendo instanciado en su main. Propiamente, tendría que poseer dos `sockets`: un socket *`aceptador`*, encargo de aceptar conexiones entrantes, y otro socket *`peer`*, que sería la referencia al socket aceptado una vez que el cliente se conecte. A través del socket *`peer`* nos comunicaremos con el cliente.

- **Socket**: este TDA es fundamental, pues nos permitirá encapsular los métodos necesarios para permitir la conexión Cliente-Servidor. Principalmente, el socket estará formado por un `file descriptor` que lo identifica.

| ![primeras_ideas](img/primeras_ideas.png) | 
|:--:| 
| *Primeros TDAs diseñados* |



## Procesamiento de datos de entrada <a name="procesamiento"></a>

Una vez que logré (con los tres TDAs diseñados hasta el momento) establecer una comunicación estable entre el `Cliente` y el `Servidor` logrando mandar mensajes y recibirlos de forma correcta, me predispuse a continuar con el trabajo, centrandome ahora en la entrada de las llamadas `Calls`.

Según mi implementación, una `call` es una **estructura de datos a llenar**, para luego ser enviada del cliente al servidor. Cada `call`, como se vio en el enunciado, está formada por distintos componentes: un `destino`, una `ruta`, una `interfaz`, y un `método`. A su vez y de manera opcional puede contar con `parámetros` si el método incluye una firma.

Fue en este punto cuando decidí diseñar el TDA `call`. Su utilización me permitiría encapsular el manejo de los datos, así como el ***parseo*** de la misma, obteniendo un código más limpio y con **responsabilidades más claras**.

A su vez, como se vio en el enunciado, las *calls* al cliente le pueden llegar por medio de un **archivo de entrada** o por **entrada estándar**. Para manejar esto decidí tomar una idea de la clase de presentación del trabajo práctico: la utilización de un `stdin_streamer`, un TDA cuyo principal objetivo será el de **encapsular la entrada de datos**, llamando a una función ***callback*** y pasándole la línea que contiene a la call lista para su parseo.

### Armando las calls listas para el envío (client-side) <a name="armando"></a>

En este punto, ya contaba con 5 TDAs con responsabilidades bien claras:

- **Cliente**, manejando el flujo del programa `Cliente`, utilizando un `stdin_streamer` para recibir datos de la entrada, y armando las `calls` para luego enviarlas al `servidor`;

- **Servidor**, manejando el flujo del programa `Servidor`, recibiendo las `calls` del cliente para luego imprimirlas por salida estándar;

- **Socket**, encapsulando los métodos necesarios para permitir la conexión entre `Cliente` y `Servidor`;

- **Call**, encapsulando el parseo de los distintos componentes que la forman, y permitiendo el agrupamiento de los mismos;

- **Stdin_Streamer**, responsable del ingreso de los datos al aplicativo `Cliente`.

El flujo de mis programas, entonces, sería el siguiente:

- **Aplicativo Cliente:** se inicia el `socket`, y se intenta conectar al servidor utilizando el *hostname* y el *puerto* recibidos por parámetro. Una vez conectado, se le pide al `stdin_streamer` una línea conteniendo una `call`,  para parsearla y enviarla al `servidor`. Posteriormente se esperaría por una respuesta del servidor, para imprimirla y repetir pidiéndole al `stdin_streamer` otra línea. Una vez que no hay más datos de entrada, termina su ejecución liberando los recursos.

- **Aplicativo Servidor:** se inicia el socket aceptador, y se espera por la conexión de un `cliente`. Una vez conectado, se lo asigna al socket `peer`, socket a través del cuál se recibirán las distintas `calls` enviadas. Cuando se recibe una `call`, se imprime por salida estándar y se envía una confirmación al `cliente`. Se repite este proceso hasta no recibir más calls, momento en el cuál se termina la ejecución.

## Implementando el protocolo <a name="protocolo"></a>

En este punto tenía ya un sistema funcionando que podía enviar mensajes entre **cliente** y **servidor**, leyendo los mismos desde la entrada estándar del cliente. Lo único que falta es implementar el **protocolo**.

El **protocolo** es la parte más importante en todo sistema de comunicación, ya que establece las **reglas** que nos permitirán lograr la misma de forma eficiente y ordenada.

Teniendo la `call` armada en el `Cliente`, ahora es momento de armar el mensaje siguiendo las reglas establecidas por el **Protocolo D-BUS**, para posteriormente ser enviado al `Servidor`. 

Inmediatamente se me ocurrió que era necesario **diseñar un nuevo TDA** para **encapsular el protocolo**, permitiéndome **independizarme** del mismo en el resto del programa. También tuve en cuenta que el protocólo tiene una doble función:

1. Debe actuar como un **serializador** para el `Cliente`, pues ordena la información de una cierta manera, preparando el mensaje como una cadena de bytes lista para ser enviada.

2. Debe actuar como un **decodificador** para el `Servidor`, pues debe recibir la cadena de bytes previamente serializada y entenderla, decodificando la misma y dándole un sentido.

Frente a esta doble tarea y para evitar que mi TDA **protocolo** tenga muchas responsabilidades, decidí diseñar dos TDAs distintos e independientes:

- **dbus_cliente:** *serializador* del mensaje del lado del `Cliente`.
- **dbus_servidor:** *decodificador* del mensaje del lado del `Servidor`.

Tras su implementación, pude llegar a mi diseño final, con el que me siento satisfecho, ya que pude **encapsular comportamiento** y lograr un **acoplamiento bajo**.

## Diseño final <a name="diseno_final"></a>

Mi diseño final consta de **7 TDAs** con **responsabilidades claras** y un acoplamiento razonable. Los mismos se muestran a continuación en el siguiente gráfico:

// grafico


Flujo de aplicativo `Cliente`:

// grafico


Flujo de aplicativo `Servidor`:

// grafico


Estoy satisfecho con el diseño final logrado, puesto que **encapsular el comportamiento propio del protocolo** en tres TDAs me permitirá en un futuro **reutilizar** los aplicativos **Cliente** y **Servidor**, simplemente definiendo un nuevo protocolo según el problema a resolver.

En las próximas secciones, mostraré cómo afronté ciertos desafíos planteados en el enunciado, y la **solución** que utilicé frente a los mismos.

<hr>

# Detalles de implementación <a name="detalles"></a>

En esta sección detallaré las **soluciones propuestas** frente a ciertos problemas que fueron surgiendo a lo largo del desarrollo del trabajo y que considero que son de **interés**.

## Procesamiento de datos de entrada <a name="stdin"></a>

Si bien para procesar los datos de entrada podríamos utilizar memoria dinámica y leer todo el archivo de una sóla vez, o podríamos leer de a 1 byte hasta reconocer el fin de una línea, estas primeras ideas resultarían increíblemente ineficientes:

- **Cargar todo el archivo en memoria** resulta muy ineficiente en cuanto a utilización de recursos;
- **Leer de a 1 byte** implica llamar a la ***syscall*** `read` tantas veces como caracteres existan en la entrada de datos, y considerando que esta *syscall* es muy costosa, este *approach* se vuelve inviable.

Se decide entonces, tomando la idea del enunciado, implementar una **solución compromiso**, ni un extremo ni el otro: se leerá el archivo de entrada de a **32 bytes**, utilizando para esto un *buffer* estático de **longitud fija** (*en el stack, no es necesario utilizar memoria dinámica en el heap*). Luego, **se itera** sobre este buffer estático hasta **encontrar el fin de línea** que delimita nuestras `calls`, en este caso y por enunciado, el caracter `\n`. Una vez que se la encuentra, se **envía** la call al protocolo para ser procesada y enviada.

Claramente no todas las líneas serán fijas de 32 bytes generando un final feliz, tenemos que analizar los **casos bordes**:

- Podría pasar por ejemplo que la línea sea de longitud **mayor a 32 bytes**, por lo que al llegar al último byte y no haber encontrado el caracter delimitador, deberemos **guardar nuestra línea parcial en memoria dinámica** y leer otros 32 bytes, para completarla. Esto puede repetirse tantas veces como sea necesario hasta tener la línea final formada.

- En un caso totalmente opuesto, podría tratarse de líneas con longitud **menor a 32 bytes**, en cuyo caso podríamos tener varias líneas en los mismos 32 bytes. Este caso es más sencillo de tratar: cada vez que encontremos un caracter delimitador `\n` en el buffer, **enviaremos la línea encontrada hasta ese momento**, y usaremos variables para indicar que **la próxima línea empieza desde la posición siguiente** en nuestro buffer.

El comportamiento descripto puede encontrarse en la función: **`stdin_streamer_run`**. Esta función posee más de 20 líneas de código, límite que se estableció en el enunciado, pero creo que **su extensión se justifica** con la **necesidad de cubrir** todos los **casos bordes** de este problema.

## Consideración del endianness <a name="endianness"></a>

Como sabemos, la forma en que los bytes se **organizan en memoria depende** de nuestra **arquitectura**. Es decir, un entero de 4 bytes no se almacenará de igual manera en todos los sistemas. Tenemos dos tipos de endianness: *little-endian* y *big-endian*.

En las arquitecturas *little-endian*, se almacena el byte menos significativo en la primer dirección de memoria de nuestra variable. Por ejemplo, si definimos un entero de 4 bytes:

```
uint32_t a = 1;
```

En la dirección de a, `&a`, encontraremos los bytes en el siguiente órden: |1|0|0|0|.

En cambio, este mismo código corriendo en una arquitectura *big-endian*, almacenaría los bytes la siguiente manera: |0|0|0|1|.

Frente a esta problemática se hace **necesario** en todo protocolo de comunicación **establecer el endianness** a utilizar. En este caso por enunciado se utiliza *little-endian*, por lo que los mensajes deberán ser enviados organizando los bytes según esta convención.

Para asegurarme de que cuando escriba un entero en el mensaje el mismo se escriba en *little-endian*, utilicé dos funciones auxiliares:
- Una función que me permite **conocer el endianness** de la arquitectura actual;
- Una función built-in de C que permite realizar un ***swap*** de los bytes de un entero.

La primera funciona de manera muy simple *(la idea fue obtenida de StackOverflow, ya que se me habían ocurrido maneras más complicadas de chequear el endianness y esta forma me pareció óptima)*. La función es la siguente:

```
static int _i_am_big_endian() {   
    int n = 1;
    if (*(char *)&n == 1) {
        return 0;
    } else {
        return 1;
    }
}


file: client_dbus_client.c
```

La idea es la siguiente: establecemos una variable entera inicializada en `1`, y luego comparamos lo que hay en **su primer byte** con una **constante = 1**, haciendo uso de lo que expliqué anteriormente. Si estamos en *big-endian*, el primer byte será 0, mientras que si estamos en *little-endian*, será 1.

A continuación muestro como ejemplo el **uso de esta función** a la hora de escribir enteros en el mensaje:

```
static void _copy_uint32_checking_endianness(char* dest, int* offset,
                                             uint32_t* src, size_t len) {
    uint32_t int_to_copy = *src;

    if (_i_am_big_endian()) {
        int_to_copy = __builtin_bswap32(int_to_copy);
    }

    _copy_to_msg(dest, offset, &int_to_copy, len);
}


file: client_dbus_client.c
```

Lo que hacemos es **chequear la arquitectura** utilizando la función previamente explicada, y en caso de estar en *big-endian* **invertimos los bytes**. Es importante aclarar que **no modificamos la variable original**, ya que en la estructura de datos las variables deben permanecer legibles por la arquitectura actual. Si yo hubiese invertido los bytes directamente desde `uint32_t* src` y mi programa luego hubiese querido imprimir dicho valor, hubiese impreso el valor inverso. O peor aún, si hubiese querido realizar un `malloc` por dicho valor de memoria, tendríamos problemas.

## Códigos de retorno <a name="retorno"></a>

Si bien no era solicitado por el enunciado, decidí utilizar **códigos de retorno** específicos para tener mayor información ante un error en la ejecución. Los mismos los detallo a continuación:

- Códigos de retorno en **`Cliente`**:

    | Código de retorno | Descripción | 
    |-------------------|-------------|
    | 1: *USAGE_ERROR* | No se recibieron los parámetros esperados |
    | 2: *CREATE_ERROR* | Ocurrió un error al instanciar al cliente |
    | 3: *INPUT_FILE_ERROR* | El archivo de entrada no existe o no se pudo abrir |
    | 4: *CONNECT_ERROR* | No se pudo conectar con el servidor |
    | 5: *SEND_ERROR* | Ocurrió un error en el envío del mensaje |
    | 6: *SHUTDOWN_ERROR* | Ocurrió un error al intentar apagar el cliente|
    | 7: *DESTROY_ERROR* | Ocurrió un error al intentar liberar los recursos utilizados |

- Códigos de retorno en **`Servidor`**:

    | Código de retorno | Descripción | 
    |-------------------|-------------|
    | 1: *USAGE_ERROR* | No se recibieron los parámetros esperados |
    | 2: *CREATE_ERROR* | Ocurrió un error al instanciar al servidor |
    | 3: *OPEN_ERROR* | No se pudo abrir el servidor a la escucha de conexiones entrantes |
    | 4: *ACCEPT_ERROR* | No se pudo aceptar la conexión entrante |
    | 5: *RECEIVE_ERROR* | Ocurrió un error en la recepción del mensaje |
    | 6: *SHUTDOWN_ERROR* | Ocurrió un error al intentar apagar el servidor |
    | 7: *DESTROY_ERROR* | Ocurrió un error al intentar liberar los recursos utilizados |

<hr>

# Conclusiones <a name="conclusiones"></a>

Esta fue la primera vez que implementé un aplicativo **Cliente-Servidor**, así como un protocolo. Antes de este trabajo, no sabía bien lo que era un `socket`, ni mucho menos qué era un `protocolo TCP`. Me parecieron conceptos muy interesantes, y me divertí mucho armando estos aplicativos.

Si bien soy consciente de que **el código podría haber sido más optimizado**, refactorizando nuevamente y creando nuevos TDAs para **encapsular aún más** el comportamiento, estoy satisfecho con el trabajo realizado ya que no sólo consistió en sentarse a escribir código, sino que también incluyó muchas horas de **investigación** y de **aprendizaje** sobre el tema.























