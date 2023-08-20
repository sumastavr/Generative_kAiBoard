

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

bool isToFinitialized=false;

#define LOW_THRESHOLD_DETECTION     100
#define HIGH_THRESHOLD_DETECTION    400


void initTof(){
    Wire.begin();
    //Wire.setClock(400000); //Sensor has max I2C freq of 400kHz 
    
    Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
    if (myImager.begin() == false){
        Serial.println(F("Sensor not found - check your wiring. Freezing"));
        isToFinitialized=false;
    }else{
        isToFinitialized=true;
    }
    
    myImager.setResolution(8*8); //Enable all 64 pads
    
    imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width

    
}

bool scanToF(){
    int triggeredPixelCounter;

    myImager.startRanging();
    delay(500);
    if (myImager.isDataReady() == true){
        if (myImager.getRangingData(&measurementData)){ //Read distance data into array
        //The ST library returns the data transposed from zone mapping shown in datasheet
        //Pretty-print data with increasing y, decreasing x to reflect reality
        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth){
            for (int x = imageWidth - 1 ; x >= 0 ; x--){
                Serial.print("\t");
                int currentReading=measurementData.distance_mm[x + y];
                if (currentReading>LOW_THRESHOLD_DETECTION && currentReading<HIGH_THRESHOLD_DETECTION){
                    triggeredPixelCounter++;
                }

                if(triggeredPixelCounter>20){
                    Serial.println("Object Detected");
                    myImager.stopRanging();
                    return true;
                }

                if(currentReading>2000){
                    Serial.print("X");
                }else{
                    Serial.print(currentReading);
                }
            }
            Serial.println();
        }
        Serial.println();
        }
    }
    myImager.stopRanging();
    return false;
}