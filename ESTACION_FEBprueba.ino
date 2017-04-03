// LIBREARIAS NECESARIAS PARA LA INTERPRETACION DE LAS LECTURAS
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include "DHT.h"
#include <SFE_BMP180.h>

//**********************************************************************************************
//Inicialización de variables Y asignación de I/O para sensores
//**********************************************************************************************
 #define DHTPIN 4                                                         //Definición de la entrada digital en la que se conectará 
                                                                          //el sensor de tempertatura y humedad
 #define DHTTYPE DHT22                                                   // DHT 22  (AM2302), AM2321
 DHT dht(DHTPIN, DHTTYPE);

SFE_BMP180 pressure;
  const int chipSelect = 10;                                              //Pin digital reservado para la comunicación con el shield SD
  const int pinhumedad=0; int pinluz=1; int pinCO2=2; int pinCO=3;       //Entrada análoga a la que se conectarán los sensores
  const int lec_seca=0; int lec_agua=800;                               //Lecturas superior e inferior, son particulares para cada sensor
                                                                        //estas deben de sensarse a priori y ponerse en esta línea de código
  int i,ii,j;
  const int lecturas=10;       // número de mediciones que queremos hacer para obtener cada valor promedio  
  //*********************************************Humedad******************************************
    float pendiente=-100./(lec_seca-lec_agua);      //Se supone una respuesta lineal de la resistencia del sensor con respecto a la presencia de 
    float ordenada=0.;                              //humedad, por lo tanto definimos una pendiente y una ordena al origen.
                                                    //(Una lectura de 0 debería corresponder al sensor sumergido en agua)

   //******************************************Incialización para los promedios*****************************************
    float Humidity_prom; float porcentaje_prom;float t_prom;float hic_prom;float luz_prom;float CO2_prom;float CO_prom;
    float TP_prom;float presion_prom;
    int hora_h;int hora_m;int hora_s;int fecha_d;int fecha_m;int fecha_a;

void setup() {
  // Abrir la comunicacion serial y esperar hasta que esto se cumpla:
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
//   if (pressure.begin())
//      Serial.println("SENSOR DE PRESION AL TIRO");
//    else
//    {
//      Serial.println("FALLO EN EL SENSOR DE PRESION CHECAR CONEXION (SDA-PIN4; SCL-PIN5");
//      //while(1); // Pause forever.
//    }

  Serial.print("Inicializando tarjeta SD ...");

  // Ver si la tarjeta esta presente para inicializarla
   if (!SD.begin(chipSelect)) {
    Serial.println("Error en tarjeta o Tarjeta no presente"); //Un leeeeddddddd
    return;
  }
  Serial.println("Tarjeta al 100 ;)");
 dht.begin();
}
  void loop() {
    tmElements_t tm;
    String dataString = "";  //Inicialización de la cadena de caracteres donde se imprimiran las lecturas
    char status;
    double TP,PRESION;
    //******************************************Incialización para los promedios*****************************************
      Humidity_prom=0; porcentaje_prom=0; t_prom=0; hic_prom=0; luz_prom=0;CO2_prom=0;CO_prom=0;
      presion_prom=0;TP_prom=0;
    //***********************************************************************************************
    //Ciclo principal controlado por el número de mediciones que queremos hacer para obtener cada valor promedio
    //***********************************************************************************************
    
    for (int i=0; i <=lecturas-1; i=i+1){
        delay(3000); //********************************Tiempo entre lecturas, recordar que los 
                     //***************************sensores tienen un tiempo mínimo de respuesta

//***************************Presión, altitud y temperatura (BMP180)************************************** 
    //********************************************************************************************************
//             status = pressure.startTemperature();
//        if (status != 0)
//        {
////          // ESPERA MIENTRAS SE COMPLETA LA MEDICION
//          delay(status);
////      
////          // LA FUNCION STATUS REGRESA 1 SI HAY MEDICION O CERO EN CASO DE QUE HAYA FALLOS
//          status = pressure.getTemperature(TP);
//          if (status != 0)
//          {
//            TP_prom=TP_prom+TP; 
//            delay(4000);           
//            status = pressure.startPressure(3);
//            if (status != 0)
//            {
//              delay(status);
//              status = pressure.getPressure(PRESION,TP);
//              if (status != 0) presion_prom=presion_prom+PRESION; 
//              else Serial.println("error AL OBTENER LA MEDICION DE PRESION\n");
//            }
//            else Serial.println("error AL COMENZAR LA MEDICION DE PRESION\n");
//          }
//          else Serial.println("error AL OBTENER LA MEDICION DE TEMPERATUR\n");
//        }
//        else Serial.println("error AL COMENZAR LA MEDICION DE TEMPERATURA\n");   
    //***************************Temperatura & Humedad relativa************************************** 
    //***********************************************************************************************
    //El tiempo de respuesta del DHT 22 es de unos 2 segundos por lo que ese debe ser el intervalo 
    //mínimo entre mediciones
    delay(3000);
      float h = dht.readHumidity();
    // Lee la temp en grados Celsius (default)
      float t = dht.readTemperature();
    // Lee la temp en grados Fahrenheit (Fahrenheit = true)
      float f = dht.readTemperature(true);
    // Aquí se verifica si hay error, si lo hay se indica en el monitor
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Error al leer el sensor de temperatura/Humedad"); // Aqui prende un led!!!
      //return;
      }
    // Sensación térmica en Fahrenheit (the default)
     float hif = dht.computeHeatIndex(f, h);
    // Sensación térmica en Celsius (isFahreheit = false)
     float hic = dht.computeHeatIndex(t, h, false);

    //Serial.print("Humidity: ");Serial.print(h);Serial.print(" %\t");Serial.print("Temperature: ");Serial.print(t);Serial.print(" *C ");
    //Serial.print(f);Serial.print(" *F\t");Serial.print("Heat index: ");Serial.print(hic);Serial.print(" *C ");Serial.print(hif);Serial.println(" *F");
     Humidity_prom=Humidity_prom+h;t_prom=t_prom+t;hic_prom=hic_prom+hic;//f_prom=f_prom+f;hif_prom=hif_prom+hif;  
    //**************************Humedad********************************************************* 
    //***************************************************************************************** 
      int humiditySensor = analogRead(pinhumedad);
      float  porcentaje=ordenada+pendiente*(humiditySensor-lec_seca);
      if (porcentaje>100.) porcentaje=100.;
      if (porcentaje<0.) porcentaje=0.;
      //Serial.print(porcentaje);Serial.println(" %");
       porcentaje_prom=porcentaje_prom+porcentaje;

    //**************************Luz********************************************************* 
    //**************************************************************************************
      int Sensorluz=analogRead(pinluz);
       luz_prom=luz_prom+Sensorluz;

    //**************************CO2 (MQ135)    ******************************************** 
    //**************************************************************************************
      int SensorCO2=analogRead(pinCO2);
          CO2_prom=CO2_prom+SensorCO2;
    //**************************CO (MQ-7)    *********************************************** 
    //**************************************************************************************
      int SensorCO=analogRead(pinCO);
        CO_prom=CO_prom+SensorCO;
 
 ii=i+1;
 }  //TERMINA CICLO

    //**************************  CALCULO DE PROMEDIOS******************************************************* ** 
    //***********************************************************************************************************
     
     Humidity_prom=Humidity_prom/(ii);
     t_prom=t_prom/(ii);//f_prom=f_prom/(i-1);
     hic_prom=hic_prom/(ii);//hif_prom=hif_prom/(i-1);                       //Cálculo de promedios cada minuto con lecturas hechas cada 3 segundos
     porcentaje_prom=porcentaje_prom/(ii);luz_prom=luz_prom/(ii);
     CO2_prom=CO2_prom/(ii);CO_prom=CO_prom/(ii);
     presion_prom=presion_prom/(ii);TP_prom=TP_prom/(ii);
     
    //**************************COMUNICACIÓN SERIAL(OPCIONAL)*********************************************** 
    //******************************************************************************************************

