" vi-bookmarks - for documentation see ~/.vim/vi-bookmarks_README.txt
"
" MODIF:
" 2025-10-27  autocmd VimLeave added. RF.
" 2022-12-29  change split -> sview; RF.
" 2021-08-16  update bookmarks without having to save modified mainfile. RF.
" 2021-08-15  enable correct open if mainFile opened via tag (-t) RF.
" 2021-08-10  RF.
":::::::::::::::::::::::::::::::::::::::::::::::::
"initialize vi-bookmarks.vim     // enter vim
"g:mainFnam         filename of editfile
"g:marksFnam        bookMarkfilename (/home/fwork/.vim/bookmarks/t1_c_0)
"g:marksStatOpen    0 = bookMarkfilename not valid; 1 = g:marksFnam is open;
"g:primBnr          bufferNr of primary buffer
"g:marksBnr         0 = bookmark-buffer does not exist; else exists;
"g:marksStatVis     0 = bookmark-buffer is hidden; 1 = is visible;
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"Vi_marks_loadFile0      //      enter buffer
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"Vi_marks_showToggle  ("bv")                     // view | hide bookmark-window
"  Vi_marks_load__       //      write file vi-bookmarks_fnam, get as g:marksFnam
"  Vi_marks_showCreate   //      create bookmark-buffer
"  Vi_marks_showOn       //      set bookmark-buffer visible
"  Vi_marks_showOff      //      set bookmark-buffer hidden
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"Vi_marks_toggle     ("bb")               // [create BW and] add active line
"  Vi_marks_load__
"  Vi_marks_reRead       //      
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"Vi_marks_del        ("bd")              // delete all bookmarks
"
":::::::::::::::::::::::::::::::::::::::::::::::::
"Vi_marks_saveFile               
"
" g:marksFnam is full-filename; expand("%:t") gives NOT full-filename 
" - so use bufferNr ..
"=================================================
" vi-bookmarks.c   -> vi-bookmarks64              executable (Vi_marks_exe())
"
"=================================================


"echo "vi-bookmarks.vim ..."

" set bb (2 times key 'b') to add active line to bookmarks and display
"   if line already exists - delete it !
nnoremap <silent> bb :call Vi_marks_toggle()
" set bv (key b then key v) to show or hide the bookmarkFile
nnoremap <silent> bv :call Vi_marks_showToggle()
" set bd = delete all bookmarks of active file (only source-window)
nnoremap <silent> bd :call Vi_marks_del()
" bb or dd or del-key in bookmark-window deletes bookmark
" double-click or Enter-key in bookmark-window jumps to bookmark
" tab-key = change cursor between source-window / bookmark-window 
nnoremap <silent> <Tab> :call Vi_marks_winToggle()




"================ functions =====================================

"------------------------------------------------ 
" get g:marksFnam = full filename bookmarksfile
:function Vi_marks_get_marksFilnam ()

  for line in readfile($HOME . "/.vim/bookmarks/vi-bookmarks_fnam", "", 1)
    let g:marksFnam = line
  endfor

    call Vi_marks_log (" ex-Vi_marks_get_marksFilnam |".g:marksFnam."|")

:endfunction


"------------------------------------------------ 
" Vi_marks_toogle - add or delete bookmark; display bookmarkFile ("bb")
" also from BW-window with Del-key or dd
:function Vi_marks_toggle ()

  call Vi_marks_log ("========= Vi_marks_toggle-marksStatOpen=".g:marksStatOpen)
  call Vi_marks_log ("Vi_marks_toggle g:marksBnr = ".g:marksBnr)

  " if bookmarks-file not loaded: do "__MARKS_LOAD__"
  if g:marksStatOpen == '0'
    call Vi_marks_load__()
  endif

  " write active line into file ~/.vim/bookmarks/vi-bookmarks.dat
  let cmd1 = ":.write!~/.vim/bookmarks/vi-bookmarks_txtl_" . g:marksSessNr
  execute cmd1

  " start vi-bookmarks-executable with command "__MARKS__"
  call Vi_marks_exe ("__MARKS__", "_")

  " test if bookmark-buffer exists
  if g:marksBnr == '0'
   " bookmark-buffer does not exist; create bookmark-buffer and make active
    call Vi_marks_showCreate ()
    return
  endif

  " test if bookmark-window is shown
  if g:marksStatVis == '0'
   call Vi_marks_showOn ()
   return
  endif

  " if bookmark-window is active -
  let l:bNr = bufnr("%")
  call Vi_marks_log ("Vi_marks_toggle l:bNr = ".l:bNr)
  if l:bNr == g:marksBnr
    " go back into mainWindow
    execute ":wincmd w"
  endif


  " reRead bookmarks
  call Vi_marks_reRead ()

