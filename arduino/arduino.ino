#include "Actuator.h"
#include "Knob.h"
#include "Types.h"

const distance ACTUATOR_POS_ACCURACY = 10; // mm

Actuator actuatorLeft(
);

Actuator actuatorRight(
);

Knob targetPosKnob();

void setup() {
}

void loop() {
  float targetPosRatio = targetPosKnob.readTargetPosRatio();
  actuatorLeft.goTo(targetPosRatio);
  actuatorRight.goTo(targetPosRatio);
}
