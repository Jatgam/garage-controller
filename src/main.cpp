#include "main.h"

// Default values for WiFiManager Configuration
char mqtt_server[40];
char mqtt_port[6] = "8883";
char mqtt_user[10];
char mqtt_pass[20];

char door1_name[10];
char door2_name[10];

byte uuid[16];
char uuidC[32];

bool save_config = false;

int update_interval = 100000;
unsigned long last_update=0;

volatile bool state_changed = false;

void saveConfigCallback () {
  Serial.println("Saving config...");
  save_config = true;
}

void handlePinStateChange() {
    state_changed = true;
}

Garage::Garage() {
    temp = new Temperature();
    door1 = new Door(DOOR1_OPEN_SENSOR_PIN, DOOR1_CLOSED_SENSOR_PIN, DOOR1_RELAY_PIN, &handlePinStateChange);
    door2 = new Door(DOOR2_OPEN_SENSOR_PIN, DOOR2_CLOSED_SENSOR_PIN, DOOR2_RELAY_PIN, &handlePinStateChange);
}

//ModuleVersion firmwareVersion = (char*)JATGAM_MODULE_VERSION;

//WiFiClientSecure sslClient;
WiFiClient wificlient;
PubSubClient mqttClient(wificlient);
Garage* garage;

ICACHE_FLASH_ATTR String uuidToStringNoDash(uint8_t* uuidLocation) {
    String string = "";
    int i;
    for (i=0; i<16; i++) {
        int topDigit = uuidLocation[i] >> 4;
        int bottomDigit = uuidLocation[i] & 0x0f;
        // High hex digit
        string += "0123456789abcdef"[topDigit];
        // Low hex digit
        string += "0123456789abcdef"[bottomDigit];
    }

    return string;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    char data[length+1];
    for (int i=0; i<length; i++) {
        data[i] = (char)payload[i];
    }

    const int bufferSize = 200;
    StaticJsonBuffer<bufferSize> jsonBuffer;
    JsonObject& jsonPayload = jsonBuffer.parseObject(data);
    if (!jsonPayload.success())
    {
        Serial.println("Failed to parse mqtt payload");
        return;
    }
    jsonPayload.printTo(Serial);
}

void mqttReconnect() {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(uuidC, mqtt_user, mqtt_pass)) {
            Serial.println("connected");
            // ... and subscribe to topic
            mqttClient.subscribe(uuidC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

bool publishDeviceStatus(char* publishUpdateTopic) {
    // DynamicJsonBuffer jsonBuffer;
    // JsonObject& json = jsonBuffer.createObject();
    // json["deviceModule"] = JATGAM_MODULE;
    // json["deviceVersion"] = JATGAM_MODULE_VERSION;
    // json["deviceId"] = uuidC;
    // json["tempC"] = garage->temp->getCelsius();
    // json["door1Name"] = door1_name;
    // json["door2Name"] = door2_name;
    // json["door1State"] = garage->door1->getStateString();
    // json["door2State"] = garage->door2->getStateString();
    // size_t len = json.measureLength();
    // size_t size = len + 1;
    // char output[size];
    // json.printTo(output, size);

    bool pub_temp = mqttClient.publish("jatgam/garage/tempC", garage->temp->getCelsiusChar());
    bool pub_door1state = mqttClient.publish("jatgam/garage/door1/state", garage->door1->getStateString());
    bool pub_door2state = mqttClient.publish("jatgam/garage/door2/state", garage->door2->getStateString());
    //bool result = mqttClient.publish(publishUpdateTopic, output);
    return false;
}

void httpFirmwareUpdate(char* updateURL) {
    t_httpUpdate_return return_code = ESPhttpUpdate.update(updateURL);
    switch(return_code) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
    }
}

void setup(void) {

    Serial.begin(115200);
    garage = new Garage();

    Serial.println("Mounting file system for config...");

    if (SPIFFS.begin()) {
        //SPIFFS.format();
        Serial.println("mounted file system");
        if (SPIFFS.exists("/config.json")) {
            Serial.println("reading config file");
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                Serial.println("opened config file");
                size_t size = configFile.size();

                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                json.printTo(Serial);
                if (json.success()) {
                    Serial.println("\nparsed json");
                    strcpy(mqtt_server, json["mqtt_server"]);
                    strcpy(mqtt_port, json["mqtt_port"]);
                    strcpy(mqtt_user, json["mqtt_user"]);
                    strcpy(mqtt_pass, json["mqtt_pass"]);
                    strcpy(door1_name, json["door1_name"]);
                    strcpy(door2_name, json["door2_name"]);
                    strcpy(uuidC, json["uuid"]);

                    // const char* config_version = json["module_version"];
                    // if (firmwareVersion != ModuleVersion(config_version)) {
                    //     Serial.println("WARNING: Config version doesnt match firmware");
                    //     // Handle upgrades?
                    // }

                } else {
                    Serial.println("failed to load json config");
                }
            }
        } else {
            // Initialize uuid on first setup
            ESP8266TrueRandom.uuid(uuid);
            uuidToStringNoDash(uuid).toCharArray(uuidC, 32);
        }
    } else {
        Serial.println("failed to mount FS");
    }

    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
    WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 10);
    WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);
    WiFiManagerParameter custom_door1_name("door1", "door1 name", door1_name, 10);
    WiFiManagerParameter custom_door2_name("door2", "door2 name", door2_name, 10);

    WiFiManager wifiManager;

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_pass);
    wifiManager.addParameter(&custom_door1_name);
    wifiManager.addParameter(&custom_door2_name);

    //wifiManager.resetSettings();

    wifiManager.setTimeout(320);

    if (!wifiManager.autoConnect(AP_AUTOCONFIG_SSID, AP_AUTOCONFIG_PASSWORD)) {
        Serial.println("failed to connect and hit timeout");
        delay(3000);

        ESP.reset();
        delay(5000);
    }

    Serial.println("WiFi Connected...");

    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(mqtt_user, custom_mqtt_user.getValue());
    strcpy(mqtt_pass, custom_mqtt_pass.getValue());
    strcpy(door1_name, custom_door1_name.getValue());
    strcpy(door2_name, custom_door2_name.getValue());

    if (save_config) {
        Serial.println("saving config");
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        json["mqtt_server"] = mqtt_server;
        json["mqtt_port"] = mqtt_port;
        json["mqtt_user"] = mqtt_user;
        json["mqtt_pass"] = mqtt_pass;
        json["door1_name"] = door1_name;
        json["door2_name"] = door2_name;
        //json["module_version"] = JATGAM_MODULE_VERSION;
        json["uuid"] = uuidC;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
            Serial.println("failed to open config file for writing");
        }

        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    mqttClient.setServer(mqtt_server, atoi(mqtt_port));
    mqttClient.setCallback(mqttCallback);

}

void loop(void) {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }
    mqttClient.loop();

    unsigned long currentMillis = millis();
    if ((unsigned long)(currentMillis - last_update) >= update_interval || state_changed){
        Serial.print("Temperature: ");
        Serial.print(garage->temp->getCelsius());
        Serial.print(" ");
        Serial.print((char)223);
        Serial.println("C");
        Serial.printf("Door 1 Status: %s\n", garage->door1->getStateString());
        Serial.printf("Door 2 Status: %s\n", garage->door2->getStateString());
        if (!publishDeviceStatus((char*)"deviceUpdates")) {
            Serial.println("Failed to publish device updates...");
        }
        if (state_changed) {
            state_changed = false;
        }
        last_update = currentMillis;
    }
}