:endfunction


"------------------------------------------------ 
" Vi_marks_exe - start vi-bookmarks-executable with 3 parameters
:function Vi_marks_exe (exCmd1, exCmd2)
 " get bmExe = filename of vi-marks-exe
   " Linux -
  let bmExe = $HOME . "/.vim/vi-bookmarks" . system("getconf LONG_BIT")
  " MS-Win
  "let bmExe = $VIMRUNTIME . "\\Bookmark.exe"
  let bmExe = substitute(bmExe, "\n", "", "g")
  "echo "bmExe = |" . bmExe . "|"
  "call Vi_marks_log (bmExe)

  let cmd1 = bmExe . " " . a:exCmd1 . " " . g:marksSessNr . " " . a:exCmd2
  call Vi_marks_log ("Vi_marks_exe |".cmd1."|")
 " following line does not work wit lines starting with "//"
  "call Vi_marks_log ("cmd1=|" . cmd1 . "|")
 " fix Bookmarkfile (add or delete Bookmark-line or laod or save file)
  let irc = system(cmd1)

:endfunction


"------------------------------------------------ 
" Vi_marks_winToggle - change window
:function Vi_marks_winToggle ()
  " if only one window exit
  if g:marksStatVis == '0'
    return
  endif
  " if source-window is active
  if winnr() == 1
    execute ":wincmd j"
  else
    execute ":wincmd w"
  endif
:endfunction


"------------------------------------------------ 
" Vi_marks_showToggle - show/hide bookmarks ("bv")
:function Vi_marks_showToggle ()

  call Vi_marks_log (". . . . .")
  call Vi_marks_log ("======== Vi_marks_showToggle marksStatOpen ".g:marksStatOpen)

  " test if bookMarkfilename is valid
  if g:marksStatOpen == '0'
   " register new session (sessNr) and get bookMarkfilename g:marksFnam
   call Vi_marks_load__()
  endif

 " test if bookmark-buffer exists
  call Vi_marks_log ("_showToggle g:marksBnr = ".g:marksBnr)
  if g:marksBnr == '0'
   " bookmark-buffer does not exist; create bookmark-buffer
    call Vi_marks_showCreate()
    return
  endif

  " bookmark-buffer exists; test if active (how many active windows)
   call Vi_marks_log ("_showToggle  g:marksStatVis = ".g:marksStatVis)
   if g:marksStatVis == '0'
    " bookmark-buffer exists but is not yet active; open
     call Vi_marks_showOn ()
   else
    " bookmark-buffer is active; hide hide bookmark-window
     call Vi_marks_showOff ()
   endif

:endfunction


"------------------------------------------------ 
" Vi_marks_showCreate - create bookmark-buffer and window and make active
:function Vi_marks_showCreate ()

  "call Vi_marks_log (" Vi_marks_showCreate |".g:marksFnam."|")

  " open bookmarkfile in new window below with 8 lines
  "let cmd1 = ":8 sview " . g:marksFnam
  let cmd1 = ":8 split " . g:marksFnam
    call Vi_marks_log (" _showCreate |" . cmd1 . "|")
  execute cmd1

 " define key enter only for bookmark-window
  nnoremap <buffer> <silent> <Enter> :call Vi_marks_jump()
  nnoremap <buffer> <silent> <2-LeftMouse> :call Vi_marks_jump()
  nnoremap <buffer> <silent> <Del> :call Vi_marks_toggle()
  nnoremap <buffer> <silent> dd :call Vi_marks_toggle()
 " get g:marksBnr = nr of the bookmark-buffer
  let g:marksBnr = bufnr("%")
   call Vi_marks_log (" Vi_marks_showCreate |".g:marksBnr."| created ..")
  "execute ":setlocal syntax=off"
  execute ":setlocal nomodifiable"
  " back into source-window
  execute ":wincmd w"
 let g:marksStatVis = '1'

:endfunction


