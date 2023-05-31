#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Servo.h>

#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
#define LIS3DH_CS 10

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

Servo Servo_x;
Servo Servo_y;

int pos_x = 0;
int pos_y = 0;

float accel_angle_x;
float accel_angle_y;

const int ARRAY_SIZE = 15;

// X-värde
int pos_x_history[ARRAY_SIZE] = {0};
int current_index_x = 0;

// y-värde
int pos_y_history[ARRAY_SIZE] = {0};
int current_index_y = 0;

void setup(void) {
  // Servo
  Servo_x.attach(9);
  Servo_y.attach(10);
  
  Serial.begin(115200);
  while (!Serial) 
    delay(10);
//test om acceleromentern är kopplad rätt
  Serial.println("Accelerometer test!");

  if (! lis.begin(0x18)) {
    Serial.println("Couldnt start");
    while (1) 
      yield();
  }
  Serial.println("Accelerometer found!");

  // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); 
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;
    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }
}

void loop() {
  lis.read();      // get X Y and Z data at once

  sensors_event_t event;
  lis.getEvent(&event);
  value_of_xyz();

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");

  Serial.println();
}

void value_of_xyz() {
    // Read sensor data from accelerometer
    sensors_event_t event;
    lis.getEvent(&event);

    // Calculate x and y angles from accelerometer data
    float x_val = event.acceleration.x;
    float y_val = event.acceleration.y;
    float z_val = event.acceleration.z;

    // Calculate squares of accelerometer values
    float x2 = x_val * x_val;
    float y2 = y_val * y_val;
    float z2 = z_val * z_val;

    // Calculate x angle
    float result = sqrt(y2 + z2);
    float accel_angle_x = atan(x_val / result) * 180 / PI;
    float pos_x = map(accel_angle_x, -90, 90, 0, 180);
    Serial.print("\tServo X: ");
    Serial.print(pos_x);

    // Add pos_x to history array and calculate average value
    pos_x_history[current_index_x] = pos_x;
    current_index_x = (current_index_x + 1) % ARRAY_SIZE;
    int sum_x = 0;
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum_x += pos_x_history[i];
    }
    float average_x = (float) sum_x / ARRAY_SIZE;

    // Ger medelvärdet till servo för x axeln
    Servo_x.write(average_x);

    // Calculate y angle
    result = sqrt(x2 + z2);
    float accel_angle_y = atan(y_val / result) * 180 / PI;
    float pos_y = map(accel_angle_y, -90, 90, 0, 180);
    Serial.print("\tServo Y: ");
    Serial.print(pos_y);

    // Add pos_y to history array and calculate average value
    pos_y_history[current_index_y] = pos_y;
    current_index_y = (current_index_y + 1) % ARRAY_SIZE;
    int sum_y = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum_y += pos_y_history[i];
    }
    float average_y = (float) sum_y / ARRAY_SIZE;

    // Ger medelvärdet till servo2 för y axeln
    Servo_y.write(average_y);
}
