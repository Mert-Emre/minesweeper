In this project I tried to create a clone of minesweeper game. Rules are the same with the original game and there is a hint button which suggests a safe cell that can be found by the player.
<h3>Classes In The Project:</h3>
<div><h4>Minesweeper.cpp:</h2>This class contains the main function and responsible from creating the grid and the buttons. It also connects signals and slots of these buttons and the grid. From this file row size, column size and the mine count of the game can be changed easily.
MyGrid.cpp: This class is responsible from creating the mine map, determining their location at the start of the game, assigning each cell a status(open,revealed, not revealed) and controlling user input and change the status of the cells accordingly.
When an instance of this class is created, it loads the asssets and create icons from them. When the cells are created, these icons are passed to the cell constructor(ImageButton).</div></br>
This class has 5 slots and 3 signals.
Slots are:</br></br>
<b>1) onRightClick =></b> When an image button is right clicked it sends a signal with its row and column information. This slot gets the row and column from the signal
and toggles the icon of the ImageButton that sends the signal.</br>
<b>2) onLeftClick => </b>When an image button is left clicked it sends a signal with its row and column information. This slot gets the row and column from the signal
and starts opening the cells recursively.</br>
<b>3) onRestart => </b>When restart button is clicked, this slot reactives the signals for ImageButtons(thus they are clickable again), resets their icon to empty icon, changes the status of
all cells to not_revealed and resets bomb locations.</br>
<b>4) onHint => </b>When hint button is clicked this slot first changes the color of a cell that does not contain a bomb(this location can be also detected by a human), when the
hint button is clicked again, it opens that cell recursively.</br>
<b>5) onGameFinish => </b>This slot listens its own gameWon and gameLost signals. It makes everything unclickable except the restart button and opens all of the mines.</br>
<b>Signals are:</b></br>
<b>1) gameWon => </b>When remaining unopened cell count is equal to the bomb count in the map, this signal is emitted, ongamefinish slot listens for it.</br>
<b>2) gameLost => </b>When the player clicks to a mine, this signal is emitted, ongamefinish slot listens for it.</br>
<b>3) gameStarted => </b>When game is started by clicking to a cell for the first time(after restart or after starting the app), this signal is emitted, hint button listens for it
and it becomes clickable when it gets this signal.</br>
There are 4 types of game status:</br>
<b>INIT:</b> None of the cells are opened, game can be started by left clicking to a cell.</br>
<b>PLAYING:</b> Game has already startes and it continues.</br>
<b>WIN:</b> Game is finished and it is won.</br>
<b>LOSE:</b> Game is finished and it is lost.</br></br>
<h4>ImageButton.cpp:</h4> This button is a special type of button that shows an icon on itself and the cell abstraction in the game. It also implements its own mousePressEvent slot to differentiate between right and left click to the cells. It does not contain any game logic in it.</br>
There are 3 types of cell status:</br>
<b>NOT_REVEALED:</b> The cell is not opened and it is not known if it has a mine.</br>
<b>REVEALED:</b> The cell is not opened but it is known to the player that the cell contains a mine.</br>
<b>OPEN:</b> The cell is opened.</br>
<h4>HintButton.cpp:</h4> This button is implemented because the hint button should be disabled when the game has not yet started and when the game is finished. It has 2 slots, and listens for enable and disable signals. These signals are sent from the grid.
