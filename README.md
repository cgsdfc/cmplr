This is the **cmplr** project, my first attempt to writing a Not-A-Toy 
C compiler.

How to build and run:

The build system is cmake. Just "mkdir build;cd build;cmake ..&&make" 
is enough to give you all the binary to play with. You will see 2 subdirs
under the build/ : tknzr/ and parser/ , each of which contains one 
static library and one excutable.

The following document will focus on the tknzr part, which is a DFA-based
tokenizer.
The current implementation uses a definitive finite autometa whose states 
and transitions are hard coded in a half-auto way, which is explained as:
the core of a dfa is 3 things: states, condition, and action. As a tokenizer,
condition is essentially a boolean expression: 

(char in char\_class && !rev || !char in char\_class && rev)

An input char is an element of a set of char or it isn't.

when the dfa is running, it is working its way in a loop in fact.

1. fetch a char from input.

2. lookup the current state and that char in a table to figure out
an entry, which records the next state, the condition to match and 
the action to take.

3. if step 2 failed to find one and only one next state to goto given
the current state and input char, a token-level error is detected, which is
normally hard to recover from, like an empty char literal (''), unclosed
string literal or terrible suffix for integer literal (123LD), so the tokenizer
simply complains and gives up.

4. if step 2 succeeded, the specific action will be taken, such as filling a buffer
with that char, or accept a particular kind of token, like identifier or string.
or put that char back into the input, and the state variable is updated.

5. step 1-4 run until an error breaks out or EOF is hitted. if EOF is hitted when
state is not INIT, that indicates premature end of input, another name of error.

more details about the tokenizer:

All the tokens produced so far are buffered. Every call to get\_token will trigger
get\_next\_token only when the buffer was consumed and became empty. 

The construction of the dfa uses something like DSL: take the construction of the
part of dfa that recognises identifier for an example: one first define some local
variables to keep the state number allocated; still some variables to keep the char\_class
number which is also allocated. Then these states and char\_class conditions are combined
with action enum and added to the dfa table with function calls. The shared states are 
made global variables.

The diagram of dfa depicted roughly:
  
  begin -[word]-> loop -[Word]-> loop -[!Word]-> end

where word is [\_a-zA-Z] and Word is [0-9\_a-zA-Z].

Error handling:

The error handling is via putting at the end of each row a special entry for error cases
to fall into. some error codes are put into the the entry, which can be later decoded to
error messages.

About testing:

Under the test/ dir there are files for each kind of token, namely identifier, string, char, int,
float, operator and punctuation. the tall.sh script at the top dir runs them all at once. the c-source/
dir contains c source files and headers which can be run by another script at the top: run-src.sh.
One can inspect the errors for each run by looking at the errlog at the top. Since the input contains 
exclusively fine tokens, one run is considered failed if the tokenizer failed to recognise them and exit
non-zero or just segment fault.

TODO:
1. install fully the error handler of all kind of tokens. Currently all the errors are treated as
empty char literal simply for simplicity.

2. test all combinations of different tokens.

3. use better algorithm to generate dfa from regular expression.
