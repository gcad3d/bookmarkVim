/*   Add or delete Bookmark                           RF  2004-06-01

Add or delete Bookmark in Bookmarkfile $HOME/.vim/bookmarks/<filename>

For documentation see ~/.vim/vi-bookmarks_README.txt

==================================
Compile & Link Unix:
cc -o vi-bookmarks`getconf LONG_BIT` vi-bookmarks.c
" creates vi-bookmarks32 or vi-bookmarks64

Compile & Link MS-Windows VC:
cl -o vi-bookmarks.exe vi-bookmarks.c


==================================
FILES:
~/.vim
  vi-bookmarks_README.txt  using, installation
  vi.sh             resolv <filename>:<lineNr>;  call vim -N $*
  vi-bookmarks.vim  calls executable vi-bookmarks64
  vi-bookmarks.c    compile, link -> executable vi-bookmarks64
  vim.out           logfile (if enabled)

~/.vim/bookmarks
  vi-bookmarks_fList   filenames with refCount being modified
  vi-bookmarks_fnam    temp-file (name of file modified last)
  <bookmarkfiles>

//================================================================
PROGRAMFLOW:

marks_load  __MARKS_LOAD__   - enter new file
  in: sessNr and mfNam (userfile)
  make sfNam = safe-filename of mfNam
  find fNr = nr of active copies of file <sfNam> in vi-bookmarks_fList
  get afNam = <sfNam> + '_' + 'fNr'
  if file <sfNam> does not exist yet:
    if (fNr==0) then create new empty file
    else copy file <sfNam>_0 <sfNam>_<fNr>
  write afNam into file vi-bookmarks_fList into line sessNr

main - __MARKS__
  in: sessNr and fnTxt = text to add/remove in active-bookmark-file
  get filename of active-bookmark-file from sessNr
  add or remove fnTxt in active-bookmark-file

marks_save  __MARKS_SAVE__  exit-file <sessNr>
  in: sessNr
  clear line nr sessNr in file vi-bookmarks_fList (change to "-")


//================================================================
main-Input:
Parameter-1:
  __MARKS__               add new bookmark
  __MARKS_LOAD__
  __MARKS_SAVE__
Parameter-2:
  sessNr         session-number  for all Parameter-1;
Parameter-3:
  mfNam          filename, only if Parameter-1 is __MARKS_LOAD__


..................................
main // Add or delete Bookmark in Bookmarkfile
  // in: par1=Filename, par2=LineNr, par3=Linetext
..................................
marks_load        // :417 user has opened new file
  get_safNam        // get sfNam = safe filename Bookmarkfile;
  marks_lst_wr__    // :620
  get_fNr           // :298 get nr of active copies of file <sfNam>

..................................
__MARKS__   main :195
  get_fnTxt      // get fnTxt = bookmarkText
  get_bfNam :840 // get bfNam = full filename of active-bookmarkfile

..................................
marks_save :453


//================================================================
Test exe:

// open new session # 123
~/.vim/vi-bookmarks64 __MARKS_LOAD__ 123 t1.c

// add bookmark to session # 123
echo " linetext in file t1.c .." > ~/.vim/bookmarks/vi-bookmarks_txtl_123
~/.vim/vi-bookmarks64 __MARKS__ 123

// close session # 123
~/.vim/vi-bookmarks64 __MARKS_SAVE__ 123


==================================
2021-08-10 RF.
2004-07-09 . RF.
2004-06-19 MS-Win-Version korr. RF.
2018-09-04 new version with vi-marks. RF.
2021-07-31 rewrite. RF.
*/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // access

#include <stdarg.h>


#define  MAX_NR    16

char *mfDir = "/.vim/bookmarks/";    // in $HOME

int  sessNr;                         // session-number

char mfNam[256];                     // unmodified filename 
char sfNam[256];                     // safe filename 
char afNam[256];                     // filename active file (sfNam_refCount)
char fnLst[256];                     // filename of file with names of active files
FILE *fpLst;

char fnTmp[256];
FILE *fpTmp;



  void log__ (char* sFmt, ...);
  char* UTX_CleanCR (char* string);


