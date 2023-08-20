
void buzzMotor(int repeat,int duration){
    for (int i=0;i<repeat;i++){
        digitalWrite(PORT_VIB_MOT_L,HIGH);
        digitalWrite(PORT_VIB_MOT_R,HIGH);
        delay(duration/2);
        digitalWrite(PORT_VIB_MOT_L,LOW);
        digitalWrite(PORT_VIB_MOT_R,LOW);
        delay(duration/2);
    }
}