/* DRT - A Dataflow Run Time - v0.1
 * Copyright (c) Roberto Giorgi 2013/09/19 - FREE SOFTWARE: SEE NOTICE BELOW */

#ifndef DRT_H
#define DRT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

uint64_t df_ts0[100],df_ts1[100];
uint64_t df_tt;

uint64_t totlaunch,totscans,totbusy,maxfmem,tottsched,tottdestr,tottw,maxdscheddestr,totdfx;
uint64_t totwaiting,totready,totdec,tottcache,tottsub,tottpub;
typedef uint64_t *afp;
typedef void (*fpt)(void);
uint64_t fsize;
afp fifo;
uint64_t start,end,end1;
afp corefp;
#define MAXLOOPS	3
uint64_t launcherbusy, launcherlooping;
uint64_t mydtsudebug;  /* contains DRT_DEBUG environment variable valuie */

#define FGUARD		4 // guard space to overcome a limitation of scale generated code
#define FIRSTFREEOFFSET 5
#define TWAITING	(0x1L)
#define TREADY		(0x2L)
#define TBITS(A)	((A)& 0x7L)
#define TADDR(A)	((A)& 0xFFFFFFFFFFFFFFF8L)
#define TSETEMPTY(A)	(A = 0L)
#define TSETWAIT(A,V)	(A = ((V) & 0xFFFFFFFFFFFFFFF8L)+TWAITING)
#define TSETREADY(A)	((A) = (((A) & 0xFFFFFFFFFFFFFFF8L)+TREADY))

