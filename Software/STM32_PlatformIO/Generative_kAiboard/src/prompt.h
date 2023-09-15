#include <Arduino.h>

#define isDetected  !=-1

String promptClassName[]={"Myself","Work","Time","Location","Hobby","Statistics"};

String promptClassKeywords[6][20]={{"i","me","myself","who"},
                                  {"work","occupation","duty"},
                                  {"time","clock","today","week","month","year"},
                                  {"where","location","position","city","country","province"},
                                  {"hobby","fun","sports","activity","weekend","party"},
                                  {"user","state","work","control"}
                                  };

String promptClassContents[6]={ "My name is sumasta, I am the creator of Generative Kaiboard. ",
                                "I am currently working for Signify as a lead development engineer in Trulifi business unit since march 2020. ",
                                "Here's infomation about todays clock and date: ",
                                "Currently i am located in Eindhoven, the netherlands. "
                                "As a hobby I love playing soccer and badminton. "
                                "Here some real time statistics details about the usage of this generative kaiboard",
                              };

bool stateClassTriggered[6]={0,0,0,0,0,0};

String processPromptArray(String prompt){

  String copyPrompt=prompt;
  String appendedPrompt="";
  copyPrompt+=" ";
  copyPrompt.toLowerCase();
  
  for (int i=0;i<6;i++){
    if(stateClassTriggered[i]==false){
      for (int y=0;y<sizeof(promptClassKeywords[i]);y++){
          if(copyPrompt.indexOf(promptClassKeywords[i][y])isDetected){
            appendedPrompt+=promptClassContents[y];
            Serial.print("Prompt Added: ");
            Serial.println(promptClassName[y]);
            stateClassTriggered[i]=true;
            break;
          }
      }
    }
  }

  prompt=appendedPrompt+prompt;

  return prompt;

}

String processPrompt(String prompt){

  String copyPrompt=prompt;
  copyPrompt+=" ";
  copyPrompt.toLowerCase();
  
  if(copyPrompt.indexOf(" i ")!=-1 || copyPrompt.indexOf(" i")!=-1 ){
    prompt="FYI my name is sumasta. I am the creator of generative kaiboard. "+prompt;
    Serial.println("Prompt: Append Sumasta");
  }

  if(copyPrompt.indexOf(" work ")!=-1 || copyPrompt.indexOf(" signify ")!=-1 || copyPrompt.indexOf(" trulifi ")!=-1){
    prompt="I work for signify since 1st of march 2020 and located in Eindhoven as lead development engineer. "+prompt;
    Serial.println("Prompt: Work");
  }

  if(copyPrompt.indexOf(" time ")!=-1 || copyPrompt.indexOf(" how long ")!=-1 || copyPrompt.indexOf(" today ")!=-1 || copyPrompt.indexOf(" week ")!=-1 || copyPrompt.indexOf(" month ")!=-1 || copyPrompt.indexOf(" year ")!=-1 || copyPrompt.indexOf(" date ")!=-1 || copyPrompt.indexOf(" calendar ")!=-1){
    prompt="Today is "+ getCompleteDateStringUS() + " at " +getClockString() +". "+ prompt;
    Serial.println("Prompt: date time");
  }

  if(copyPrompt.indexOf(" where ")!=-1 || copyPrompt.indexOf(" location ")!=-1 || copyPrompt.indexOf(" city ")!=-1 || copyPrompt.indexOf(" country ")!=-1 || copyPrompt.indexOf(" europe ")!=-1 || copyPrompt.indexOf(" distance ")!=-1 || copyPrompt.indexOf(" far ")!=-1 || copyPrompt.indexOf(" area ")!=-1){
    prompt="Currently I am located in Eindhoven the Netherlands, particularly in the trulifi office. "+ prompt;
    Serial.println("Prompt: location");
  }

  return prompt;

}