//----------------------------------------------------------------
// prototypes:
  int marks_load ();
  int marks_save ();
  int get_fnTxt (char *fnTxt);
  int get_bfNam (char *bfNam);
  int get_fnBkm (char *fnBmk);
  int marks_lst_wr_ini ();
  int marks_lst_wr__ (int lNr, char *sTxt );
  int marks_lst_rd__ (char *sTxt, int lNr);
  int marks_lst_wr_Tmp2Lst ();
  int marks_lst_add (int sessNr, char *fNam);
  int marks_lst_del (int sessNr);



//================================================================
//================================================================
 



//===================================================================
  int main (int paranz, char *params[]) {
//===================================================================
// Add or delete Bookmark in Bookmarkfile "Bookmark.lst".


  int    i1, ii, tNr;
  FILE   *fpi, *fpo;
  char   fnTxt[256], lCmd[256],
         s1[400], bfNam[256],
         *p0, *p1, *p2;



  log__ ("************** vi-bookmarks %d",paranz);
  // for(i1=1; i1<paranz; ++i1) log__("par[%d] = |%s|",i1,params[i1]);


  if(paranz < 2) {
    // log__ ("ERROR paranz = %d",paranz);
    printf("Usage: par1=Filename, par2=LineNr, par3=Linetext\n");
    return -1;
  }
    log__ ("|%s|%s|",params[1],params[2]);


  // // get par.1 = filename
  // strcpy(fnTxt, params[1]);
    // // log__("fnTxt = |%s|",fnTxt);


  // get lCmd = command
  strcpy(lCmd, params[1]);
  UTX_CleanCR (lCmd);              // remove following CR,Lf, ..
    log__("lCmd = |%s|",lCmd);

  // sessNr = session-number
  sessNr = atoi(params[2]);
    log__("sessNr = |%d|",sessNr);

  // get filename of outFile
  if(paranz < 4) {
    strcpy(mfNam, "_");
  } else {
    strcpy(mfNam, params[3]);
  }
    log__("mfNam = |%s|",mfNam);


  // get fnLst = filename of file with names of active files
  sprintf(fnLst, "%s%svi-bookmarks_fList", getenv("HOME"), mfDir);
    log__(" fnLst |%s|",fnLst);

  // create tempFilename
  sprintf(fnTmp, "%s%svi-bookmarks_tmpList", getenv("HOME"), mfDir);
    log__(" fnTmp |%s|",fnTmp);


  if(!strcmp (lCmd, "__MARKS_LOAD__")) return marks_load ();
  if(!strcmp (lCmd, "__MARKS_SAVE__")) return marks_save ();

  if(strcmp (lCmd, "__MARKS__")) {
    log__("vi-bookmarks-exe: ERR-1");
    return -1;
  }


  //================================================================
  // "__MARKS__"

  // get fnTxt = bookmarkText out of ~/.vim/bookmarks/vi-bookmarks_txtl_<sessNr>
  get_fnTxt (fnTxt);
    log__("fnTxt = |%s|", fnTxt);

  // get bfNam = filename active-bookmarkfilename
  get_bfNam (bfNam);
    log__("bfNam = |%s|", bfNam);

  //----------------------------------------------------------------
  // add or delete fnTxt in file bfNam

  // open fnTmp for write
  if((fpo = fopen (fnTmp, "w")) == NULL) return -1;

  ii = 1;
  tNr = 0;

  // open bfNam for read
  if((fpi = fopen (bfNam, "r")) == NULL) {
    goto L_9;
  }


  while (!feof (fpi)) {
    if(fgets (s1, 256, fpi) == NULL) break;
    UTX_CleanCR (s1);              // remove following CR,Lf, ..
      // log__(" tNr= %d s1 = |%s|",tNr,s1);
    if(!strcmp(s1, "--empty--")) continue;
    // if line already exists, delete line
    i1 = strlen(s1);  // fnTxt has one blank more !
    if(!strncmp(s1, fnTxt, i1)) { ii = 0; continue;}
    // copy existing bookmark
    fprintf(fpo,"%s\n",s1);
    ++tNr;
  }

  fclose (fpi);

  // add new tag
  L_9:
  if(ii) {
    // ii=1: bookmark not yet exists; write
      // log__(" add tag = |%s|",fnTxt);
    fprintf(fpo,"%s\n", fnTxt);
    ++tNr;
  }

  if(tNr == 0) {
    fprintf(fpo,"--empty--\n");
    ++tNr;
  }

  fclose (fpo);


  remove (bfNam);
  rename (fnTmp, bfNam);

  return 0;

}


