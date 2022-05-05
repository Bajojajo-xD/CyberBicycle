#include <IRremote.h>

#define RECV_PIN A1

IRrecv irrecv(RECV_PIN);
decode_results results;

long
S11_onOffBtn = 16753245, S11_menuBtn = 16769565, 
S11_testBtn = 16720605, S11_plusBtn = 16712445, S11_backBtn = 16761405, 
S11_prevBtn = 16769055, S11_startStopBtn = 16754775, S11_nextBtn = 16748655, 
S11_0btn = 16738455, S11_minusBtn = 16750695, S11_clearBtn = 16756815, 
S11_1btn = 16724175, S11_2btn = 16718055, S11_3btn = 16743045,
S11_4btn = 16716015, S11_5btn = 16726215, S11_6btn = 16734885,
S11_7btn = 16728765, S11_8btn = 16730805, S11_9btn = 16732845;
// IR remote buttons, !! RUN "Helpers/IR-check" to get values !!
bool
on = false, start = false;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    if(results.value == S11_onOffBtn) {
      if(on){
        Serial.print("Turning off\n");
        on = false; 
      }  
      else {
        Serial.print("Turning on\n");
        on = true;  
      }
    }
    else if (results.value == S11_menuBtn) {
      Serial.print("Menu\n");      
    }
    else if (results.value == S11_testBtn) {
      Serial.print("Test\n");      
    }
    else if (results.value == S11_plusBtn) {
      Serial.print("+\n");      
    }
    else if (results.value == S11_backBtn) {
      Serial.print("Back\n");      
    }
    else if (results.value == S11_prevBtn) {
      Serial.print("Previous\n");      
    }
    else if (results.value == S11_startStopBtn) {
      if(start){
        Serial.print("Stopped\n");
        start = false;    
      }  
      else {
        Serial.print("Playing\n");
        start = true;  
      }   
    }
    else if (results.value == S11_nextBtn) {
      Serial.print("Next\n");      
    }
    else if (results.value == S11_0btn) {
      Serial.print("0\n");      
    }
    else if (results.value == S11_minusBtn) {
      Serial.print("-\n");      
    }
    else if (results.value == S11_clearBtn) {
      Serial.print("Clear\n");      
    }
    else if (results.value == S11_1btn) {
      Serial.print("1\n");      
    }
    else if (results.value == S11_2btn) {
      Serial.print("2\n");      
    }
    else if (results.value == S11_3btn) {
      Serial.print("3\n");      
    }
    else if (results.value == S11_4btn) {
      Serial.print("4\n");      
    }
    else if (results.value == S11_5btn) {
      Serial.print("5\n");      
    }
    else if (results.value == S11_6btn) {
      Serial.print("6\n");      
    }
    else if (results.value == S11_7btn) {
      Serial.print("7\n");      
    }
    else if (results.value == S11_8btn) {
      Serial.print("8\n");      
    }
    else if (results.value == S11_9btn) {
      Serial.print("9\n");      
    };
    irrecv.resume(); // Receive the next value
  }
}