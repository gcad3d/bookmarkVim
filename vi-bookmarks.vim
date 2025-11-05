" vi-bookmarks - bookmark-tool for console-editor nvi (vi, vim).
" franz.reiter cadcam.co.at
"
" MODIF:
" 2025-11-05  rewrite, update for NVIM v0.6.1. RF.
" 2025-10-27  autocmd VimLeave added. RF.
" 2022-12-29  change split -> sview; RF.
" 2021-08-16  update bookmarks without having to save modified mainfile. RF.
" 2021-08-15  enable correct open if mainFile opened via tag (-t) RF.
" 2021-08-10  RF.
":::::::::::::::::::::::::::::::::::::::::::::::::
"VARIABLES:
"g:mainFnam         filename of editfile
"g:marksFnam        g:mainFnam with directory - eg /home/xy/.vim/bookmarks/xyz
"g:primWnr          bufferNr of primary buffer
"g:bookWnr          bufferNr of bookmark-bufferi; 0 = does not exist;
"g:marksStatVis     0 = bookmark-buffer is hidden; 1 = is visible;
"g:marksStatOpen    0=open-bookmarkBuffer-NOT-active; 1=open-bookmarkBuffer-active;
"g:markNewFile      1=entering-new-source; 0=not
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"FUNCTIONS:
"Vi_marks_loadFile      //      enter buffer
"
"Vi_marks_showToggle  ("bv")     view or hide bookmark-window
"  Vi_marks_showCreate   //      create bookmark-buffer
"  Vi_marks_showOn       //      set bookmark-buffer visible
"  Vi_marks_showOff      //      set bookmark-buffer hidden
"
"Vi_marks_toggle     ("bb")      [create BW and] add or remove active line
"  Vi_marks_reRead       //      
"
"Vi_marks_edit       ("be")      edit bookmarkfile; save with ":w"; return with Ctrl-O;
""
"
"Vi_marks_quit                           // closing mainWin close bookWin also
"
"=================================================
" vi-bookmarks.c   -> vi-bookmarks64              executable (Vi_marks_exe())
" build executable with:
" cc -o vi-bookmarks`getconf LONG_BIT` vi-bookmarks.c
" TEST: check logfile with gedit ~/.vim/vim.out (see Vi_marks_log and Dump_all)
"=================================================


" bb (2 times key 'b') to add active line to bookmarks and display
"   if line already exists - delete it !
nnoremap <silent> bb :call Vi_marks_toggle()

" bv (key b then key v) to show or hide the bookmarkFile
nnoremap <silent> bv :call Vi_marks_showToggle()

" be (keys b and e) edit bookmarkFile
nnoremap <silent> be :call Vi_marks_edit()


" double-click or Enter-key in bookmark-window jumps to bookmark
nnoremap <silent> bv :call Vi_marks_showToggle()

" tab-key = change cursor between source-window / bookmark-window 
nnoremap <silent> <Tab> :call Vi_marks_winToggle()



"================ functions =====================================

"------------------------------------------------ 
" edit bookmarkFile
:function Vi_marks_edit ()  
  
  call Dump_all("   _edit-in")

  if g:marksStatVis == 1
    call Vi_marks_showOff ()
  endif

  let cmd1 = ":edit ".g:marksFnam
    call Vi_marks_log ("    ".cmd1)
  execute cmd1
  execute ":setlocal ma"
  execute ":setlocal noswapfile"

  call Dump_all("        _edit-ex")

:endfunction



"------------------------------------------------ 
" enter new primaryWindow
" after Ctrl-O (return from tagFile): remove bookmarkWin;
:function Vi_marks_enterW ()

  call Dump_all("   _enterW-in")

  " if entering bmFile
  if g:marksStatOpen == 1
    if g:bookWnr == 0
      let g:bookWnr = bufnr("%")
        call Dump_all("        _enterW-ex1")
      return
    endif 
  endif 

  " after Ctrl-O (return from tagFile): remove bookmarkWin;
  if g:mainFnam != expand("%:t")
    "let g:StatFilClo = 1
    if g:primWnr != bufnr("%")
      " update g:primWnr g:mainFnam g:marksFnam
      call Vi_marks_newPrimWin ()
      " remove bookmarkWin
      if g:marksStatVis == 1
        call Vi_marks_showOff ()
      endif
    endif
      call Dump_all("        _enterW-ex2")
    return
  endif

  "let g:StatFilClo = 0
  call Dump_all("        _enterW-ex0")

:endfunction