//================================================================
  int get_safNam () {
//================================================================
// get fnBmk = bookmarkFilename
// Input:
//   mfNam
// change leading '.' -> '_'
// change all '/' -> '-'

  char   *p0, *p1, bfNam[256], s1[256];
  FILE   *fpi;


  log__("get_safNam |%s|", mfNam);


  p0 = mfNam;

  // skip primary '/'
  while(p0[0] == '/') ++p0;

  strcpy(sfNam, p0);

  p0 = sfNam;

  // change all '.' -> '_'
  while(p1 = strchr(p0, '.')) *p1 = '_';

  // change all '/' -> '-'
  while(p1 = strchr(p0, '/')) *p1 = '-';

    log__("ex-get_safNam sfNam=|%s|",sfNam);

  return 0;

}


//================================================================
  int get_fNr () {
//================================================================
// get fNr = nr of active copies of file <sfNam> in vi-bookmarks_fList
// if session is already active: return active refCount
// if session is not yet active: return lowest not active refCount
// Input:
//   sfNam    filename of file to edit

  int    fNr, rNr, nLen, i1;
  char   s1[400], *ps, *p0, sTab[] = "0123456789";

  log__("get_fNr |%s| %d\n",sfNam,sessNr);



  if((fpLst = fopen (fnLst, "r")) == NULL) {
    log__("vi-bookmarks-exe: ERROR get_fNr 1");
    fNr = 0;
    goto L_exit;
  }

  fNr = 0;

  while (!feof (fpLst)) {
    if(fgets (s1, 400, fpLst) == NULL) break;
    // get sessNr
    rNr = atoi(s1);
    ps = strchr(s1, ' ');
    if(!ps) continue;
    ++ps;
    // get nLen = length without "_<refCount>"
    p0 = strrchr(ps, '_');
    if(!p0) continue;
    nLen = p0 - ps;          // length filename
      log__(" get_fNr fNr=%d sessNr=%d nLen=%d |%s|",fNr,sessNr,nLen,ps);

    // get refCount
    ++p0;
    i1 = atoi(p0);
    if(rNr == sessNr) {fNr = i1; goto L_done;}  // yet active ..

    if(strncmp(ps, sfNam, nLen)) continue;
    // found fileName, add refCount
    sTab[i1] = '_';
    ++fNr;
      log__(" get_fNr fNr=%d sessNr=%d nLen=%d",fNr,sessNr,nLen);
    if(fNr > 9) break;
  }

  // find unused fNr
    log__(" get_fNr sTab |%s|",sTab);
  for(i1=0; i1<10; ++i1) {
    if(sTab[i1] == '_') continue;
    fNr = i1;
    break;
  }



  L_done:
    fclose (fpLst);


  L_exit:
    log__(" ex-get_fNr fNr=%d",fNr);
  return fNr;

}


//================================================================
  int marks_fnam_wr (char *fNam) {
//================================================================
// write fNam into file vi-bookmarks_fnam

  int    irc;
  char   fnfn[256];


  // get line nr <sessNr> out of file vi-bookmarks_fList
  sprintf(fnfn, "%s%svi-bookmarks_fnam", getenv("HOME"), mfDir);
  if((fpLst = fopen (fnfn, "w")) == NULL) {
    log__("vi-bookmarks-exe: ERROR marks_fnam_wr 1");
    irc = -1;
    goto L_exit;
  }

  fprintf(fpLst, "%s\n", fNam);

  fclose(fpLst);

  irc = 0;

  L_exit:
    log__(" ex-marks_fnam_wr irc=%d |%s|",irc);
  return irc;

}


