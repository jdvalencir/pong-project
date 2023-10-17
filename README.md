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
