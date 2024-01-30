int UltraPres(int desactivation, int memoire, int distance) {
  int mesure = ultrasonic.read();
  if (desactivation == 1){
    if ((mesure <= distance - 10 || mesure >= distance + 10) && mesure != 357 && mesure != 0){
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Intrusion!!");
      distance = ultrasonic.read(); // intrusion
      client.publish("maison1/garage/presence", String(1).c_str());
      memoire = 1;
      // buzzer
      for (int x = 3000; x < 20 ; x += 100){
        tone (pin_buzz, x,10);
        delay(10);
      }
      noTone(pin_buzz);
    } else {
      client.publish("maison1/garage/presence", String(0).c_str());
      memoire = 0;
    }
  } else {
    if (memoire == 1){
      client.publish("maison1/garage/presence", String(1).c_str());
    } else {
      if ((mesure <= distance - 10 || mesure >= distance + 10) && mesure != 357 && mesure != 0){
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Intrusion!!");
        distance = ultrasonic.read(); // intrusion
        client.publish("maison1/garage/presence", String(1).c_str());
        memoire = 1;
        //buzzer
        for (int x = 3000; x < 20 ; x += 100){
          tone (pin_buzz, x,10);
          delay(10);
        }
        noTone(pin_buzz);
      } else {
        client.publish("maison1/garage/presence", String(0).c_str());
      }
    }
  }
  return memoire;
}