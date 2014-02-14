Implementation of the Micro toy language from "Crafting of a compiler with C". I did this as I particularly appreciate the historical perspective (the book is from 1991), as it's well-written, and - by necessity - not focused on the backend (optimization) which is much more fruitfully tackled after a solid understanding of basic compiler steps exists. The code is following the code in that book; however, I changed names, file organzation, read in from file as opposed to command line, some grammar details, etc.

Changed grammar details (as of the time of this typing) include:
EOF for begin compilation; not a specific command
read from file (Planned extension: file or stdin)