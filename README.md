# chessy
A mid-level chess engine under development (Linux only). Its ELO rating ~1800, which is just below a Candidate Master. It currently has just enough of the XBoard protocol implemented to use the xboard gui. 

## Use with XBoard
To use it, compile the code and then open xboard, select engine -> load first engine -> engine command and select the 'chessy' program compiled earlier. It may take a few moments to initialize as it uses moderetaly large transposition table caching. The time each move takes is currently set to 10s - there isn't enough of the XBoard protocol at the moment to change this the GUI.

## TODO:
- Add an opening book
- Add better support for xboard protocol
- Improve efficiency (currently gets to a depth of around 7 ply in 10s on a 4GB machine)
