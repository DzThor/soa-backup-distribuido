Autor: Daniel Ramírez Concepción

# Backup distribuido
Utilizando Qt he creado un sistema de backup distribuido propio.

La aquitectura será muchos a uno. Es decir, un cliente envia los datos a un servidor y este los distribuye a varios clientes, los cuales almacenan los archivos en su sistema de archivos local:

# Programas

* Un servidor y un cliente de backup.

# Servidor

* El servidor recibe los datos desde el cliente de origen y los retransmite hacia los clientes de destino.
* Sólo hace falta que haya un servidor en ejecución.
* El puerto de escucha del servidor se configura desde GUI.
* Al mismo tiempo pueden estarse ejecutando múltiples clientes en distintas máquinas.

# Cliente Backup

* Puede elegirse como como origen, para enviar datos a los demás clientes, o en su defecto, como cliente que recibe datos de los usuarios que envían.
* Los clientes de origen, eligen una ruta para realizar el backup, enviando todos los ficheros y directorios, a los demás clientes.
* La IP y el puerto del servidor al que se va a conectar es configurable desde la GUI
* En cada cliente la carpeta origen o destino de los archivos es configurable desde la GUI.
Funcionamiento

# Funcionamiento

* El sistema funciona de la siguiente forma:
1. El servidor se inicia. El usuario puede configurar el puerto y ponerlo a la escucha.
2. Un cliente se inicia.
  * El usuario puede configurar la dirección del servidor, elegir la carpeta de destino y conectarlo como cliente de destino.
  * O, el usuario puede configurar la dirección del servidor, elegir la carpeta de origen, indicar cuántos destinos deben estar conectados y conectar como cliente de origen.
3. Los clientes de destino esperan indefinidamente hasta que el servidor inicia la transferencia.
4. Los clientes de origen esperan hasta que hay suficientes clientes conectados en el servidor, entonces empiezan a recorrer el directorio y a transferir hacia el servidor.
5. El servidor envía los datos recibidos del cliente origen a los clientes destino, que deben reconstruir el arbol de directorios en su carpeta local.
6. Durante la transferencia los clientes muestran el progreso de la copia:
  * El nombre del archivo que se está copiando y el tamaño total.
  * Muestra el progreso de la copia total.

# Protocolo

He creado un protocolo propio:
  * ¿Qué protocolo de transporte usa?: TCP.
  * ¿Protocolo binario o texto?: texto.
  * ¿Formato?: Formato propio.
  * Diferentes tipos de mensajes diferenciados en `Msg2Server()` y `Msg2Client()`.
  * Para iniciar el envío el cliente recibe un mensaje de "Puedes enviar\n".
  * Se envía un fichero de información previo a los datos, para cada fichero, que incluye el nombre y su tamaño total.
  * Para mantener actualizada la barra de progreso utilizamos el contador del tamaño actual recibido/enviado y el tamaño total.
