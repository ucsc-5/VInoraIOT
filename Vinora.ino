void setup() {
  Serial.begin(115200);
  setup_display();
  setup_LoadCell();


}

void loop() {
  loop_LoadCell();
  
}
