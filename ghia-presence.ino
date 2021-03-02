/*
 * Presence detection for BLE devices
 * 
*/

#include "config.h"
#include "BLEDevice.h"

static BLEAddress *pServerAddress;
bool deviceFound = false;
bool recent = false;

BLEScan* pBLEScan;

static BLEAdvertisedDevice* myDevice;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice Device) {
    
    // Serial.print("BLE Advertised device found: ");
    // Serial.println(Device.toString().c_str());
    pServerAddress = new BLEAddress(Device.getAddress());
    for (int i = 0; i < (sizeof(knownMACs) / sizeof(knownMACs[0])); i++) {
      if (strcmp(pServerAddress->toString().c_str(), knownMACs[i].c_str()) == 0) {
        if (Device.getRSSI() > minRSSI) {
          deviceFound = true;
          Device.getScan()->stop();
          break;
        }
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Ghia (Presence)");
  
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  Serial.println("Done initiating BLE");
}

void hello() {
  recent = true;
  Serial.println("Running hello");
  /// code here...
}

void goodbye() {
  recent = false;
  Serial.println("Running goodbye");
  /// code here...
}

void Bluetooth() {
  unsigned long scan_timer;
  Serial.println();
  Serial.println("Scanning for 60 seconds (or until found)...");
  deviceFound = false;
  scan_timer = millis();
  BLEScanResults scanResults = pBLEScan->start(60);
  
  if (deviceFound) {
    Serial.print("Found in ");
    Serial.print((millis() - scan_timer)/1000);
    Serial.println(" seconds (Sleeping for 10 seconds)"); // Technically, for 11 seconds.

    if (!recent) {
      hello();
    }
    
    delay(10000);
  } else {
    Serial.println("Nothing found (Sleeping for 1 second)");
    
    if (recent) {
      goodbye();
    }

    delay(1000);
  }
}

void loop() { 
  Bluetooth();
}
