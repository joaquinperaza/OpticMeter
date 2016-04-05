
#include <Wire.h>
#include "PX4Flow.h"

#define Long_Focal_MM 16

#define LED 13
//variables 
long last_data = 0;
int px = 0;
int py = 0;
int px2 = 0;
int py2 = 0; 
int rele = 12;
float distancia;
int dosis = 3;
float focal_length_px = (Long_Focal_MM) / (4.0f * 6.0f) * 1000.0f;
float z_rate;//cabeceras
boolean giro;
  float max_giro = 1.4; 


// Initialize PX4Flow library
PX4Flow sensor = PX4Flow(); 

void setup()
{
 
  pinMode(LED, OUTPUT); 
pinMode(rele, OUTPUT)  ;
  
 
  Wire.begin();       
 
  Serial.begin(115200);  
}

void loop()
{
  long loop_ini = millis();
  
  while (Serial.available() > 0) {
    if (Serial.read() == 'r') {
      Serial.println("Reseteado");
      px = 0;
      py = 0;
    }
  }
  
  
  
  if (loop_ini - last_data > 100) {
    // Fetch I2C data  
    sensor.update_integral();
    float x_rate = sensor.gyro_x_rate_integral() / 10.0f;       // mrad
    float y_rate = sensor.gyro_y_rate_integral() / 10.0f;      // mrad
    float flow_x = sensor.pixel_flow_x_integral() / 10.0f;      // mrad
    float flow_y = sensor.pixel_flow_y_integral() / 10.0f;      // mrad  
    int timespan = sensor.integration_timespan();               // microseconds
    int ground_distance = sensor.ground_distance_integral();    // mm
    int quality = sensor.quality_integral();
    
    if (quality > 100)
    {
      // Update flow rate with gyro rate
      float pixel_x = flow_x + x_rate; // mrad
      float pixel_y = flow_y + y_rate; // mrad
      
      // Scale based on ground distance and compute speed
      // (flow/1000) * (ground_distance/1000) / (timespan/1000000)
      float velocity_x = pixel_x * ground_distance / timespan;     // m/s
      float velocity_y = pixel_y * ground_distance / timespan;     // m/s 
      
      //determinar si esta girando
     z_rate = sensor.gyro_y_rate_integral() / 10.0f / timespan;      // radianes por segundo
      if ( z_rate > max_giro )
      {
        giro = true;
      }
      else
      {
        giro = false;
      }
      
      // Integrate velocity to get pose estimate
      px = px + velocity_x * 100;
      py = py + velocity_y * 100;
      
      //calculo de distancia
      if (giro = false);
      {
      distancia += sqrt(((px2-px)*(px2-px))-((py2-py)*(py2-py)));
      } 
           //reseteo de variacion xy
      px2= px;
      py2= py;
      
      
      
      
      
      //aplicador
      if (distancia > dosis)
      {
      digitalWrite(rele, HIGH);   // aplica
  delay(1000);                  // tiempo
  digitalWrite(rele, LOW); 
Serial2.print("frase de aplicacion") ; //cierra
    distancia=0;  
    }
           
            
 
    }
    else {
      Serial.println(".");
    }
    
    last_data = loop_ini;
  }
}

