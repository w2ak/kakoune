#hook global InsertChar \t %{
  #exec -draft h@
#}

#def -hidden insert-bs %{
  #try %{
    #exec -draft -no-hooks h %opt{indentwidth}H <a-k>\A<space>+\Z<ret> d
  #} catch %{
    #exec <backspace>
  #}
#}

#map global insert <backspace> '<a-;>:insert-bs<ret>'

map global insert <tab> '<a-;><gt>'
map global insert <backtab> '<a-;><lt>'