__always_inline static uint64_t mytschedule(uint8_t _cond,fpt _ip, uint64_t _sc, uint32_t sz)
{ afp fp; 
  if (!_cond) return((uint64_t)NULL);
  fp=(afp)malloc((FIRSTFREEOFFSET+_sc+FGUARD)*sizeof(uint64_t));
  if(fp == NULL) { printf("- DRT: cannot allocate frame. \n"); fflush(stdout); exit(8); }
  fp[0]=(uint64_t)(_ip);fp[1]=_sc;
  fp[2]=(uint64_t)(&fifo[end]);
  fp[4]=(uint64_t)(sz);
  /* mytschedule implementation finishes here. Below there is an optimization to recover empty slots in FIFO.*/
  fp[3]=_sc;
  maxfmem+=(FIRSTFREEOFFSET+_sc+FGUARD)*sizeof(uint64_t);
  ++tottsched;
  if (maxdscheddestr<tottsched-tottdestr) maxdscheddestr=tottsched-tottdestr;
  TSETWAIT(fifo[end],(uint64_t)fp); ++totwaiting;
  if (mydtsudebug > 1) {
    printf("Thread Scheduler: Frame Id=%ld  Instruction Pointer=%p  Frame Pointer=%p   Synchronous Counter=%ld/%ld \n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],TADDR(fifo[end]),fp[1],fp[3]); fflush(stdout);
  }
  end = (end+1) % fsize;
  if (TBITS(fifo[end])) {
    ++totscans;
    /* scan to see if there are empty slots ahead */
    end1=end;
    totbusy=0;
    do {
      end1 = (end1+1) % fsize;
      ++totbusy;
      if (end1==end) { printf("- FIFO: queue is totally full. (totlaunch=%ld,totscans=%ld,totbusy=%ld)\n",totlaunch,totscans,totbusy); fflush(stdout); exit(3); }
    } while (TBITS(fifo[end1]));
    end = end1;
    if (mydtsudebug) {
      printf("- FIFO busy slot (totlaunch=%ld,totscans=%ld,totbusy=%ld e=%ld)\n",totlaunch,totscans,totbusy,end); fflush(stdout);
    }
  }
//  if (end==start) { printf("- FIFO ended (totlaunch=%ld) end=%ld fifo[end]=0x%x, totbusy=%ld\n",totlaunch,end,fifo[end],totbusy); fflush(stdout); exit(1); }
  return((uint64_t)fp);
}

__always_inline static uint64_t mytwrite(afp fp1, uint64_t off1, uint64_t val)
{ 
  /* for df_writeN: */
  afp fp; uint64_t off;
//  printf("     xx: fp1=%p off1=%p val=%p\n",fp1,off1,val); fflush(stdout);
  if (mydtsudebug > 2) {
   // printf("     Tw: fp1=%p off1=%p val=0x%lx\n",fp1,off1,val); fflush(stdout);
  }
  if (((uint64_t)fp1 & 0x80000000000000UL)>>48 == 0x80) { //small hack to support df_writeN
    off = ((uint64_t)fp1 & 0xFFUL); 
    fp = (afp)(((uint64_t)fp1 & 0xFF7FFFFFFFFFFFFFUL) >>8);
  }
  else { 
    fp = fp1; off=off1;
  }
//  printf("     yy: fp= %p off= %p val=%p\n",fp,off,val); fflush(stdout);
  fp[off+FIRSTFREEOFFSET]=val; (fp[1])--; if (fp[1] == 0) { TSETREADY(*((afp)(fp[2]))); ++totready; --totwaiting;} ++tottw; 
  if (mydtsudebug > 2) {
    printf("Thread Write:     Frame Id=%ld  Instruction Pointer=%p  Frame Pointer=%p/%ld Synchronous Counter=%ld/%ld  Value=%ld  \n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],fp,off,fp[1],fp[3], val); fflush(stdout);
  }
}

__always_inline void mydropframe()
{
  ++tottdestr;
  if(!launcherbusy) { launcherbusy=1;
    /* Launcher */
    if(start==end) { printf("- FIFO: attempt to extract from empty queue. \n"); fflush(stdout); exit(2); }
    else {
      uint64_t found,start1,start0,start2;
      uint64_t bits, bits0;
      uint64_t val;
      afp fpnew;
      uint64_t ipnew;
      if (mydtsudebug) {
     //   printf("- DRT: Starting Dataflow launcher.\n"); 
	fflush(stdout);
      }
      totlaunch = 0;
      launcherlooping = 0;
      do {
        start1 = start; found=0;
        do {
          val = fifo[start1]; fpnew = (afp)(TADDR(val)); bits = TBITS(val);
          ipnew = 0;
          start0 = start1; bits0 = bits;
          start1 = (start1+1) % fsize;
          bits = TBITS(fifo[start1]);
          if (bits0 == TREADY) { 
            TSETEMPTY(fifo[start0]); /* clean fifo slot */
            --totready; ++found;
	    corefp = fpnew; ipnew = fpnew[0];
            ++totlaunch;
            if (mydtsudebug > 1) {
             // printf("  TE: fi=%ld st=%ld  ipnew=%p  fpnew=%p (s=%ld,s1=%ld,e=%ld,ll=%ld  launched=%ld  R=%ld,W=%ld)\n",start0,bits0,ipnew,fpnew,start,start1,end,launcherlooping,totlaunch,totready,totwaiting); 
fflush(stdout);
            }
            ((fpt)(ipnew))();
          }
//        } while ((bits != 0 || start1!=end) && launcherlooping < MAXLOOPS);
//          if (bits==TWAITING) printf("  pp: fi=%ld st=%ld  ipnew=%p  fpnew=%p (s=%ld,s1=%ld,e=%ld,ll=%ld  launched=%ld  R=%ld,W=%ld)\n",start0,bits,ipnew,fpnew,start,start1,end,launcherlooping,totlaunch,totready,totwaiting); fflush(stdout);
        } while ((bits == TREADY || start1!=end) && launcherlooping < MAXLOOPS);
        if (! found) ++launcherlooping; 
        if (launcherlooping == MAXLOOPS) { printf("- DRT runtime: cannot find READY threads! looping reached %ld\n", MAXLOOPS); exit(7); }
//        while (bits == 0 && start!=end) { start = (start+1) % fsize; bits=TBITS(fifo[start]); }
//        while (bits == 0 && start!=start2) { start = (start+1) % fsize; bits=TBITS(fifo[start]); }
//        while (bits != TREADY && start!=start2) { start = (start+1) % fsize; bits=TBITS(fifo[start]); }
//        start2=(start+1) % fsize;
//        bits=TBITS(fifo[start]);
//        while (bits != TREADY && start!=start2) { start = (start-1) % fsize; bits=TBITS(fifo[start]); }
        start2=(start-1) % fsize;
        bits=TBITS(fifo[start]);
//        if (bits != TREADY)
          while (bits != TREADY && start!=start2) { start = (start+1) % fsize; bits=TBITS(fifo[start]); }
 //        printf("  qq: fi=%ld st=%ld  ipnew=%p  fpnew=%p (s=%ld,s1=%ld,e=%ld,ll=%ld  launched=%ld  R=%ld,W=%ld)\n",start0,bits,ipnew,fpnew,start,start1,end,launcherlooping,totlaunch,totready,totwaiting); fflush(stdout);
//        start2=(start+1) % fsize;
//        bits=TBITS(fifo[start]);
//        while (bits != 0 && start!=start2) { start = (start-1) % fsize; bits=TBITS(fifo[start]); }
//      } while (bits != 0);
//      } while (bits == TREADY || start==start2);
      } while (bits == TREADY);
    }
    if (mydtsudebug) {
  //    printf("- DRT: Ending DF launcher (mxfmem=%ld MB,ts=%ld,td=%ld,tw=%ld,mxsd=%ld R=%ld,W=%ld).\n",maxfmem/1024/1024,tottsched,tottdestr,tottw,maxdscheddestr,totready,totwaiting);
      fflush(stdout);
    }
    launcherbusy=0;
    free(fifo);
  } else {
    maxfmem -= (FIRSTFREEOFFSET+corefp[3]+FGUARD)*sizeof(uint64_t);
    if (mydtsudebug > 1) {
     // printf("  TD: fi=%ld  ip=%p  fp=%p  sc=%ld (mxfmem=%ld MB,ts=%ld,td=%ld,tw=%ld,mxsd=%ld R=%ld,W=%ld)\n",(corefp[2]-(uint64_t)fifo)/sizeof(uint64_t),corefp[0],corefp,corefp[3],maxfmem/1024/1024,tottsched,tottdestr,tottw,maxdscheddestr,totready,totwaiting); 
fflush(stdout);

//      printf("- DRT: mydfdestroy (mxfmem=%ld MB,ts=%ld,td=%ld,tw=%ld,mxsd=%ld,fp=%p,sc=%ld).\n",maxfmem/1024/1024,tottsched,tottdestr,tottw,maxdscheddestr,corefp,corefp[3]); fflush(stdout);
    }
    free(corefp);
  }
}
__always_inline static uint64_t mydfstart(uint64_t df_ts0[])
{
  uint64_t phypz, psize, totcpu, totmem, fszmem, i, t;
  char *s;
  phypz = sysconf(_SC_PHYS_PAGES);
  psize = sysconf(_SC_PAGE_SIZE);
  totcpu = sysconf(_SC_NPROCESSORS_CONF);
  totmem = phypz * psize /1024 /1024 /1024;
  fsize = ((phypz * psize) / sizeof(uint64_t)) / 10; /* rule of thumb: allocate a 10% of total phy memory */
  s = getenv("DRT_DEBUG"); mydtsudebug = (s) ? atoi(s) : 0;
  s = getenv("DRT_FSIZE"); t = (s) ? atoi(s) : fsize; if (t) fsize = t;
  fszmem = fsize /1024 /1024 *sizeof(uint64_t);
  if (mydtsudebug) {
    printf("- DRT: you have %ld GB of physical memory and %d processors on this host.\n", totmem, totcpu);
    printf("- DRT: allocating a FIFO queue of %ld MB (%ld entries).\n", fszmem, fsize);
    fflush(stdout);
  }
  fifo = (afp)malloc(fsize*sizeof(uint64_t));
  if(fifo == NULL) { printf("- FIFO: cannot allocate queue. \n"); fflush(stdout); exit(4); }
  for (i =0; i<fsize; ++i) TSETEMPTY(fifo[i]);
  if (mydtsudebug) {
   // printf("- DRT: FIFO allocation+initialization done.\n");
    fflush(stdout);
  }
}

__always_inline static uint64_t mydfexit(void) {
  ++totdfx;
  if (totdfx>1) {
    printf("- DRT: mydfexit called more the ONCE !\n");
  }
  if (start!=end && !launcherbusy) {
    printf("- DRT: mydfexit there are pending threads (%ld) start=%ld end=%ld!\n", end-start,start,end);
  }
}

__always_inline static uint64_t mydfread(afp fp, uint64_t off) {
  uint64_t val = fp[(off)+FIRSTFREEOFFSET];
  if (mydtsudebug > 3) {
    printf("Thread Read:      Frame Id=%ld  Instruction Pointer=%p  Frame Pointer=%p/%ld Value=%ld\n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],fp,off,val);fflush(stdout);
  }
  return val;
}

__always_inline static void mymwrite(afp p, uint64_t val) {
  if (mydtsudebug > 3) {
    printf("     MW: %p  val=0x%lx\n",p,val);fflush(stdout);
  }
  *p=val;
}

__always_inline static uint64_t mymread(afp p) {
  if (mydtsudebug > 3) {
    printf("     xx: %p  \n",p);fflush(stdout);
  }
  uint64_t val = ((uint64_t)(*p));
  if (mydtsudebug > 3) {
    printf("     MR: %p  val=0x%lx\n",p,val);fflush(stdout);
  }
  return val;
}

__always_inline static void mytdecreaseN(afp fp1, uint64_t n)
{ 
  afp fp; uint64_t off;
  if (mydtsudebug > 2) {
    printf("     T.: fp1=%p  n=%ld\n",fp1,n); fflush(stdout);
  }
  if (((uint64_t)fp1 & 0x80000000000000UL)>>48 == 0x80) { //small hack to support df_writeN
    off = ((uint64_t)fp1 & 0xFFUL);
    fp = (afp)(((uint64_t)fp1 & 0xFF7FFFFFFFFFFFFFUL) >>8);
  }
  else {
    fp = fp1; off=0;
  }
//  printf("     yy: fp= %p off= %p val=%p\n",fp,off,val); fflush(stdout);

  fp[1] -= n; if (fp[1] == 0) { TSETREADY(*((afp)(fp[2]))); ++totready; --totwaiting; } ++totdec; 
  if (mydtsudebug > 2) {
    printf("     T0: fi=%ld  ip=%p  fp=%p  sc=%ld/%ld\n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],fp,fp[1],fp[3]); fflush(stdout);
  }
}

__always_inline static void *mytcache(afp fp1)
{ 
  afp rfp, fp; uint64_t off;
  if (mydtsudebug > 2) {
    printf("     Tc: fp1=%p\n",fp1); fflush(stdout);
  }
  if (((uint64_t)fp1 & 0x80000000000000UL)>>48 == 0x80) { //small hack to support df_writeN
    off = ((uint64_t)fp1 & 0xFFUL);
    fp = (afp)(((uint64_t)fp1 & 0xFF7FFFFFFFFFFFFFUL) >>8);
  }
  else {
    fp = fp1; off=0;
  }
//  printf("     yy: fp= %p off= %p val=%p\n",fp,off,val); fflush(stdout);

  rfp = fp + FIRSTFREEOFFSET; ++tottcache; 
  if (mydtsudebug > 2) {
    printf("     TC: fi=%ld  ip=%p  fp=%p  sc=%ld/%ld rfp=%p\n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],fp,fp[1],fp[3], rfp); fflush(stdout);
  }
  return rfp;
}

__always_inline static void *mytsubscribe(afp fp1, uint32_t roff, uint32_t sz, uint8_t rw)
{ //TODO
  afp rfp, fp; uint64_t off;
  if (mydtsudebug > 2) {
    printf("     Tu: fp1=%p\n",fp1); fflush(stdout);
  }
  if (((uint64_t)fp1 & 0x80000000000000UL)>>48 == 0x80) { //small hack to support df_writeN
    off = ((uint64_t)fp1 & 0xFFUL);
    fp = (afp)(((uint64_t)fp1 & 0xFF7FFFFFFFFFFFFFUL) >>8);
  }
  else {
    fp = fp1; off=0;
  }
//  printf("     yy: fp= %p off= %p val=%p\n",fp,off,val); fflush(stdout);

  uint64_t val = (uint64_t) malloc(sz);
  fp[off+FIRSTFREEOFFSET]=val; ++tottsub;

  if (mydtsudebug > 2) {
    printf("     TU: fi=%ld  ip=%p  fp=%p  p=%p\n",(fp[2]-(uint64_t)fifo)/sizeof(uint64_t),fp[0],fp,val); fflush(stdout);
  }
}

__always_inline static void mytpublish(void* regptr)
{ //TODO
}

#define df_tstamp(df_ts0)	mydfstart(df_ts0);
#define df_ldframe(n)		afp fp = corefp;
#define df_destroy()		mydropframe();
//#define df_frame(_off)	(fp[(_off)+FIRSTFREEOFFSET])
#define df_frame(_off)		mydfread(fp,_off)
#define df_read(_off)		(df_frame(_off))
#define df_write(_tid,_off,_val) \
				mytwrite((afp)(_tid), _off, _val)
#define df_tschedule_cond(_ip,_sc,_cond) \
				(mytschedule(_cond,(fpt)(_ip),_sc,(_sc)*sizeof(uint64_t)))
#define df_tscheduleN(_ip,_sc)	(mytschedule(1,(fpt)(_ip),(_sc),(_sc)*sizeof(uint64_t)))
#define df_tschedule(_ip,_sc)	(mytschedule(1,(fpt)(_ip),(_sc),(_sc)*sizeof(uint64_t)))
#define df_talloc(_type, _sz)	(uint64_t)malloc(_sz)
///* df_writeN assumes we do not care about the 3 most #define df_tread_al(_p)		((uint64_t)(*((afp)(_p))))
#define df_tread_al(_p)		mymread((afp)(_p))
//#define df_twrite_al(_p,_val)	*((afp)(_p))=(_val)
#define df_twrite_al(_p,_val)	mymwrite((afp)(_p),_val)
#define df_tfree(_p)		free((void *)(_p))
#define df_exit()		mydfexit()
//#define _TLOC(_tp,_l)		((_tp)+((uint64_t)(_l)<<3))
//#define _TLOC(_tp,_l) ((_tp)+((unsigned long long)(_l)<<3))
#define _TLOC(_tp,_l)		(((((uint64_t)_tp)<<8)+(_l)) | 0x0080000000000000UL)
#define myL8b(_VL)		(((uint64_t)_VL) & 0xFFUL)
#define myH56b(_VH)		(((uint64_t)_VH) >> 8)
//#define df_writeN(_tid,_val)	mytwrite((afp)(myH56b(_tid)), myL8b(_tid), _val)
#define df_writeN(_tid,_val)	mytwrite((afp)(_tid), 0, _val)
/* df_writeN assumes we do not care about the 3 most sigificant bits of _tid */
#define df_tdecrease(_tid)	mytdecreaseN((afp)(_tid), 1)
#define df_tdecreaseN(_tid,_n)	mytdecreaseN((afp)(_tid), (uint64_t)(_n))
#define df_tschedule64(_ip,_sc) \
				(mytschedule(1,(fpt)(_ip),(_sc),(_sc)*sizeof(uint64_t)))
#define df_tschedulez(_ip,_sc,_sz) \
				(mytschedule(1,(_ip),(_sc),(_sz)))
#define df_tload()		(corefp+FIRSTFREEOFFSET)
#define df_tcache(_tid)		(mytcache((afp)(_tid)))
#define df_tid_t		uint64_t
#define df_constrain(_tid,_xc)	
#define df_printstats(_ts0,_ts1,_t1)	
#define _TLOC_OFF(_tloc)	(myL8b(_tloc))
#define df_subscribe(_tloc,_roff,_sz,_rw) \
				mytsubscribe((afp)(_tloc),_roff,_sz,_rw)
#define df_publish(_regptr)	mytpublish(_regptr)
#define _OWM_DESC(sz,rw)	(((sz)<<8)|((rw)&0xff))
#define _OWM_R(d)		((d)&0x01)
#define _OWM_W(d)		((d)&0x02)
#define _OWM_SZ(d)		((d)>>8)
#define _OWM_MODE_R		(0x01)
#define _OWM_MODE_W		(0x02)
#define _OWM_MODE_RW		(_OWM_MODE_R|_OWM_MODE_W)
typedef void (*df_thread_t)(void);



/*
   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#endif // DRT_H

