/*   Add or delete Bookmark                           RF  2004-06-01
 franz.reiter cadcam.co.at

MODIF:
2025-10-29 update nvi RF.
2025-10-25 warning:-Wformat-overflow fixed. RF.
2025-05-23 update for UBU 22. RF.
2021-07-31 rewrite. RF.
2018-09-04 new version with vi-marks. RF.
2004-06-19 MS-Win-Version korr. RF.
2004-07-09 Created. RF.


==================================
FILES:
~/.vim
  vi-bookmarks.vim         calls executable vi-bookmarks64
  vi-bookmarks.c           compile, link -> executable vi-bookmarks64
  vim.out                  logfile (if enabled)

~/.vim/bookmarks
  vi-bookmarks_txtl        text to add or remove from file (parameter)
  <bookmarkfiles>

//================================================================
PROGRAMFLOW:
read findTxt from file ~.vim/bookmarks/vi-bookmarks_txtl
open new outFile
open actFile = file from parameter;
  read line
  compare with findTxt
  - if equal: skip line; continue
  - else copy line out to outfile;
  - at end: if findTxt was not found: add finText to file;
 rename outFile -> actFile


==================================
build:
Compile & Link Unix:
cd ~/.vim
cc -o vi-bookmarks`getconf LONG_BIT` vi-bookmarks.c
" creates vi-bookmarks32 or vi-bookmarks64

Compile & Link MS-Windows VC:
cl -o vi-bookmarks.exe vi-bookmarks.c

Tests:
cd ~/.vim/bookmarks 
rm t1
./vi-bookmarks64 t1 show
./vi-bookmarks64 t1 show
rm t1                                
echo "abc def gh" > vi-bookmarks_txtl
./vi-bookmarks64 t1 modify

*/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         // access

#include <stdarg.h>
#include <unistd.h>          ///f. access, R_OK ..


// PROTOS:
  char* UTX_CleanCR (char* string);
  int OS_checkFilExist (char* filnam, int mode);




