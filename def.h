#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

#define MSG_REQ 0x00
#define MSG_ACK 0x01
#define MSG_ENTER 0x02
#define MSG_LEAVE 0x03

#define MSG_MSTR 0x10
#define MSG_SLV  0x20

#define SLAVENAME "slave"
#define EMPIRE "empire"
struct message{
  int TYPE;
  int clock;
  int senderID;
  int routeID;
  int senderTID; //to send an ACK
}typedef message;

int Lamport_clock = 0;
