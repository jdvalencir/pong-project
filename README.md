# pong-project
The project Pong is a practice where you can find a server made in C and a client that implements a protocol for connecting sockets using the TCP/IP architecture.

## Objetivo del protocolo 
1. Crear salas 
2.  Unirse a salas 
3. Comunicarse en la sala
4. Comunicarse durante del juego

Formato de mensaje: 
## Quiero mandar como PDU un Json que tenga:
```
 {
Type : “ ”
Payload : “ “    ----> ## En el payload se agregan los campos necesarios es decir, mi payload es variado y puede tener varios cambios eso depende de la request ##
 }
```
## Donde: 
```
1.Create room 
Request 
{
Type: “Create ROOM”
Nickname : “Jdvalencia”
}
Response
{ 
type : “CREATE ROOM_SUCCESS”
Code_room: “ “
}
```
```
2.Join Room 
Request 
{
type: “Join_”
nickname: “Jdvalencia”
code_room : “ “

Response 
{
Type: “JOIN_SUCCESS” 
Nickname: “ ”
}
```
## Conclusiones
### Usamos el protocolo TCP ya que no hay que emparejar muchos usuarios al mismo tiempo, esto debido a que contamos un con juego usuario a usuario que no requiere otro tipo de protocolo
---
### Es necesaria la instalación de Tk en python para el uso del juego
---
### Actualmente el juego solo funciona en sistemas operativos linux, esto debido a la configuración de los sockets, estamos trabajando en resolverlo
