# JNote v3.1.1

Simple note application in Qt and SQLite.  Programmed by Jaeha Lee on Apr 2019.
<p>

## Features
- Full search words in entire notes
- Resize Font and dialog and it's persistent.
- Save as update note automatically.
- Attach any files by Drag and Drop into text area.
- Read other db.jnote by drag and drop
- Copy attached file by drag abd drop
<p>

## Binary Installation
For Mac, download JNote311.dmg, and copy to Application.
For Windows, Download JNote311.zip and extract into any folder.
<p>
~/Documents/JNote will be default location for the JNote data.
<p>
  
## Source Comopile
Download all the files under 'src', and open the JNote.pro from Qt Creator and compile it.  I used Qt 5.6 to create binaries.
<p>
  
## Usage
![JNote v3.1](docs/jnote_main_screen.png)

- To add new note, just click add button and start to write on right text area.  All changes will be saved automatically.
- Remove button will remove current note.
- You can also open another JNote using 'Open', but this is only avilable that session.  When close, default JNote under Documents will be used.
- Full text search on 'Find'.
- Ctrl+F will allow search in the current note.
- Attach file by Drag and Drop to text area.  if it's text file, you will have option to copy into text as well.
<p>
  
 Enjoy!!