//================================================================
  int marks_load () {
//================================================================
// write fnBmk = name of active bookmarksfile into line <sessNr> of file 
//   ~/.vim/bookmarks/vi-bookmark_fList
// Input:
//   mfNam   filename in editor

  int    irc, fNr, i1;
  char   ffNam[256];
  char   s1[256], s2[400];

  log__ ("===============================");
  log__ ("marks_load sessNr=%d mnNam=|%s|",sessNr,mfNam);

  // make sure directory mfDir "~/.vim/bookmarks/" exist ..
  sprintf(s2,"mkdir -p \"%s%s\"", getenv("HOME"), mfDir);
  system (s2);

  // create file fnLst (vi-bookmarks_fList) - if not yet exists -
  // - or if starting first session
  if(((access (fnLst, R_OK)) != 0) || (sessNr < 2)) {
    log__ ("marks_load-init");
//     // create empty file fnLst
//     if((fpLst = fopen (fnLst, "w")) == NULL) {
//       log__("vi-bookmarks-exe: ERROR marks_load 1");
//       irc = -1;
//       goto L_exit;
//     } 
//     for(i1=0; i1<MAX_NR; ++i1) fprintf(fpLst,"-\n");
//     fclose (fpLst);
    remove (fnLst);

    // and remove vim.out
    sprintf(s2,"%s/.vim/vim.out",getenv("HOME"));
    log__ ("marks_load-remove |%s|",s2);
    remove (s2);
  }
  

  // get sfNam = safe-filename of mfNam
  get_safNam ();

  // get fNr = nr of active copies of file <sfNam> in vi-bookmarks_fList
  fNr = get_fNr ();
  if(fNr < 0) {
    log__("EEEEEEEEEE vi-bookmarks-exe: ERROR marks_load 2");
    irc = -2;
    goto L_exit;
  } 

  // get afNam = <sfNam> + '_' + 'fNr' (safeName + refCount)
  sprintf(afNam, "%s_%d", sfNam, fNr);
    log__("marks_load - afNam=|%s|",afNam);

  // create full-filename ffNam (bkmDir + afNam);
  sprintf(ffNam, "%s%s%s", getenv("HOME"), mfDir, afNam);
    log__("marks_load - ffNam=|%s|",ffNam);
  
  // check if ffNam already exists
  if ((access (ffNam, R_OK)) != 0) {
    // ffNam not yet exists
    if (fNr == 0) {
      // create new empty file 
      sprintf(s1, "echo \"--empty--\" > %s", ffNam);
      irc = system (s1);
      if(irc) {
        log__("vi-bookmarks-exe: ERROR marks_load 3");
        irc = -3;
        goto L_exit;
      }

    } else {
      // copy file <sfNam>_0 <sfNam>_<fNr>
      sprintf(s1, "%s%s%s_0", getenv("HOME"), mfDir, sfNam);
      sprintf(s2,"/bin/cp -f \"%s\" \"%s\"",s1,ffNam);
        log__("%s",s2);
      system (s2);
    }
  }

  // write afNam into file vi-bookmarks_fList into line sessNr
  // marks_lst_wr__ (sessNr, afNam);
  marks_lst_add (sessNr, afNam);

  // write afNam into file vi-bookmarks_fnam
  marks_fnam_wr (ffNam);

  irc = 0;

/*
  // get fnBmk = filename Bookmarkfile; eg $HOME/.vim/bookmarks/<safe-filnam>
  get_fnBkm (fnBmk);
    log__("fnBmk = |%s|",fnBmk);

  // if file fnBmk does not exist:
  if ((access (fnBmk, R_OK)) != 0) {
    // create empty file fnBmk
    sprintf(cbuf, "echo \"--empty--\" > %s", fnBmk);
    irc = system (cbuf);
    if(irc) {
      log__("vi-bookmarks-exe: ERROR marks_load 1");
      irc = -1;
      goto L_exit;
    }
  }

  //----------------------------------------------------------------
  // check if a primary file alread exists
  // get fnLst - vi-bookmarks_name_<sessNr>
  sprintf(fnLst, "%s%svi-bookmarks_name_%d",getenv("HOME"),mfDir,sessNr);
  
  // open fnLst for write
  if((fpLst = fopen (fnLst, "w")) == NULL) {
    log__("vi-bookmarks-exe: ERROR marks_load 2");
    irc = -2;
    goto L_exit;
  } 

  fprintf(fpLst, "%s", fnBmk);

  fclose (fpLst);
*/

  L_exit:
    log__(" ex-marks_load irc=%d",irc);
  return irc;

}


