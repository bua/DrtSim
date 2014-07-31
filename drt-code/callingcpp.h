#ifndef HEADER_FILE
#define HEADER_FILE

 #ifdef __cplusplus
     extern "C" {
 #endif
	int radGen();  // calling random
  int lffttable (char *fpValue, char *flag);   // functio
  int wtTable (char *fpValue, char *ipValue, char *scValue, char *flag);
 int plqTable (char *fpValue, char *ipValue, char *flag);
  void callPrint(int counter1, char *name); 
// // Calling DTSU Randomly
int dffttable (char *flag);
int ptqTable (char *ipValue, char *scValue, char *flag);
void calldtsuPrint(int counter, char *name);
 
 // void callWTPrint(int counter2);

 #ifdef __cplusplus
     }
 #endif

 #endif



