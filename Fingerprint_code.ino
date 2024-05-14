//by Kaloso mbengwa 

#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Fingerprint scanner pins
//be sure to change the keypad configuration to suit your connection
#define Finger_Rx 14 // D5
#define Finger_Tx 12 // D6
#define key1 2
#define key2 0
#define key3 9
#define key4 10
SoftwareSerial mySerial(Finger_Rx, Finger_Tx);
Adafruit_Fingerprint finger(&mySerial);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display
uint8_t id;  // Global variable for storing the fingerprint ID
void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
  Wire.begin(5, 4); // GPIO 5 (D1) as SDA, GPIO 4 (D2) as SCL
  lcd.init(); // Initialize the LCD
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Fingerprint");

  if (finger.verifyPassword()) {
    lcd.setCursor(0, 1);
    lcd.print("Sensor found");
    delay(2000);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("No sensor found");
    while (1) { delay(1); } // Stop everything if no sensor is found
  }
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);
  
}

void loop() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1:Enroll 2:Match");
    lcd.setCursor(0, 1);
    lcd.print("3:Delete");
    Serial.println("Ready for operation");

    // Ensure there's a user input and it's correctly handled
    char choice = waitForChoice();

    switch (choice) {
        case '2':
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enrolling...");
            Serial.println("Enrolling operation");
            enrollFingerprint();
            break;
        case '3':
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Matching...");
            Serial.println("Matching operation");
            matchFingerprint();
            break;
        case '4':
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Deleting...");
            Serial.println("Deleting operation");
            deleteFingerprint();
            break;
        case '1':
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Empty Database?");
            lcd.setCursor(0, 1);
            lcd.print("Key4: Yes");
            emptyDatabase();
            break;
        default:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Invalid choice");
            delay(2000); // Show error for 2 seconds
            break;
    }

    // Wait for a moment before showing the main menu again
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Operation Done");
    Serial.println("Operation Done");
    delay(2000);
}

char waitForChoice() {
     while (true) {
        int key1S = digitalRead(key1);
        int key2S = digitalRead(key2);
        int key3S = digitalRead(key3);
        int key4S = digitalRead(key4);

        if (!key1S) {
            // Enroll button pressed
            while (!digitalRead(key1)) {} // Wait for button release
            return '1';
        }

        if (!key2S) {
            // Match button pressed
            while (!digitalRead(key2)) {} // Wait for button release
            return '2';
        }
        if (!key3S) {
            // Match button pressed
            while (!digitalRead(key3)) {} // Wait for button release
            return '3';
        }
         if (!key4S) {
            // Match button pressed
            while (!digitalRead(key4)) {} // Wait for button release
            return '4';
        }
        delay(100); // Reduce CPU usage
    }
}

uint8_t readNumberFromKeypad(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter ID: 1-127");

  id = 1;
  while (true) {
    int key1S = digitalRead(key1);
    int key2S = digitalRead(key2);
    int key3S = digitalRead(key3);
    int key4S = digitalRead(key4);

    if (!key1S) {
      // Increment ID by 1
      id++;
      while (!digitalRead(key1)) {} // Wait for button release
    }

    if (!key2S) {
      // Increment ID by 10
      id += 10;
      while (!digitalRead(key2)) {} // Wait for button release
    }

    if (!key3S) {
      // Decrement ID by 5
      id -= 5;
      while (!digitalRead(key3)) {} // Wait for button release
    }

    if (!key4S) {
      // OK button pressed
      while (!digitalRead(key4)) {} // Wait for button release
      break;
    }

    // Ensure ID is within the valid range (1-127)
    if (id < 1) {
      id = 1;
    } else if (id > 127) {
      id = 127;
    }

    lcd.setCursor(0, 1);
    lcd.print("ID: ");
    lcd.print(id);

    delay(100); // Reduce CPU usage
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enrolling ID:");
  lcd.setCursor(12, 0);
  lcd.print(id);
  Serial.println(id);

  return id;
}

void enrollFingerprint() {
  id = readNumberFromKeypad();
  if (id < 1 || id > 127) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Invalid ID");
    delay(2000);
    return;
  }

  while (!getFingerprintEnroll());
}