//================================================================
  int marks_save () {
//================================================================
// copy ~/.vim/bookmarks/vi-bookmarks -> bookmarksfile

  int  irc;
  char cbuf[512], fnBmk[256], bfNam[256];


  log__("marks_save sessNr %d", sessNr);

  // clear line nr sessNr in file vi-bookmarks_fList (change to "-")

  // replace line sessNr with ""
  // marks_lst_wr__ (sessNr, "-");
  marks_lst_del (sessNr);



/*
  //----------------------------------------------------------------
  // get <fn> out of vi-bookmarks_fList from lineNr = sessionNr

  // open
  marks_lst_wr_ini ();

  // get mfNam = real filename; irc = chrNr of mfNam
  irc = marks_lst_rd__ (mfNam, sessNr);
    log__("mfNam %d |%s|", irc, mfNam);
  if(irc < 1) {
    // file was open read-only (not as first intance);
    // skip saving (overwriting) the bookmarks
    return 0;
  }


  //----------------------------------------------------------------
  // copy back vi-bookmarks_<sessionNr> -> <safe-filnam>

  // set active-bookmarkfilename $HOME/.vim/bookmarks/vi-bookmarks_<sessionNr>
  get_bfNam  (bfNam, sessNr);
    log__("bfNam = |%s|",bfNam);

  // set fnBmk = filename Bookmarkfile; eg  $HOME/.vim/bookmarks/<safe-filnam>
  get_fnBkm (fnBmk);
    log__("marks_save fnBmk = |%s|",fnBmk);

  sprintf(cbuf,"cp -f \"%s\" \"%s\"",bfNam,fnBmk);
  // printf(cBuf, "copy /y %s %s",fnOld, fnNew);  // MS
    printf("|%s|\n",cbuf);


  //----------------------------------------------------------------
  // delete <sessNr> out of list of open files vi-bookmarks_fList

  // replace line sessNr with ""
  marks_lst_wr__ (sessNr, "-");

  // close files, copy Tmp -> Lst;
  marks_lst_wr_Tmp2Lst ();


  return system (cbuf);
*/

  irc = 0;

  return irc;

}


//================================================================
  int marks_lst_wr_ini () {
//================================================================
// open fnLst, fnTmp  on fpLst, fpTmp
// if fnLst does not exist: create empty file.
// retCod:  0 OK
//         -1 fnLst initialized
//         -2 ERROR unrecov.

  int    i1, irc;


  log__("marks_lst_wr_ini %d",sessNr);

  irc = 0;


  if(sessNr >= MAX_NR) {
    log__("vi-bookmarks-exe: ERROR marks_lst_wr_ini 1");
    return -2;
  }


  // create tempFilename
  sprintf(fnTmp, "%s%svi-bookmarks_tmpList", getenv("HOME"), mfDir);
    log__(" fnTmp |%s|",fnTmp);

  // open fnTmp for write
  if((fpTmp = fopen (fnTmp, "w")) == NULL) {
    log__("vi-bookmarks-exe: ERROR marks_lst_wr_ini 2");
    irc = -2;
    goto L_exit;
  }

  // open fnLst for read
  L_ini1:
  if((fpLst = fopen (fnLst, "r")) == NULL) {
    if(irc < 0) return -2;
    // fnLst does not exist: create empty file.
    if((fpLst = fopen (fnLst, "w")) == NULL) {
      log__("vi-bookmarks-exe: ERROR marks_lst_wr_ini 3");
      irc = -3;
      goto L_exit;
    }

    for(i1=0; i1<MAX_NR; ++i1) fprintf(fpLst,"\n");
    fclose (fpLst);
    log__("marks_lst_wr_ini: init fnLst");
    irc = -1;
    goto L_ini1;
  }


  L_exit:
    log__(" ex-marks_lst_wr_ini %d",irc);
  return irc;

}