"------------------------------------------------ 
" get vars after changing the primWin
:function Vi_marks_newPrimWin ()

  call Dump_all("   _newPrimWin-in")

  let g:primWnr = bufnr("%")
  let g:mainFnam = expand("%:t")
  "let g:marksFnam = "~/.vim/bookmarks/".g:mainFnam
  let g:marksFnam = g:bmDir.g:mainFnam

  call Dump_all("        _newWin-ex")

:endfunction


"------------------------------------------------ 
" Vi_marks_toogle - add or delete bookmark; display bookmarkFile ("bb")
:function Vi_marks_toggle ()

  call Vi_marks_log ("---------------------------------------\n")
  call Dump_all("== _toggle-in")

  " skip bb in bookWin
  if bufnr("%") == g:bookWnr
    return
  endif

  if bufnr("%") != g:primWnr
    call Vi_marks_log("XXXXXXXXXXXXXX new win !!!!!!!!!!!")
  endif

  let g:marksStatOpen = 1

  " write active line into file ~/.vim/bookmarks/vi-bookmarks.txtl
  let cmd1 = ":sil.write!".g:bmDir."vi-bookmarks_txtl"
  execute cmd1

  " add (or remove (if already exists) cmd2 in file g:mainFnam
  " start vi-bookmarks-executable with parameters (<filNam> "modify")
  call Vi_marks_exe (g:mainFnam, "modify")

  " test if bookmark-window is shown
  if g:marksStatVis == '0'
    call Vi_marks_showOn ()
    let g:marksStatOpen = 0
    call Dump_all("== _toggle-ex2")
    call Vi_marks_log ("---------------------------------------\n")
    return
  endif

  " reRead bookmarks
  call Vi_marks_reRead ()

  call Dump_all("== _toggle-ex1")
  call Vi_marks_log ("---------------------------------------\n")

:endfunction


"------------------------------------------------ 
" Vi_marks_exe - start vi-bookmarks-executable with 2 parameters
:function Vi_marks_exe (exCmd1, exCmd2)

  " fix Bookmarkfile (add or delete Bookmark-line or laod or save file)
  let cmd1 = g:bmExe." ".a:exCmd1." ".a:exCmd2
  call Vi_marks_log ("    Vi_marks_exe |".cmd1."|")
  let irc = system(cmd1)

:endfunction


"------------------------------------------------ 
" Vi_marks_winToggle - change window
:function Vi_marks_winToggle ()
  " if bookMarWin is not active -
  if g:marksStatVis == '0'
    return
  endif
  " if source-window is active
  if bufnr("%") == g:primWnr
    execute ":wincmd j"
  else
    execute ":wincmd w"
  endif
:endfunction


"------------------------------------------------ 
" Vi_marks_showToggle - show/hide bookmarks ("bv")
:function Vi_marks_showToggle ()

  call Vi_marks_log ("---------------------------------------\n")
  call Dump_all("== showToggle-in")

  if bufnr("%") != g:primWnr
    call Vi_marks_newPrimWin()
  endif

  let g:marksStatOpen = 1

  " if bookmark-buffer is active: hide bookmark-window
  if g:marksStatVis == '1'
    call Vi_marks_showOff ()
    let g:marksStatOpen = 0
    call Dump_all("== showToggle-ex2")
    call Vi_marks_log ("---------------------------------------\n")
    return
  endif 

  " start vi-bookmarks-executable with parameters (<filNam> "show")
  call Vi_marks_exe (g:mainFnam, "show")

  " activate bookmark-buffer
  if g:bookWnr == '0'
   " bookmark-buffer does not exist; create bookmark-buffer
    call Vi_marks_showCreate()
  else
    call Vi_marks_showOn()
  endif

  let g:marksStatOpen = 0

  call Dump_all("== showToggle-ex1")
  call Vi_marks_log ("---------------------------------------\n")

:endfunction


"------------------------------------------------ 
" Vi_marks_delLn_bW -  delete active line in bookWin
:function Vi_marks_delLn_bW ()

  call Dump_all("== delLn_bW-in")

  " write active line into file 
  "let cmd1 = ":sil.write!~/.vim/bookmarks/vi-bookmarks_txtl"
  let cmd1 = ":sil.write!".g:bmDir."vi-bookmarks_txtl"
  execute cmd1    

  " remove line in file txtl
  call Vi_marks_exe (g:mainFnam, "modify")

  " reload bW
  call Vi_marks_reRead ()

:endfunction


