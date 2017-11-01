// connection information for the Hue hub
#define HUE_HUB_IP_ADDRESS_BYTES {192,168,77,50}
#define HUE_HUB_IP_ADDRESS "192.168.77.50"
#define HUE_DEVELOPER_NAME "newdeveloper"
#define HUE_LIGHT_CRM_1 "5"
#define HUE_LIGHT_CRM_2 "6"

int led1 = D7;
bool light = FALSE;
int read_val = 0;

TCPClient client;

void setup() {
    pinMode(led1, OUTPUT);
    pinMode(D0, OUTPUT);
    pinMode(A0, INPUT);
    Particle.variable("computer_room", light);
    Particle.variable("A_read", read_val);

    digitalWrite(D0,HIGH);
}

void loop() {
    bool state_crm = FALSE;
    if (analogRead(A0) > 80 ) {
        state_crm = TRUE;
    }

    if (state_crm != light) {
        light = state_crm;
        do_light_update(light,HUE_LIGHT_CRM_1, 254);
        do_light_update(light,HUE_LIGHT_CRM_2, 254);
        if (light) {
            digitalWrite(led1, HIGH);
        }
        else {
            digitalWrite(led1, LOW);
        }
    }
    read_val = analogRead(A0);
    delay(200);
}

// do_light_update - On/Off, HUE light ID number, Brightness 32-254
void do_light_update(bool state, String light_id, int bri) {
    if (client.connect(HUE_HUB_IP_ADDRESS_BYTES, 80)) {
        char* req;
        asprintf(&req, "PUT /api/%s/lights/%s/state HTTP/1.0", HUE_DEVELOPER_NAME, light_id.c_str());

        char* host;
        asprintf(&host, "Host: %s", HUE_HUB_IP_ADDRESS);

        char* on;
        if (state) {
            asprintf(&on, "true");
        }
        else {
            asprintf(&on, "false");
        }

        char* content;
        asprintf(&content, "{\"on\":%s,\"bri\":%i}",on, bri);

        char* content_length;
        asprintf(&content_length, "Content-Length: %i", strlen(content));

        client.println(req);
        client.println(host);
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println(content_length);
        client.println();
        client.println(content);

        free(host);
        free(req);
        free(content_length);
        free(content);
        free(on);

        client.flush();
        client.stop();
    }
}