"------------------------------------------------ 
" Vi_marks_showOn - redisplay bookmark-window
:function Vi_marks_showOn ()

   "call Vi_marks_log (" Vi_marks_showOn |" . g:marksBnr . "|")

 if g:marksStatVis == '0'
   "let cmd1 =  ":8 sview | buffer " . g:marksBnr
   let cmd1 = ":8 split " . g:marksFnam
    "call Vi_marks_log (" _showOn |" . cmd1 . "|")
   execute cmd1
   "execute ":setlocal syntax=off"
   execute ":setlocal nomodifiable"
   " back into source-window
   execute ":wincmd w"
   let g:marksStatVis = '1'
 endif

:endfunction


"------------------------------------------------ 
" Vi_marks_showOff - hide bookmark-window
:function Vi_marks_showOff ()

 if g:marksStatVis == '1'
   call Vi_marks_log (" Vi_marks_showOff |" . g:marksBnr . "|")
   " goto bookmarkWindow
   execute ":wincmd j"
   " hide the bookmarkWindow
   let cmd1 =  ":quit!"
   "let cmd1 =  ":" . g:marksBnr . "quit!"
   "let cmd1 =  ":bunload! " . g:marksBnr
   execute cmd1
   let g:marksStatVis = '0'
   let g:marksBnr = '0'
 endif

:endfunction


"------------------------------------------------ 
" Vi_marks_jump - jump to bookmark-line in source-window
:function Vi_marks_jump ()
"echo "Vi_marks_jump"

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
" Vi_marks_del = delete all bookmarks of active file
:function Vi_marks_del ()
 call Vi_marks_log ("================ Vi_marks_del")
 " only if bookmarkWindow is active
 if(g:marksStatVis == 1)
   " only in source-window 
   if winnr() == 1
     " empty file (rm kills buffer/window)
     let cmd1 = "echo --empty-- > " . g:marksFnam
       call Vi_marks_log ("cmd1 = |" . cmd1 . "|")
     let irc = system(cmd1)
     " update bookmarks
     call Vi_marks_reRead()
   endif
 endif
:endfunction


"------------------------------------------------ 
" Vi_marks_reRead - reRead bookmark-buffer
:function Vi_marks_reRead ()

  call Vi_marks_log ("Vi_marks_reRead g:marksFnam |".g:marksFnam."|")
  call Vi_marks_log ("Vi_marks_reRead marksStatVis = ".g:marksStatVis)


  if g:marksStatVis == '1'

    let g:statReadBmf = 1

    execute ":wincmd j"
    execute ":edit!"
    execute ":wincmd w"

"   " goto bookmark-window, reRead
"   let cmd1 = ":b ".g:marksBnr
"   execute cmd1
"   let cmd1 = ":e ".g:marksFnam
"   execute cmd1
"   "execute ":setlocal syntax=off"
"   " back into primary window
"   let cmd1 = ":b ".g:primBnr
"   execute cmd1
"
"   "let v1 = "lgetfile " . g:marksFnam
"   "execute v1
"   "lopen 6    " 6 = Nr of lines of quickfix-/errorfile

    let g:statReadBmf = 0
  endif

  call Vi_marks_log ("exit Vi_marks_reRead marksStatVis")

:endfunction


"------------------------------------------------ 
" Vi_marks_loadFile0 - new file being loaded
:function Vi_marks_loadFile0 ()

  " get name of file to be loaded
  let l:fNam = expand("%:t")
  let l:bNr = bufnr("%")
  call Vi_marks_log ("Vi_marks_loadFile0 l:fNam = |".l:fNam."|")
  call Vi_marks_log ("Vi_marks_loadFile0 l:bNr = ".l:bNr)
  call Vi_marks_log ("Vi_marks_loadFile0 g:primBnr = ".g:primBnr)

  " exit if buffer != primary-buffer
  if l:bNr != g:primBnr
    return
  endif

  let g:mainFnam = l:fNam

  " hide bmk-win if buftype is 'help'
  "call Vi_marks_log ("Vi_marks_loadFile0 buftype = |" . &buftype . "|")
  if &buftype == "help"
   " if bmk-window is active: hide.
   call Vi_marks_showOff ()
  endif

:endfunction


"------------------------------------------------ 
" Vi_marks_load - new file to be used; called when bmk-win comes up
:function Vi_marks_load__ ()

  "let l:fNam = expand("%:t")
  let l:bNr = bufnr("%")
  call Vi_marks_log ("=========== Vi_marks_load__ l:bNr ".l:bNr."")
  call Vi_marks_log ("  Vi_marks_load__ g:mainFnam = |".g:mainFnam."|")

  " exit if buffer != primary-buffer
  "if l:fNam != g:mainFnam
  if l:bNr != g:primBnr
    return
  endif

  call Vi_marks_exe ("__MARKS_LOAD__", g:mainFnam)

  " get g:marksFnam = bookMarkfilename
  call Vi_marks_get_marksFilnam ()

  let g:marksStatOpen = '1'

