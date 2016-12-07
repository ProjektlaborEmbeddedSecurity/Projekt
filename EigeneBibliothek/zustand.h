#ifndef ZUSTAND_H
#define ZUSTAND_H

//byte Zustand = 0;
enum Zustand {AllesOk, FeOZuWarm, FeOZuKalt, FeOZuWarmZuFeucht, FeOZuKaltZuFeucht, FeOZuWarmZuTrocken, FeOZuKaltZuTrocken, FeOZuFeucht, FeOZuTrocken,FeZZuWarm, FeZZuKalt, FeZZuWarmZuFeucht, FeZZuKaltZuFeucht, FeZZuWarmZuTrocken, FeZZuKaltZuTrocken, FeZZuFeucht, FeZZuTrocken };

byte temp_up_thr = 30; //[C°]
byte temp_down_thr = 2; //[C°]
byte humidity_up_thr = 70; //[%]
byte humidity_down_thr = 20;//[%]

int thrDoor = 30; //[cm]

int ZustandTuerStatus (int cm);
int Zustandsmaschine(int offen, int temperature, int humidity);

int ZustandTuerStatus (int cm)
{
	int offen;
	//Tür offen
	if(cm > thrDoor)
	{
		offen = 1;
	}
	else{
		offen = 0;
		//Zustand = FensterZu;
	}
	return offen;
}

int Zustandsmaschine(int offen, int temperature, int humidity){
   
 int Zustand = AllesOk;  
 if (offen == 1){
    //TEMP
    if (temp_up_thr < temperature)
        Zustand = FeOZuWarm; //zu warm (und feuchtigkeit ok)
	else if (temp_down_thr > temperature)
        Zustand = FeOZuKalt; //zu kalt (und feuchtigkeit ok)
    
    //Humid
   if (humidity_up_thr < humidity){
      if(Zustand == FeOZuWarm)
        Zustand = FeOZuWarmZuFeucht; //zu warm & zu feucht
      else if (Zustand == FeOZuKalt)
          Zustand = FeOZuKaltZuFeucht; //zu kalt & zu feucht
	  else
          Zustand = FeOZuFeucht; //nur zu feucht
	}
	else if(humidity_down_thr > humidity) {
       if (Zustand == FeOZuWarm)
         Zustand = FeOZuWarmZuTrocken; //zu warm & zu trocken
       else if (Zustand == FeOZuKalt)
          Zustand = FeOZuKaltZuTrocken; //zu kalt & zu trocken
	   else
           Zustand = FeOZuTrocken; //nur zu trocken
    }
  }
  else{ //fenster zu
    //TEMP
    if (temp_up_thr < temperature)
        Zustand = FeZZuWarm; //zu warm (und feuchtigkeit ok)
    else if (temp_down_thr > temperature)
        Zustand = FeZZuKalt; //zu kalt (und feuchtigkeit ok)
    
    //Humid
	if (humidity_up_thr < humidity){
      if(Zustand == FeZZuWarm)
        Zustand = FeZZuWarmZuFeucht; //zu warm & zu feucht
      else if (Zustand == FeZZuKalt)
          Zustand = FeZZuKaltZuFeucht; //zu kalt & zu feucht
      else
          Zustand = FeZZuFeucht; //nur zu feucht
	}
	else{
      if(humidity_down_thr > humidity) {
       if (Zustand == FeZZuWarm)
         Zustand = FeZZuWarmZuTrocken; //zu warm & zu trocken
       else if (Zustand == FeZZuKalt)
          Zustand = FeZZuKaltZuTrocken; //zu kalt & zu trocken
       else
           Zustand = FeZZuTrocken; //nur zu trocken
      }
    }  
  } 
  return Zustand;
}

#endif