//================================================================
  int marks_lst_rd__ (char *sTxt, int lNr) {
//================================================================
// read line <lNr>
// retCod:  0 = line empty (not first instance ..)
//         -1   internal error
//         >0   OK; sTxt = filename

  int    irc, sNr;
  char   s1[256];


  log__("marks_lst_rd__ %d", lNr);


  rewind (fpLst);
  rewind (fpTmp);


  sNr = -1;
  irc = -1;
  while (!feof (fpLst)) {
    if(fgets (s1, 256, fpLst) == NULL) break;
    ++sNr;
    if(sNr == lNr) {
      UTX_CleanCR (s1);              // remove following CR,Lf, ..
      strcpy (sTxt, s1);
      irc = strlen(sTxt);
    }
  }


  return irc;

}

/*
//================================================================
  int marks_lst_wr__ (int lNr, char *sTxt ) {
//================================================================
// replace line lNr with sTxt - read fnLst, write fnTmp

  int    irc, sNr;
  char   s1[400];


  log__("marks_lst_wr__ %d |%s|",lNr, sTxt);

  // open fnTmp for write
  if((fpTmp = fopen (fnTmp, "w")) == NULL) {
    log__("vi-bookmarks-exe: ERROR marks_lst_wr__ 1");
    irc = -1;
    goto L_exit;
  }

  // open fnLst for read
  if((fpLst = fopen (fnLst, "r")) == NULL) {
    log__("vi-bookmarks-exe: ERROR marks_lst_wr__ 2");
    irc = -2;
    goto L_exit;
  }

  sNr = -1;
  while (!feof (fpLst)) {
    if(fgets (s1, 256, fpLst) == NULL) break;
    ++sNr;
    if(sNr == lNr) fprintf(fpTmp,"%s\n", sTxt);
    else           fprintf(fpTmp,"%s", s1);
  }

  fclose(fpTmp);
  fclose(fpLst);

  // cp fnTmp fnLst
  sprintf(s1,"cp -f %s %s", fnTmp, fnLst);
  system (s1);

  irc = 0;


  L_exit:
    log__(" ex-marks_lst_wr__ %d",irc);
  return irc;

}
*/


//================================================================
  int marks_lst_add (int sessNr, char *fNam ) {
//================================================================
// add sessNr filename into vi-bookmarks_fList

  int    lNr;
  char   s1[300], sNr[8];
  FILE   *fpi, *fpo;


  log__("marks_lst_add %5d %s",sessNr,fNam);


  sprintf(sNr, "%d", sessNr);
  lNr = strlen(sNr);

  //----------------------------------------------------------------
  // add or delete fnTxt in file bfNam

  // open fnTmp for write
  if((fpo = fopen (fnTmp, "w")) == NULL) return -1;

  // open bfNam for read
  if((fpi = fopen (fnLst, "r")) == NULL) {
    goto L_9;
  }


  while (!feof (fpi)) {
    if(fgets (s1, 256, fpi) == NULL) break;
    UTX_CleanCR (s1);              // remove following CR,Lf, ..
      // log__(" tNr= %d s1 = |%s|",tNr,s1);
    // if line already exists, delete line
    // i1 = strlen(s1);  // fnTxt has one blank more !
    if(!strncmp(s1, sNr, lNr)) goto L_exit;  // alread exists ..
    // copy existing bookmark
    fprintf(fpo,"%s\n",s1);
    // ++tNr;
  }

  fclose (fpi);

  // add new tag
  L_9:
    fprintf(fpo,"%s %s\n",sNr,fNam);

  fclose (fpo);


  remove (fnLst);
  rename (fnTmp, fnLst);


  L_exit:
  return 0;

}


