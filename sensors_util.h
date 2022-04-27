// read sanitizer
void readSanitizerLevel() {
  digitalWrite(TRIG,LOW);
  delayMicroseconds(2000);
  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  float echoTime = pulseIn(ECHO,HIGH);
  float distance = (SOUND_SPEED * echoTime) / 2;
  sanitizerLevel = map(distance,0,15,0,100);
  if(sanitizerLevel <= 30) {
    levelLow = true;
    postNotification = true;
  } else {
    levelLow = false;
    bool lowPriorityPosted = false;
    bool highPriorityPosted = false;
  }
}
// change laser state
void toggleLaser(int state) {
  digitalWrite(LSR_PIN,state);
}
// change pump state
void togglePump(int state) {
  digitalWrite(MTR_PIN, state);
}
// detect presense or absence of hand
void detectHand() {
  float laserIntensity = analogRead(LDR_PIN);
  if(!isDispensing && (millis() >= dispenseTimeout)) {
    if(laserIntensity < LDR_THR) {
      isDispensing = true;
      dispenseTimeout = millis() + dispenseInterval;
    }
  }
  if(isDispensing) {
    if(laserIntensity >= LDR_THR) {
      usageCount++;
      isDispensing = false;
      postUsageCount = true;
      dispenseTimeout = 0L;
    }
  }
}
// control dispensing system
void controlDispenser() {
  if(isDispensing && (millis() <= dispenseTimeout)) {
    togglePump(HIGH);
  } else {
    togglePump(LOW);
  }
}