void matchFingerprint() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Place Finger");

    
    uint8_t result = getFingerprintIDez();

    // Clear the display and show matching result
    lcd.clear();
    lcd.setCursor(0, 0);

    if (result != FINGERPRINT_OK) {
        lcd.print("No Match Found");
        
        return;
    } else {
        lcd.print("Match Found!");
        lcd.setCursor(0, 1);
        lcd.print("ID: ");
        lcd.print(finger.fingerID);
        Serial.print("Match found! ID: ");
        Serial.println(finger.fingerID);
        delay(2000);  // Display result for 2 seconds
        return;
    } 
}
void deleteFingerprint() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter ID: 1-127");

    uint8_t id = 1;
    while (true) {
        int key1S = digitalRead(key1);
        int key2S = digitalRead(key2);
        int key3S = digitalRead(key3);
        int key4S = digitalRead(key4);

        if (!key1S) {
            // Increment ID by 1
            id = (id % 127) + 1;
            while (!digitalRead(key1)) {} // Wait for button release
        }

        if (!key2S) {
            // Increment ID by 10
            id = ((id + 9) % 127) + 1;
            while (!digitalRead(key2)) {} // Wait for button release
        }

        if (!key3S) {
            // Decrement ID by 5
            id = ((id - 6 + 127) % 127) + 1;
            while (!digitalRead(key3)) {} // Wait for button release
        }

        if (!key4S) {
            // OK button pressed
            while (!digitalRead(key4)) {} // Wait for button release
            break;
        }

        lcd.setCursor(0, 1);
        lcd.print("ID: ");
        lcd.print(id);
        

        delay(100); // Reduce CPU usage
    }

    // Validate the ID
    if (id < 1 || id > 127) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid ID");
        delay(2000);
        return;
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Deleting ID:");
    lcd.setCursor(12, 0);
    lcd.print(id);
    Serial.println(id);

    uint8_t p = finger.deleteModel(id);

    if (p == FINGERPRINT_OK) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Deleted!");
        delay(1000);
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Comm error");
        delay(2000);
    } else if (p == FINGERPRINT_BADLOCATION) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bad location");
        delay(2000);
    } else if (p == FINGERPRINT_FLASHERR) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Flash error");
        delay(2000);
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Unknown error");
        delay(2000);
    }
}
void emptyDatabase() {
    while (true) {
        int key4S = digitalRead(key4);

        if (!key4S) {
            // Key4 pressed, confirm emptying the database
            while (!digitalRead(key4)) {} // Wait for button release
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Emptying DB...");
            finger.emptyDatabase();
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Database Empty");
            delay(2000);
            break;
        }

        delay(100); // Reduce CPU usage
    }
}
uint8_t getFingerprintEnroll() {
  int p = -1;

  // First Finger Scan
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place finger");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.setCursor(0, 1);
        lcd.print("Image taken");
        delay(1000);
        break;
      case FINGERPRINT_NOFINGER:
        lcd.setCursor(0, 1);
        lcd.print("Waiting...");
        delay(500);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.setCursor(0, 1);
        lcd.print("Comm error");
        delay(2000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.setCursor(0, 1);
        lcd.print("Imaging error");
        delay(2000);
        break;
      default:
        lcd.setCursor(0, 1);
        lcd.print("Unknown error");
        delay(2000);
        break;
    }
  }

  p = finger.image2Tz(1);
  lcd.clear();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Converted");
      delay(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image messy");
      delay(2000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Comm error");
      delay(2000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Feature error");
      delay(2000);
      return p;
    default:
      lcd.print("Unknown error");
      delay(2000);
      return p;
  }

  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  p = 0;

  // Wait until the finger is removed
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.clear();
  lcd.print("ID: ");
  lcd.setCursor(0, 1);
  lcd.print(id);
  delay(500);
  // Second Finger Scan
  lcd.clear();
  lcd.print("Place again");

  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        lcd.setCursor(0, 1);
        lcd.print("Image taken");
        delay(1000);
        break;
      case FINGERPRINT_NOFINGER:
        lcd.setCursor(0, 1);
        lcd.print("Waiting...");
        Serial.println("Enrolling operation failed");
        delay(500);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.setCursor(0, 1);
        lcd.print("Comm error");
        Serial.println("Enrolling operation failed");
        delay(2000);
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.setCursor(0, 1);
        lcd.print("Imaging error");
        Serial.println("Enrolling operation failed");
        delay(2000);
        break;
      default:
        lcd.setCursor(0, 1);
        lcd.print("Unknown error");
        Serial.println("Enrolling operation failed");
        delay(2000);
        break;
    }
  }

  p = finger.image2Tz(2);
  lcd.clear();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.print("Converted");
      delay(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image messy");
      Serial.println("Enrolling operation failed");
      delay(2000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Comm error");
      Serial.println("Enrolling operation failed");
      delay(2000);
      return p;
    case FINGERPRINT_FEATUREFAIL:
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Feature error");
      Serial.println("Enrolling operation failed");
      delay(2000);
      return p;
    default:
      lcd.print("Unknown error");
      delay(2000);
      return p;
  }

  // Create a model with both scans
  lcd.clear();
  lcd.print("creating");
  lcd.setCursor(0, 1);
  lcd.print("model");
  delay(500);

  p = finger.createModel();
  lcd.clear();
  if (p == FINGERPRINT_OK) {
    lcd.setCursor(0, 0);
    lcd.print("Prints matched!");
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.setCursor(0, 1);
    lcd.print("Comm error");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.setCursor(0, 1);
    lcd.print("Mismatch");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Unknown error");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  }

  // Store the fingerprint model
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stored!");
    Serial.println("Enrolling operation success");
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.setCursor(0, 1);
    lcd.print("Comm error");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.setCursor(0, 1);
    lcd.print("Bad location");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.setCursor(0, 1);
    lcd.print("Flash error");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Unknown error");
    Serial.println("Enrolling operation failed");
    delay(2000);
    return p;
  }
  return true;
}

