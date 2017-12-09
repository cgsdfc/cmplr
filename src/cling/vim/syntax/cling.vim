" Vim Syntax File
" Langauge: Cling
" Maintainer: Cgsdfc
" Latest Revision Time: 9 December 2017

if exists('b:current_syntax')
  finish
endif
let b:current_syntax='cling'

syn keyword ClingStatement for switch return if else case default
syn keyword ClingType int char void const
syn keyword ClingSpecial printf scanf main

syn match ClingChar +'[-+*/0-9a-zA-Z]'+
syn match ClingIdentifier '[a-zA-Z_][a-zA-Z_0-9]*'
syn match ClingNumber '[-+]\?\d\+'
syn match ClingString '".*"'
syn match ClingOperator '[+-*/\[\]\{\}()]'

hi def link ClingStatement Statement
hi def link ClingType Type
hi def link ClingNumber Constant
hi def link ClingString Constant
hi def link ClingSpecial Special