:endfunction


"------------------------------------------------ 
" Vi_marks_saveFile - file beeing unloaded
:function Vi_marks_saveFile ()

  let l:bNr = bufnr("%")
  call Vi_marks_log ("=========== Vi_marks_saveFile l:bNr = |".l:bNr."|")
  call Vi_marks_log ("Vi_marks_saveFile g:mainFnam = |".g:mainFnam."|")

  " do nothing while reLoad bookmark-buffer
  if g:statReadBmf == 1
    return
  endif

  " exit if no mainFile yet
  if g:mainFnam == ""
    return
  endif

 let l:fNam = expand("%:t")
 call Vi_marks_log ("Vi_marks_saveFile fNam = |".l:fNam."|")
 call Vi_marks_log ("Vi_marks_saveFile g:marksFnam = |".g:marksFnam."|")
 call Vi_marks_log ("Vi_marks_saveFile g:primBnr = |".g:primBnr."|")
 call Vi_marks_log ("Vi_marks_saveFile g:marksBnr = |".g:marksBnr."|")
 call Vi_marks_log ("Vi_marks_saveFile marksStatVis = ".g:marksStatVis)
 call Vi_marks_log ("Vi_marks_saveFile marksStatOpen = ".g:marksStatOpen)
 call Vi_marks_log ("Vi_marks_saveFile statReadBmf = ".g:statReadBmf)

  " for q in BM-window
  if l:bNr == g:marksBnr
    " bookmarkWindow is active; close
    call Vi_marks_showOff ()
    return
  endif

  "if l:fNam == g:mainFnam
  if l:bNr == g:primBnr
    " closing primary-buffer
    if g:marksStatOpen == '1'
      " close bookmarkFile (remove from file vi-bookmarks_fList)
      call Vi_marks_exe ("__MARKS_SAVE__",  "_")
      " close bookmarkWindow
      call Vi_marks_showOff ()
      let g:marksStatOpen = '0'
    endif
    return
  endif

:endfunction


"------------------------------------------------ 
"" exit vim
":function Vi_marks_leave ()
"  call Vi_marks_log ("Vi_marks_leave")
":endfunction


"------------------------------------------------ 
:function Vi_marks_log (msg)
  " remove following line for activation
  "return
  let irc = system("echo \"" . a:msg . " \" >> $HOME/.vim/vim.out")
:endfunction



"================ INIT =====================================
" define the directory for the bookmark-files
let g:bmDir = "${HOME}/.vim/bookmarks/"

" create ~/.vim/bookmarks/ if necessary
let irc = system("mkdir -p " . g:bmDir)

" kill test-outputfile
let cmd1 = "rm -f " . g:bmDir . "vim.out"
let cmd1 = "rm -f ${HOME}/.vim/vim.out"
let irc = system(cmd1)

call Vi_marks_log ("=========== vi-bookmarks.vim bmDir = |".g:bmDir."|")

let g:mainFnam = ""
let g:marksFnam = ""
let g:marksBnr = '0'
let g:marksStatOpen = '0'
let g:marksStatVis = '0'
let g:primBnr = bufnr("%")
call Vi_marks_log ("  vi-bookmarks.vim g:primBnr = ".g:primBnr)

"" get g:marksSessNr = number of active vim-processes
"let g:marksSessNr = system("ps -C vim --no-heading -o pid | wc -l")

let g:marksSessNr = system("echo $RANDOM")
" remove the following \n
let g:marksSessNr = substitute(g:marksSessNr, "\n", "", "")
call Vi_marks_log ("  vi-bookmarks.vim g:marksSessNr = ".g:marksSessNr)


let g:statReadBmf = 0

" remove the following \n
"let g:marksFnam = substitute(g:marksFnam, "\n", "", "")

" exit vi - remove split-buffer
:autocmd BufReadPre * call Vi_marks_loadFile0()
:autocmd BufUnload * call Vi_marks_saveFile()
:autocmd VimLeave * call Vi_marks_leave()

" inhibit message bookmark-window-content changed
:set autoread

" use also for help-window
:set splitbelow
" do not substitute blanks by spaces for :.write!~/.vim/bookmarks/vi-bookmarks.dat
:retab

" EOF
