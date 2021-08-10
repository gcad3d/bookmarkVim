# bookmarkVim
VIM-Script for Bookmarks in vim                            2021-07-31
franz.reiter@gcad3d.org



----------------------------------------------------------------------
vi-bookmarks

- can add, jump-to, remove bookmarks for the active sourcefiles
- bookmarks are stored in files in directory ~/.vim/bookmarks/
- can display / hide bookmarks
- bookmarks are displayed using a split-buffer on bottom of window
- can open same file many times

The 'jump-to-bookmark' is done by a 'find bookmark-text'
  (does not jump to a stored line-number).
Advantage: finds bookmark also if line-number has changed
  (if lines have been inserted or deleted)
Disadvantage: the bookmark-text must be long enough to be uniq.
 


----------------------------------------------------------------------
Usage:

 add bookmark for active sourceline in source-buffer:
   key 'bb'    (2 times key 'b')

 remove bookmark for active sourceline in source-buffer:
   key 'bb'    (2 times key 'b')

 remove bookmark in bookmark-buffer:
   key 'bb'    (2 times key 'b')
   or key 'dd'


 goto line of stored bookmark:
   select line in bookmark-buffer (below), key enter
   or double-click line in bookmark-buffer


 show or hide the bookmarks-section:
   key 'bv'   (key b then key v) in source-buffer
   or key q in bookmark-buffer 


 set cursor into bookmark-section or back into source-buffer:
   key  Tab


 delete all bookmarks of active file in source-buffer:
   key 'bd'  (key b then key d)




----------------------------
Installation Linux:
----------------------------

# prepare the vim-directories (if not yet exists)
mkdir ~/.vim/
mkdir ~/.vim/bookmarks/

# get files vi-bookmarks.vim and vi-bookmarks.c into ~/.vim/

# change into directory ~/.vim
cd ~/.vim/

# compile the executable (creates file vi-marks64 or vi-marks32)
cc -o vi-bookmarks`getconf LONG_BIT` vi-bookmarks.c
# vi-bookmarks.vim uses this executable to add/delete in the bookmark-files


# now find the active .vimrc - usually ~/.vimrc ($HOME/.vimrc)
# in vim do :scriptnames to find the vimrc


# add the following lines into the active .vimrc:
if filereadable(expand("~/.vim/vi-bookmarks.vim"))
:source ~/.vim/vi-bookmarks.vim
endif

# must be active for bookmarkVim -
# (enabling to change the cursorposition with the mouse) -
:set mouse=n


"-------------------------------------------------------------------
# test it:
vi $HOME/.vimrc
# key b and then b - the active sourceline is displayed below
# go down some lines, key again bb - this lines is also displayed below
# select line 1 in bookmark-buffer below; key enter; first sourcelines comes up.
# key b and v - bookmark-buffer is hidden.
# key q exits. See Bookmark-files:
ls -l $HOME/.vim/bookmarks/



----------------------------
Installation MS-Windows:
----------------------------
TODO ..


----------------------------------------------------------------------
Files:
vi-bookmarks.tar
vi-bookmarks.vim               vim-script
vi-bookmarks.c                 executable-source
vi-bookmarks_README.txt        this file


----------------------------------------------------------------------
How it works: see file vi-bookmarks.c "PROGRAMFLOW"


-------------------------------------------------------------------------
Send bug-reports, suggestions for improvement to franz.reiter@gcad3d.org
-------------------------------------------------------------------------
EOF