"------------------------------------------------ 
" Vi_marks_showCreate - create bookmark-buffer and window and make active
:function Vi_marks_showCreate ()

  call Dump_all("== showCreate-in")

  " ignore if bookWin has cursor
  if bufnr("%") == g:bookWnr
    return
  endif

  " open bookmarkfile in new window below with 8 lines
  "let cmd1 = ":8 sview " . g:marksFnam
  let cmd1 = ":8 split " . g:marksFnam
    call Vi_marks_log ("   _showCreate |" . cmd1 . "|")
  execute cmd1

 " define key enter only for bookmark-window
  nnoremap <buffer> <silent> <Enter> :call Vi_marks_jump()
  nnoremap <buffer> <silent> <2-LeftMouse> :call Vi_marks_jump()
  nnoremap <buffer> <silent> <Del> :call Vi_marks_delLn_bW()
  nnoremap <buffer> <silent> q :call Vi_marks_showOff()
" nnoremap <buffer> <silent> dd :call Vi_marks_toggle()

  let g:bookWnr = bufnr("%")

  " no colors in bW
  execute ":setlocal syntax=off"        
  execute ":setlocal nomodifiable"
  execute ":setlocal noswapfile"

  " back into source-window  w or k
  execute ":wincmd w"

  let g:marksStatVis = '1'

  call Dump_all("== showCreate-ex1")

:endfunction


"------------------------------------------------ 
" Vi_marks_showOn - redisplay bookmark-window
:function Vi_marks_showOn ()

  call Dump_all("== showOn-in")

 if g:marksStatVis == '0'
   "let cmd1 =  ":8 sview | buffer " . g:bookWnr
   let cmd1 = ":8 split " . g:marksFnam
    "call Vi_marks_log (" _showOn |" . cmd1 . "|")
   execute cmd1
   execute ":setlocal syntax=off"
   execute ":setlocal nomodifiable"
   execute ":setlocal noswapfile"
   " TODO: inhibit select text with mouse
   " back into source-window
   execute ":wincmd w"
   let g:marksStatVis = '1'

 endif

  call Dump_all("== showOn-ex")

:endfunction


"------------------------------------------------ 
" Vi_marks_showOff - hide bookmark-window
:function Vi_marks_showOff ()

 call Vi_marks_log (" Vi_marks_showOff vis = ".g:marksStatVis." Wnr = ".g:bookWnr)

 if g:marksStatVis == '1'
   " goto bookmarkWindow
   execute ":wincmd j"
   " hide the bookmarkWindow
"  let cmd1 =  ":quit!"
"  let cmd1 =  ":" . g:bookWnr . "quit!"
   let cmd1 =  ":bunload! " . g:bookWnr
   execute cmd1
   " goto window up
   execute ":wincmd w"
   let g:marksStatVis = '0'
   let g:bookWnr = '0'
 endif

:endfunction


