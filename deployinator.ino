/* Deployinator
 *
 * Author: Casey Fulton <casey@caseyfulton.com>
 * Reference: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino
 * License: https://opensource.org/licenses/MIT
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "credentials.h"

#define HOLD_LED_PIN D1
#define DEPLOY_LED_PIN D2
#define OK_LED_PIN D3
#define PWR_LED_PIN D4

#define KEY_PIN D5
#define HOLD_PIN D6
#define DEPLOY_PIN D7

void setup() {
  Serial.begin(57600);

  pinMode(HOLD_LED_PIN, OUTPUT);
  pinMode(DEPLOY_LED_PIN, OUTPUT);
  pinMode(OK_LED_PIN, OUTPUT);
  pinMode(PWR_LED_PIN, OUTPUT);

  pinMode(KEY_PIN, INPUT);
  pinMode(HOLD_PIN, INPUT);
  pinMode(DEPLOY_PIN, INPUT);

  digitalWrite(PWR_LED_PIN, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    digitalWrite(OK_LED_PIN, HIGH);
    delay(250);
    digitalWrite(OK_LED_PIN, LOW);
  }
  digitalWrite(OK_LED_PIN, HIGH);
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // If wifi is connected, turn ok LED on.
  digitalWrite(OK_LED_PIN, WiFi.status() == WL_CONNECTED);

  // If key is on, turn hold LED on.
  digitalWrite(HOLD_LED_PIN, digitalRead(KEY_PIN));

  // If both key and hold are on, turn deploy LED on.
  digitalWrite(DEPLOY_LED_PIN, digitalRead(KEY_PIN) && digitalRead(HOLD_PIN));

  // If all three, trigger deploy.
  if (digitalRead(KEY_PIN) && digitalRead(HOLD_PIN) && digitalRead(DEPLOY_PIN)) {

    // De-bounce. Wait for 5 ms and double check inputs.
    delay(5);
    if (digitalRead(KEY_PIN) && digitalRead(HOLD_PIN) && digitalRead(DEPLOY_PIN)) {
      // Turn off all LEDs.
      digitalWrite(OK_LED_PIN, LOW);
      digitalWrite(HOLD_LED_PIN, LOW);
      digitalWrite(DEPLOY_LED_PIN, LOW);

      // Call deploy Jenkins job.
      bool result = deploy();

      // Turn on deploy (green) LED if deploy succeeded, or hold (red) LED if failed.
      if (result) {
        digitalWrite(DEPLOY_LED_PIN, HIGH);
      } else {
        digitalWrite(HOLD_LED_PIN, HIGH);
      }
      delay(5000);
      digitalWrite(HOLD_LED_PIN, LOW);
      digitalWrite(DEPLOY_LED_PIN, LOW);
      delay(5000);
    }
  }
}

bool deploy() {
  WiFiClientSecure client;

  Serial.println("Beginning deploy...");

  if (!client.connect(JENKINS_JOB_HOST, JENKINS_PORT)) {
    Serial.println("Connection failed.");
    return false;
  }

  if (!client.verify(JENKINS_SSL_FINGERPRINT, JENKINS_JOB_HOST)) {
    Serial.println("Could not verify certificate.");
  }

  client.print(String("POST ") + JENKINS_JOB_PATH + " HTTP/1.1\r\n" +
               "Host: " + JENKINS_JOB_HOST + "\r\n" +
               "Authorization: Basic " + JENKINS_AUTH + "\r\n" +
               "User-Agent: SOAR Deployinator v2\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent.");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(String("< ") + line);

    if (line.indexOf("201 Created") > 0) {
      return true;
    }

    if (line == "\r") {
      break;
    }
  }

  client.stop();
  Serial.println("Something went wrong.");
  return false;
}
