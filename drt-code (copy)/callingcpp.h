#ifndef HEADER_FILE
#define HEADER_FILE

 #ifdef __cplusplus
     extern "C" {
 #endif
  int lffttable (char *fpValue, char *flag);   // functio
  int wtTable (char *fpValue, char *ipValue, char *scValue, char *flag);
 int plqTable (char *fpValue, char *ipValue, char *flag);
  void callPrint(int counter1, char *name); 
 
 // void callWTPrint(int counter2);

 #ifdef __cplusplus
     }
 #endif

 #endif



