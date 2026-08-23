// Based on the PS2X_Example sketch from the Arduino-PS2X library (GPL v3):
//   https://github.com/madsci1016/Arduino-PS2X
// Library by Bill Porter (http://www.billporter.info/),
// initial PS2 controller code by Shutter (Arduino Forum).
// Modified: ESP32-C3 pin mapping, L298N wheel motor control, pickup roller (TT motor) control.

#include <PS2X_lib.h>  // for v1.6

/******************************************************************
 * ESP32-C3 pin assignment:
 *   PS2 receiver: CLK -> GPIO4, CMD -> GPIO6, SEL -> GPIO5, DAT -> GPIO7
 *                 VCC -> 3.3V (NOT 5V!), GND -> GND
 *   Motors (L298N): left  = IN1(GPIO0) / IN2(GPIO1)
 *                   right = IN3(GPIO20) / IN4(GPIO21)
 *   TT motor -> GPIO3 (needs MOSFET/relay driver)
 *   GPIO20/21 are UART0 TX/RX - free only with "USB CDC On Boot" enabled
 * Avoid strapping pins (GPIO2/8/9), USB D-/D+ (GPIO18/19),
 * flash pins (GPIO11-17).
 ******************************************************************/

#define PS2_CLK        4
#define PS2_CMD        6
#define PS2_SEL        5
#define PS2_DAT        7

#define MOTOR_IN1      0
#define MOTOR_IN2      1
#define MOTOR_IN3      20  // UART0 pin - free only when "USB CDC On Boot" is enabled
#define MOTOR_IN4      21  // UART0 pin - free only when "USB CDC On Boot" is enabled
#define MOTOR3_PIN     3   // TT motor, needs its own driver (MOSFET or relay module)

// left motor = IN1/IN2, right motor = IN3/IN4 (L298N style driver)
#define MOTOR_L_A      MOTOR_IN1
#define MOTOR_L_B      MOTOR_IN2
#define MOTOR_R_A      MOTOR_IN3
#define MOTOR_R_B      MOTOR_IN4

// set motor speed: -255..255 (negative = reverse)
void setMotor(int pinA, int pinB, int speed) {
  if (speed > 0) {
    analogWrite(pinA, speed);
    analogWrite(pinB, 0);
  } else if (speed < 0) {
    analogWrite(pinA, 0);
    analogWrite(pinB, -speed);
  } else {
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
  }
}

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-buttons
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   false
#define rumble      false

PS2X ps2x;  // create PS2 Controller Class

// right now, the library does NOT support hot pluggable controllers, meaning
// you must always either restart your Arduino after you connect the controller,
// or call config_gamepad(pins) again after connecting the controller.

int error = -1;
byte type = 0;
byte vibrate = 0;
int tryNum = 1;

void setup() {

  Serial.begin(115200);

  pinMode(MOTOR_L_A, OUTPUT);
  pinMode(MOTOR_L_B, OUTPUT);
  pinMode(MOTOR_R_A, OUTPUT);
  pinMode(MOTOR_R_B, OUTPUT);
  pinMode(MOTOR3_PIN, OUTPUT);

  // added delay to give wireless ps2 module some time to startup, before configuring it
  while (error != 0) {
    delay(1000);  // 1 second wait
    // setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT,
                                pressures, rumble);
    Serial.print("#try config ");
    Serial.println(tryNum);
    Serial.print("error code: ");
    Serial.println(error);
    tryNum ++;
  }

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println(" Unknown Controller type found ");
      break;
    case 1:
      Serial.println(" DualShock Controller found ");
      break;
    case 2:
      Serial.println(" GuitarHero Controller found ");
      break;
    case 3:
      Serial.println(" Wireless Sony DualShock Controller found ");
      break;
  }
}

void loop() {

  if (type == 1 || type == 3) {  // DualShock (wired) or Wireless Sony DualShock
    ps2x.read_gamepad(false, vibrate);  // read controller and set large motor to spin at 'vibrate' speed

    // --- wheel motors: left stick -> left wheel, right stick -> right wheel ---
    // stick Y: 0 = fully down, 128 = center, 255 = fully up
    int spdL = constrain((ps2x.Analog(PSS_LY) - 128) * 2, -255, 255);
    int spdR = constrain((ps2x.Analog(PSS_RY) - 128) * 2, -255, 255);
    if (abs(spdL) < 20) spdL = 0;  // deadzone
    if (abs(spdR) < 20) spdR = 0;

    setMotor(MOTOR_L_A, MOTOR_L_B, spdL);
    setMotor(MOTOR_R_A, MOTOR_R_B, spdR);

    // --- third motor (TT motor): hold L1 to run ---
    if (ps2x.Button(PSB_L1)) {
      digitalWrite(MOTOR3_PIN, HIGH);
    } else {
      digitalWrite(MOTOR3_PIN, LOW);
    }

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 250) {
      lastPrint = millis();
      Serial.print("L=");
      Serial.print(spdL);
      Serial.print("  R=");
      Serial.print(spdR);
      Serial.print("  M3=");
      Serial.println(ps2x.Button(PSB_L1) ? "ON" : "OFF");
    }

    // will be TRUE as long as button is pressed
    if (ps2x.Button(PSB_START))
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    // will be TRUE as long as button is pressed
    if (ps2x.Button(PSB_PAD_UP)) {
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if (ps2x.Button(PSB_PAD_RIGHT)) {
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if (ps2x.Button(PSB_PAD_LEFT)) {
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }

    vibrate = ps2x.Analog(PSAB_CROSS);  // this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        // will be TRUE if any button changes state (on to off, or off to on)
      if (ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if (ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if (ps2x.Button(PSB_L2))
        Serial.println("L2 pressed");
      if (ps2x.Button(PSB_R2))
        Serial.println("R2 pressed");
      if (ps2x.Button(PSB_TRIANGLE))
        Serial.println("△ pressed");
    }

    // △□○×
    if (ps2x.ButtonPressed(PSB_CIRCLE))   // will be TRUE if button was JUST pressed
      Serial.println("○ just pressed");
    if (ps2x.NewButtonState(PSB_CROSS))   // will be TRUE if button was JUST pressed OR released
      Serial.println("× just changed");
    if (ps2x.ButtonReleased(PSB_SQUARE))  // will be TRUE if button was JUST released
      Serial.println("□ just released");

    if (ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) {  // print stick values if either is TRUE
      Serial.print("Stick Values:");
      Serial.print(ps2x.Analog(PSS_LY));  // Left stick, Y axis. Other options: LX, RY, RX
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_LX), DEC);
      Serial.print(",");
      Serial.print(ps2x.Analog(PSS_RY), DEC);
      Serial.print(",");
      Serial.println(ps2x.Analog(PSS_RX), DEC);
    }
  }
  delay(50);
}