"------------------------------------------------ 
" Vi_marks_jump - jump to bookmark-line in source-window
:function Vi_marks_jump ()

  call Dump_all("   _jump-in")

 " get the active line -> lTxt
  let lTxt = getline(".")

 " change all '\' -> '\\'  (Necessary for c - '\n')
  let lTxt = substitute(lTxt, '\\', '\\\', 'g')
 " change all '/' -> '\/'  (/ ends search ..)
  let lTxt = substitute(lTxt, '/', '\\/', 'g')
   "call Vi_marks_log ("jump1 |" . lTxt . "|")

 " jump into source-buffer above
  execute ":wincmd w"

 " find lTxt in source
  "call Vi_marks_log (" jump-cmd1=|" . lTxt . "|")
  execute ":set nomagic"
  let cmd1 = ":/" . lTxt
   "call Vi_marks_log ("jump2 |" . cmd1 . "|")
  execute cmd1
  execute ":set magic"

:endfunction


"------------------------------------------------ 
" Vi_marks_reRead - reRead bookmark-buffer
:function Vi_marks_reRead ()

  call Dump_all("   _reRead-in")


  if g:marksStatVis == '1'
    " goto window below
    execute ":wincmd j"
    execute ":edit!"
    " goto window below
    execute ":wincmd k"

  endif

  call Dump_all("   _reRead-ex")

:endfunction


"------------------------------------------------ 
:function Dump_all (msg)
  " comment out following line for activation
  return
  call Vi_marks_log (a:msg." StatVis ".g:marksStatVis." StatOpen = ".g:marksStatOpen." NewFile = ".g:markNewFile) 
"." StatFilClo = ".g:StatFilClo)
  call Vi_marks_log ("    mainFnam = ".g:mainFnam." fn = ".expand("%:t"))
  call Vi_marks_log ("    bufnr = ".bufnr("%")." primWnr = ".g:primWnr." bookWnr = ".g:bookWnr)
  call Vi_marks_log ("    ")
:endfunction


"------------------------------------------------ 
" Vi_marks_loadFile - new file being loaded; get g:mainFnam;
:function Vi_marks_loadFile ()

  call Dump_all("== loadFile-in")

  " skip all open of this file
  if expand("%:t") == "vi-bookmarks.vim"
    call Vi_marks_log ("      loadFile-skip bm.vim\n")
    return
  endif

  " skip open bookmarksBuffer
  if g:marksStatVis == 1
    if bufnr("%") == g:bookWnr
      call Vi_marks_log ("      loadFile-skip bW-is-open\n")
      return
    endif
  endif

  " skip open primaryBuffer
  if bufnr("%") == g:primWnr
    call Vi_marks_log ("      loadFile-skip pW-is-open\n")
    return
  endif

  " open primary-window
  if g:primWnr == '0'
    call Vi_marks_newPrimWin ()
      call Dump_all("       loadFile-ex1")
    return
  endif

  " skip open bmWin
  if g:marksStatOpen == 1
    if g:bookWnr == '0'
      call Vi_marks_log ("      loadFile-skip open bW\n")
      return
    endif
  endif


  " entering new buffer (eg from edit tagFile)
  call Vi_marks_log ("   loadFile new buf ".bufnr("%")."\n")
  let g:markNewFile = 1
  call Vi_marks_newPrimWin ()
  " close bookmarkWin
  call Vi_marks_showOff ()

  let g:markNewFile = 0
  call Dump_all("      loadFile-ex3")

:endfunction


"------------------------------------------------ 
" exit; close also bookWin
:function Vi_marks_quit ()

  call Dump_all("=== _quit-in")
  
  " skip it if in bookmarkWin;
  if g:marksStatVis == 1
    if bufnr("%") == g:bookWnr
      return
    endif
  endif

  " close bookmarkWin and exit
  call Vi_marks_showOff ()
  execute ":q"

:endfunction


"------------------------------------------------ 
" exit from alternate-mainfile, eg tagFile
:function Vi_marks_unload ()
  call Dump_all("=== unload-in")

  " ignore if leaving primWin for create bookWin
  if g:marksStatOpen == 1
    call Vi_marks_log ("      unload skip-leave-pW-or-bW\n")
    return
  endif

  " if activating new primSrc: close bookWin if exists
  if g:markNewFile == 1    " - bei Ctrl-O nicht gesetzt ..
    if g:marksStatVis == '1'
      call Vi_marks_showOff ()
    endif
    let g:markNewFile = 0
    call Dump_all("      unload-ex2\n")
    return
  endif

  call Dump_all("      unload-ex1")
:endfunction


"------------------------------------------------ 
:function Vi_marks_log (msg)
  " comment out following line for activation
  return
  let irc = system("echo \"" . a:msg . " \" >> $HOME/.vim/vim.out")
:endfunction



"================ INIT =====================================
" define the directory for the bookmark-files
let g:bmDir = "${HOME}/.vim/bookmarks/"

" create ~/.vim/bookmarks/ if necessary
let irc = system("mkdir -p " . g:bmDir)

" kill test-outputfile
let cmd1 = "rm -f ${HOME}/.vim/vim.out"
let irc = system(cmd1)

call Vi_marks_log ("=========== vi-bookmarks.vim bmDir = |".g:bmDir."|")

let g:mainFnam = ""
let g:primWnr = '0'
let g:bookWnr = '0'
let g:marksStatOpen = '0'
let g:marksStatVis = '0'
let g:markNewFile = 0
"let g:StatFilClo = 0

" get bmExe = filename of vi-marks-exe
" Linux -
let g:bmExe = "${HOME}/.vim/vi-bookmarks".system("getconf LONG_BIT")
" MS-Win
"let g:bmExe = $VIMRUNTIME . "\\Bookmark.exe"
" remove the following \n
let bmExe = substitute(g:bmExe, "\n", "", "g")

" enter new file; also enter new tagFile; not WinEnter
:autocmd BufReadPost * call Vi_marks_loadFile()

" return from (eg-tag-)file; not BufUnload
" cannot change from mainWin into bookWin ..
:autocmd BufLeave * call Vi_marks_unload()

" exit vi - remove also split-buffer; only BufWinLeave WinClosed work ..
:autocmd WinClosed * call Vi_marks_quit()

" enter new primaryWindow after Ctrl-O (return from tagfile);
:autocmd BufWinEnter * call Vi_marks_enterW()

" close boolmarkWin befor exit;
:autocmd VimLeave * call Vi_marks_quit()

" inhibit message bookmark-window-content changed
:set autoread

" use also for help-window
:set splitbelow

" do not substitute blanks by spaces for :.write!~/.vim/bookmarks/vi-bookmarks.dat
:retab

" activate testmode
":set verbose=9
" EOF
