// connection information for the Hue hub
#define HUE_HUB_IP_ADDRESS "192.168.77.20"
#define HUE_DEVELOPER_NAME "8BlbZ5TVDi3GsbcMJeA74BcIUlxNAlF5x2DmkHvO"

int HUE_LIGHT_CRM_1 = 5;
int HUE_LIGHT_CRM_2 = 6;

#include "HttpClient.h"
#include "application.h"

int led1 = D7;
bool light = FALSE;
String status = "off";
int read_val = 0;

HttpClient http;

http_header_t headers[] = {
  { "Content-Type", "application/x-www-form-urlencoded" },
  { "Accept", "*/*"},
  { NULL, NULL }
};

http_request_t request;
http_response_t response;


void setup() {
  pinMode(led1, OUTPUT);
  pinMode(D0, OUTPUT); 
  pinMode(A0, INPUT);
  Particle.variable("light", status);
  Particle.variable("A_read", read_val);
  Particle.publish("Setup finished", PUBLIC);
  digitalWrite(D0,HIGH);
}

void loop() {
    bool state_crm = FALSE;
    if (analogRead(A0) > 80 ) {
        state_crm = TRUE;
    }

    if (state_crm != light) {
        light = state_crm;
        do_light_update(light,HUE_LIGHT_CRM_1);
        // do_light_update(light,HUE_LIGHT_CRM_2);
        Particle.publish("After-Do-light", PUBLIC);
        if (light) {
            digitalWrite(led1, HIGH);
            status = "on ";
        }
        else {
            digitalWrite(led1, LOW);
            status = "off";
        }
    }
    read_val = analogRead(A0);
    delay(200);
}

// do_light_update - On/Off, HUE light ID number
void do_light_update(bool state, int light_id) { 
    Particle.publish("do-light", "13", PUBLIC);
    char path[128]; 
    char body[64];
    
    sprintf(path, "/api/%s/lights/%d/state", HUE_DEVELOPER_NAME, light_id);
    
    // Alsways default to turning the lights off.
    sprintf(body, "{\"on\":false}" );
    
    if (state) {
        sprintf(body, "{\"on\":true}" );
    }
    
    request.hostname = HUE_HUB_IP_ADDRESS; 
    request.port = 80;
    request.path = path; 
    request.body = body;
    
    http.put(request, response, headers); 
    delay(1000);
}