//================================================================
  int marks_lst_del (int sessNr) {
//================================================================
// delete file from vi-bookmarks_fList

  int    lNr;
  char   s1[300], sNr[8];
  FILE   *fpi, *fpo;


  log__("marks_lst_del %d",sessNr);


  sprintf(sNr, "%d", sessNr);
  lNr = strlen(sNr);

  //----------------------------------------------------------------
  // add or delete fnTxt in file bfNam

  // open fnTmp for write
  if((fpo = fopen (fnTmp, "w")) == NULL) return -1;

  // open bfNam for read
  if((fpi = fopen (fnLst, "r")) == NULL) {
    goto L_9;
  }


  while (!feof (fpi)) {
    if(fgets (s1, 256, fpi) == NULL) break;
    UTX_CleanCR (s1);              // remove following CR,Lf, ..
      // log__(" tNr= %d s1 = |%s|",tNr,s1);
    if(!strncmp(s1, sNr, lNr)) continue;     // found - skip
    // copy existing bookmark
    fprintf(fpo,"%s\n",s1);
    // ++tNr;
  }

  fclose (fpi);

  L_9:
  fclose (fpo);


  remove (fnLst);
  rename (fnTmp, fnLst);


  L_exit:




  return 0;

}


//================================================================
  int marks_lst_ck__ (char *fn) {
//================================================================
// check if fn is already open (if mfNam already is in fnLst)
// retCod  -1   fn does not yet exist in fnLst
//         >=0  does already exist as session <irc>


  int    irc, sNr;
  char   s1[256];


  log__("marks_lst_ck__ |%s|", fn);


  rewind (fpLst);


  // fnLst exists. read, test if file already exists
  irc = -1;
  sNr = -1;

  while (!feof (fpLst)) {
    if(fgets (s1, 256, fpLst) == NULL) break;
    ++sNr;
    if(strlen(s1) < 2) continue;
    UTX_CleanCR (s1);              // remove following CR,Lf, ..
      log__(" sNr= %d s1 = |%s|",sNr,s1);
    if(strstr(s1, fn)) {
      // already exists for session sNr;
      if(sNr == sessNr) {
        log__("vi-bookmarks-exe: ERROR marks_lst_ck__ 1");
        return -2;
      }
      irc = sNr;
      goto L_exit;
    }
  }

  L_exit:
    log__("ex-marks_lst_ck__ |%s| %d", fn, irc);
  return irc;

}


//================================================================
  int marks_lst_close () {
//================================================================
// close files


  log__("marks_lst_close");

  if(fpLst) fclose (fpLst);
  if(fpTmp) fclose (fpTmp);

  return 0;

}


//================================================================
  int marks_lst_wr_Tmp2Lst () {
//================================================================
// copy Tmp -> Lst;

  char   s1[512];


  log__("marks_lst_wr_Tmp2Lst");

  sprintf(s1,"cp -f %s %s", fnTmp, fnLst);
  system (s1);

  return 0;

}


/*
//================================================================
  int marks_lst_add () {
//================================================================
// add filename fnBmk into list of open files vi-bookmarks_fList;
// add mfNam into fnLst as line Nr <sessNr>
// check if fn is already open (if mfNam already is in fnLst)
//    yes: return its lNr
// retcod -1 is OK;
//        -2 is ERROR

  int    irc, i1, sNr;
  char   s1[512], bfNam[256], fnBmk[256];

  log__("marks_lst_add |%s| %d", mfNam, sessNr);


  // open fnLst, fnTmp
  irc = marks_lst_wr_ini ();
  if(irc < -1) return irc;


  // check if fn is already open (if mfNam already is in fnLst)
  // -1=no, else session-nr
  irc = marks_lst_ck__ (mfNam);

  if(irc < 0) {
    // mfNam not yet open
    // replace line sessNr with mfNam - read fnLst, write fnTmp
    marks_lst_wr__ (sessNr, mfNam);

    // close files, copy fnTmp -> fnLst;
    marks_lst_close ();
    marks_lst_wr_Tmp2Lst ();

  } else {
    // file <mfNam> is already open in session <irc>
    marks_lst_close ();
    // copy vi-bookmarks_<irc> -> <mfNam>
    get_bfNam (bfNam, irc);   // $HOME/.vim/bookmarks/vi-bookmarks_<sessionNr>
    get_fnBkm (fnBmk);        // eg  $HOME/.vim/bookmarks/<safe-filnam>
    sprintf(s1,"cp -f \"%s\" \"%s\"", bfNam, fnBmk);
      log__("|%s|\n", s1);
    system (s1);
  }



  L_exit:
  return 0;

}
*/