//     Serial.print(Humidity_prom);Serial.print(" ");
//     Serial.print(t_prom);Serial.print(" ");Serial.print(hic_prom);Serial.print(" ");
//     Serial.print(porcentaje_prom);Serial.print(" ");Serial.print(luz_prom);Serial.print(" ");
//     Serial.print(CO2_prom);Serial.print(" ");Serial.print(CO_prom);Serial.print(" ");
//     Serial.print(presion_prom);Serial.print(" ");Serial.print(TP_prom);Serial.println(" ");

//     Serial.print(CO2_prom);Serial.print(" ");Serial.print(CO_prom);Serial.println(" ");
     int n=11;
     for (int ii = 1; ii <= n; ii++) {
        if (ii=1)dataString +=String(Humidity_prom);dataString += ",";
        if (ii=2)dataString +=String(t_prom); dataString += ",";if (ii=3)dataString +=String(hic_prom); dataString += ",";
        if (ii=4)dataString +=String(porcentaje_prom); dataString += ",";if (ii=5)dataString +=String(luz_prom); dataString += ",";
        if (ii=6)dataString +=String(CO2_prom); dataString += ","; if (ii=7)dataString +=String(CO_prom); dataString += ",";
        if (ii=8)dataString +=String(presion_prom); dataString += ","; if (ii=9)dataString +=String(TP_prom); dataString += ",";
//       //*********************************************************************************************************
//       //**********************************************Fecha y hora***********************************************
//       //*********************************************************************************************************
//       
//       // SI EL RELOJ DA LECTURA SE PROCEDE A AGREGAR HORA Y FECHA A LA CADENA DE DATOS DEL REGISTRO
        if (RTC.read(tm)) {          
           if (ii=10){
                if (tm.Hour >= 0 && tm.Hour < 10)dataString += "0"; 
                dataString += String(tm.Hour); dataString += ":";
                if (tm.Minute >= 0 && tm.Minute< 10)dataString += "0"; 
                  dataString += String(tm.Minute);dataString += ":";
                  if (tm.Second >= 0 && tm.Second< 10) dataString += "0"; 
                  dataString += String(tm.Second);dataString += ",";
           }
            if (ii=11){
                if (tm.Day >= 0 && tm.Day < 10)dataString += "0"; 
                dataString += String(tm.Day); dataString += "/";
                if (tm.Month >= 0 && tm.Month< 10)dataString += "0"; 
                  dataString += String(tm.Month);dataString += "/";
                  if (tmYearToCalendar(tm.Year) >= 0 && tmYearToCalendar(tm.Year)< 10) dataString += "0"; 
                  dataString += String(tmYearToCalendar(tm.Year));dataString += ",";
           }  
         
//       //*********************************************************************************************************
//       //*********************************************************************************************************
//       //*********************************************************************************************************
        }
        }
//
      // LOS DATOS SON ENVIADOS AL PUERTO SERIAL (SOLO POR COMPROBACION DE LECTURAS)
      Serial.println(dataString);
        
      //APERTURA DEL ARCHIVO PARA LOS DATOS
      File dataFile = SD.open("estacion.txt", FILE_WRITE);
    
      //SI EL ARCHIVO ESTA DISPONIBLE, SE ESCRIBE
      if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
      }
      // AVISA SI EL ARCHIVO NO PUDO SER ABIERTO
      else {
        Serial.println("Error abriendo el archivo estacion.txt");
      }
  
}



