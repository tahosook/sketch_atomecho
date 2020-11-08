#ifndef __SLACK_H__
#define __SLACK_H__

#include <ssl_client.h>
#include <WiFiClientSecure.h>

void slack_setup();
void slack_senddata(String message);

#endif // __SLACK_H__