uint8_t getFingerprintIDez() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting...");

    uint8_t p = finger.getImage();
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
            case FINGERPRINT_OK:
                Serial.println("Image taken");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Image taken");
                delay(500);
                break;
            case FINGERPRINT_NOFINGER:
                Serial.println("No finger detected");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("waiting");
                lcd.setCursor(0, 1);
                lcd.print(".");
                delay(200); // Avoid rapid display refresh
                continue; // Important to keep checking until a finger is placed
            case FINGERPRINT_PACKETRECIEVEERR:
                Serial.println("Communication error");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Comm error");
                delay(2000);
                return p;
            case FINGERPRINT_IMAGEFAIL:
                Serial.println("Imaging error");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Image error");
                delay(2000);
                return p;
            default:
                Serial.println("Unknown error");
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Unknown error");
                delay(2000);
                return p;
        }
    }

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) {
        Serial.println("Failed to convert image");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Conv fail");
        delay(2000);
        return p;
    }

    p = finger.fingerFastSearch();
    if (p == FINGERPRINT_OK) {
        Serial.print("Match found! ID #");
        Serial.print(finger.fingerID);
        Serial.print(" with confidence of ");
        Serial.println(finger.confidence);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ID: ");
        lcd.print(finger.fingerID);
        lcd.setCursor(0, 1);
        delay(2000);
        lcd.print("Confidence: ");
        lcd.print(finger.confidence);
    } else if (p == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No match");
        delay(2000);  // Display result for 2 seconds
    } else {
        Serial.println("Error searching");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Search error");
        delay(2000);
    }

    return p;
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