//===================================================================
  int main (int paranz, char *params[]) {
//===================================================================
// Add or delete Bookmark in Bookmarkfile 
// Input:
//   param[1]     filename to update
//   param[2]     "show" or "modify"

  int     i1, tNr, lNr, findLen;
  char    fnIn[512], fnOut[512], s1[512], findTxt[512];
  FILE    *fpi, *fpo;


  if(paranz < 3) {
    // log__ ("ERROR paranz = %d",paranz);
    printf("**** ERROR vi-bookmarks parameters: filename, show|modify;\n");
    return -1;
  }


  sprintf(fnIn, "%s/.vim/bookmarks/%s", getenv("HOME"),params[1]);
    // printf(" fnIn = |%s|\n",fnIn);


  //----------------------------------------------------------------
  // show and modify:
  // test if outfile exists; if yes: done;
  if(OS_checkFilExist(fnIn,1) == 0) {         // 0=no, 1=yes;

    // open fnIn - write "--empty--"; done.
    if((fpo = fopen (fnIn, "w")) == NULL) {
      printf("**** ERROR vi-bookmarks cannot open %s\n",fnIn);       
      return -1;
    }
    fprintf(fpo,"--empty--\n");
    fclose(fpo);
  }

  if(!strcmp(params[2], "show")) return 0;


  //================================================================
  // modify
  // get text to find / add into findTxt in ~/.vim/bookmarks/vi-bookmarks_txtl

  sprintf(s1, "%s/.vim/bookmarks/vi-bookmarks_txtl", getenv("HOME")); 
  if((fpi = fopen (s1, "r")) == NULL) {
    printf("**** ERROR vi-bookmarks cannot open %s\n",s1);
    return -1;
  }

  if(fgets (findTxt, 500, fpi) == NULL) {
    printf("**** ERROR vi-bookmarks read _txtl\n");
    return -1;
  }
  
  fclose(fpi);

  // cut at pos 80
  if(strlen(findTxt) > 80) findTxt[80] = '\0';
  UTX_CleanCR (findTxt);              // remove following CR,Lf, ..
  findLen = strlen(findTxt);
    // printf(" findTxt |%s|\n",findTxt);
    // printf(" findLen %d\n",findLen);


  //----------------------------------------------------------------
  // open bookmarkfile for read,
  if((fpi = fopen (fnIn, "r")) == NULL) {
    printf("**** ERROR vi-bookmarks cannot open %s\n",fnIn);       
    return -1;
  }

  // open tempFile for write,
  sprintf(fnOut, "%s/.vim/bookmarks/vi-bookmarks_temp", getenv("HOME"));
  if((fpo = fopen (fnOut, "w")) == NULL) {
    printf("**** ERROR vi-bookmarks cannot open %s\n",fnOut);       
    return -1;
  }

  // read fpi, write fpo; 
  tNr = 0;
  lNr = 0;
  while (!feof (fpi)) {
    if(fgets (s1, 120, fpi) == NULL) break;
    UTX_CleanCR (s1);              // remove following CR,Lf, ..
    if(strcmp(s1, "--empty--") == 0) continue;   // skip line
    // if line already exists, delete line
    if(tNr == 0) {
      if((int)strlen(s1) == findLen) {
          // printf(" len %d %d |%s|\n",(int)strlen(s1),findLen,s1);
        if(strncmp(s1, findTxt, findLen) == 0) {
          // tag exists - remove it;
          tNr = 1;
          continue;
        }
      }
    }
    // copy existing bookmark
    fprintf(fpo,"%s\n",s1);
    ++lNr;
  }
  fclose (fpi);

  // add new tag fnTxt
  if(tNr == 0) {
      // printf(" add: %d |%s|\n",tNr,findTxt);
    // bookmark not yet exists; write
    fprintf(fpo,"%s\n", findTxt);
    ++tNr;                                                         
  } else {
      // printf(" remove: %d |%s|\n",tNr,findTxt);
    // tag removed -
    if(lNr == 0) fprintf(fpo,"--empty--\n");
  }

  remove (fnIn);

  L_exit:
    fclose (fpo);
    rename (fnOut, fnIn);
    return 0;

}


//===========================================================
  char* UTX_CleanCR (char* string) {
//===========================================================
// UTX_CleanCR              Delete CR's and LF's at end of string
// returns positon of stringterminator \0
// 
// see also UTX_del_follBl UTX_CleanSC


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


//=============================================================
  int OS_checkFilExist (char* filnam, int mode) {
//=============================================================
/// \code
/// OS_checkFilExist         check if File or Directory exists
///   filnam may not have shell-variables; see OS_osVar_eval__ 
/// mode = 0: display message sofort;
/// mode = 1: just fix returncode, no message
/// mode = 2: make Errormessage (TX_Error) if File does not exist
/// 
/// rc = 0 = Fehler; datei existiert NICHT.
/// rc = 1 = OK, Datei existiert.
/// \endcode


  char    buf[256];  


  // printf("OS_checkFilExist |%s| %d\n", filnam, mode);

  // remove following "/"
  strcpy (buf, filnam);

  // if last char of string == '/': remove it
  if(buf[strlen(buf)-1] == '/') buf[strlen(buf)-1] = '\0';


  /* Version PC: (braucht IO.h) */
  /* if ((access (buf, 0)) != 0) { */
  /* if ((access (buf, R_OK|W_OK)) != 0) { */

  if ((access (buf, R_OK)) != 0) {
    if (mode == 0) {
      printf ("*** Error OS_checkFilExist: %s does not exist\n",filnam);
      // MSG_get_1 (buf, 256, "NOEX_fil", "%s", filnam);
      // printf ("*** Error OS_checkFilExist: %s\n", buf);

    } else if (mode == 2) {
      printf ("*** Error OS_checkFilExist: %s does not exist\n",filnam);
      // MSG_err_1 ("NOEX_fil", "%s", filnam);
    }

    return 0;
  }

  // printf("ex OS_checkFilExist YES |%s| %d\n",filnam,mode);
  return 1;

}


// eof