//================================================================
  int get_fnTxt (char *fnTxt) {
//================================================================
// get bookmarkText  out of file ~/.vim/bookmarks/vi-bookmarks.dat
// Input:
//   sessNr

  char   tfNam[256];
  FILE   *fpi;

  sprintf(tfNam, "%s%svi-bookmarks_txtl_%d", getenv("HOME"), mfDir, sessNr);
    log__(" tfNam = |%s|", tfNam);


  // open bfNam for read
  if((fpi = fopen (tfNam, "r")) == NULL) {
    log__("vi-bookmarks-exe: ERR-3 vi-bookmarks.dat does not exist ..");
    return -1;
  }

  fgets (fnTxt, 256, fpi);
  UTX_CleanCR (fnTxt);
  fclose (fpi);

  return 0;

}


//================================================================
  int get_bfNam (char *bfNam) {
//================================================================
// get filename of the active bookmarkfilename

  int    irc, lNr;
  char   s1[256], fnfn[256], sNr[8];

  sprintf(sNr, "%d", sessNr);
  lNr = strlen(sNr);

  log__("get_bfNam |%s|", sNr);

  // get line nr <sessNr> out of file vi-bookmarks_fList
  if((fpLst = fopen (fnLst, "r")) == NULL) {
    log__("vi-bookmarks-exe: ERROR get_bfNam 1");
    irc = -1;
    goto L_exit;
  }

  // get s1 = fn
  while (!feof (fpLst)) {
    if(fgets (s1, 256, fpLst) == NULL) break;
    if(strncmp(s1, sNr, lNr)) continue;
    UTX_CleanCR (s1);
    break;
  }

  fclose (fpLst);


  // get full-fn
  ++lNr;
  sprintf(bfNam, "%s%s%s", getenv("HOME"), mfDir, &s1[lNr]);


  //// write fNam into file vi-bookmarks_fnam
  //irc = marks_fnam_wr (bfNam);


  L_exit:
    log__(" ex-get_bfNam irc=%d |%s|",irc,bfNam);
  return irc;

}
 

//================================================================
  int get_fnBkm (char *fnBmk) {
//================================================================
// get fnBmk = bookmarkFilename
// Input:
//   mfNam
// change leading '.' -> '_'
// change all '/' -> '-'

  char   *p0, *p1, bfNam[256], s1[256];
  FILE   *fpi;


  log__("get_fnBkm |%s|", mfNam);


  strcpy(s1, mfNam);
  p0 = s1;

  // skip primary '/'
  if(p0[0] == '/') ++p0;

  // change all '.' -> '_'
  while(p1 = strchr(p0, '.')) *p1 = '_';

  // change all '/' -> '-'
  while(p1 = strchr(p0, '/')) *p1 = '-';

  sprintf(fnBmk, "%s%s%s_%d", getenv("HOME"), mfDir, p0, sessNr);
    // log__("ex-get_fnBkm fnBmk = |%s|",fnBmk);

  return 0;

}


//================================================================
  void log__ (char* sFmt, ...) {
//================================================================
  char    s1[256], s2[256];
  va_list va;

  va_start(va,sFmt);
  vsprintf (s1, sFmt, va);
  va_end (va);
    // printf("log %s\n",s1);


  // add into file ~/.vim/vim.out
  sprintf(s2,"echo \"%s\" >> ~/.vim/vim.out",s1);
    // printf("log__ %s\n",s2);
  system (s2);

}

//===========================================================
  char* UTX_CleanCR (char* string) {
//===========================================================
/// \code
/// UTX_CleanCR              Delete CR's and LF's at end of string
/// returns positon of stringterminator \0
/// 
/// see also UTX_del_follBl UTX_CleanSC
/// \endcode


  int  ilen;
  char *tpos;

  ilen = strlen (string);

  tpos = &string[ilen];

  if(ilen < 1) goto L_exit;

  --tpos;


  while ((*tpos  == '\t') ||          /* tab */
         (*tpos  == '\n') ||          /* newline */
         (*tpos  == '\r'))   {        /* CR */

    *tpos    = '\0';
    if(tpos <= string) goto L_exit;
    --tpos;
  }

  ++tpos;


  L_exit:
  // printf("ex UTX_CleanCR |%s|\n", string);

  return tpos;
}



//========== EOF =================================================
 
