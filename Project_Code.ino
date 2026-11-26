#include <Wire.h>               // For I2C communication
#include <LiquidCrystal_I2C.h>  // For I2C LCD display
#include <DHT.h>                // For DHT11 sensor

// Pin Definitions
#define MOTOR_PIN1 2     // DC motor driver input 1
#define MOTOR_PIN2 3     // DC motor driver input 2
#define SERVO_PIN 9      // Servo motor pin
#define DHT_PIN 7        // DHT11 sensor data pin

// Initialize LCD display (I2C address: 0x27, 16x2 LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize DHT11 sensor
DHT dht(DHT_PIN, DHT11);

// Servo control variables
int servoPosition = 64; // Servo starts at 64°

// Received signal from Bluetooth
char receivedSignal;

// Timing variables
unsigned long previousDHTReadTime = 0;
unsigned long previousServoUpdateTime = 0;
const unsigned long DHTReadInterval = 2000; // Read DHT every 2 seconds
const unsigned long servoPulseInterval = 20; // Servo pulse interval (~20ms)

void setup() {
  // Set motor pins as output
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);

  // Set servo pin as output
  pinMode(SERVO_PIN, OUTPUT);

  // Initialize Serial communication
  Serial.begin(9600);
  // Serial.println("System Initialized. Waiting for commands...");

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize LCD display
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  // Display Intro Slide 1
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome To IoT");
  lcd.setCursor(0, 1);
  lcd.print("World");
  delay(2000);

  // Display Slide 2
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("By : Tanay And");
  lcd.setCursor(0, 1);
  lcd.print("Jayraj");
  delay(2000);

  // Display Slide 3
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Incharge :");
  lcd.setCursor(0, 1);
  lcd.print("Ajay Nishad");
  delay(2000);

  // Display Slide 4
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("From Alchemy");
  lcd.setCursor(0, 1);
  lcd.print("School");
  delay(2000);

  // Display Slide 5
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Borewell Rescue");
  lcd.setCursor(0, 1);
  lcd.print("Robot");
  delay(2000);
  // lcd.print("System Started");

  // Set servo to initial position (64°)
  servoPosition = 64; // Start position
}

void loop() {
  // Handle DHT11 sensor reading at regular intervals
  if (millis() - previousDHTReadTime >= DHTReadInterval) {
    readAndDisplayDHT();
    previousDHTReadTime = millis();
  }

  // Check if data is received from the Bluetooth module
  if (Serial.available() > 0) {
    receivedSignal = Serial.read(); // Read the incoming signal
    // Serial.print("Received Command: ");
    Serial.println(receivedSignal); // Print received signal for debugging

    // Control DC Motor
    handleDCMotor();

    // Control Servo Motor
    handleServoMotor();
  }

  // Continuously generate PWM signal for the servo
  generateServoPWM(servoPosition);
}

// Function to read and display temperature and humidity from the DHT11 sensor
void readAndDisplayDHT() {
  float temperature = dht.readTemperature(); // Celsius
  float humidity = dht.readHumidity();       // Percentage

  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Read Error");
    // Serial.println("Error reading from DHT sensor!");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }
}

// Function to control the DC motor
void handleDCMotor() {
  if (receivedSignal == 'F') {
    digitalWrite(MOTOR_PIN1, HIGH);
    digitalWrite(MOTOR_PIN2, LOW);
    // Serial.println("Motor: Clockwise");
  } else if (receivedSignal == 'B') {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, HIGH);
    // Serial.println("Motor: Counterclockwise");
  } else if (receivedSignal == '0') {
    digitalWrite(MOTOR_PIN1, LOW);
    digitalWrite(MOTOR_PIN2, LOW);
    // Serial.println("Motor: Stopped");
  }
}

// Function to control the servo motor
void handleServoMotor() {
  if (receivedSignal == 'S') {
    if (servoPosition < 180) {
      servoPosition += 10;
      // Serial.print("Servo: Clockwise, Position: ");
      // Serial.println(servoPosition);
    }
  } else if (receivedSignal == 'C') {
    if (servoPosition > 0) {
      servoPosition -= 10;
      // Serial.print("Servo: Counterclockwise, Position: ");
      // Serial.println(servoPosition);
    }
  } else if (receivedSignal == 'P') {
    servoPosition = 70;
    // Serial.println("Servo: Moved to 70°");
  } else if (receivedSignal == 'A') {
    servoPosition = 25;
    // Serial.println("Servo: Moved to 25°");
  }
}

// Function to generate the PWM signal for the servo
void generateServoPWM(int position) {
  int pwmValue = map(position, 0, 180, 544, 2400); // Map angle to PWM signal
  digitalWrite(SERVO_PIN, HIGH);
  delayMicroseconds(pwmValue);
  digitalWrite(SERVO_PIN, LOW);
  delay(20 - pwmValue / 1000); // Ensure a 20ms frame length
}
