# EGR 227 Final Project

## Images of the Project
### Project Model in Solidworks
![Project Model](assets/imgs/model.png)

### Project In Real Life

![Project Image](assets/imgs/20251207_202755.jpg)

### Electrical Schematic
![Schematic Image](assets/imgs/schematic.png)

### State Diagram
![State Diagram](assets/FINAL-state-diagram.drawio.png)

## Project Structure

### File Tree
```
├── assets
│   ├── imgs
│   │   ├── 20251207_191956.jpg
│   │   ├── 20251207_192033.jpg
│   │   ├── 20251207_192102.jpg
│   │   ├── 20251207_192118.jpg
│   │   ├── 20251207_192130.jpg
│   │   ├── 20251207_192202.jpg
│   │   ├── 20251207_192209.jpg
│   │   ├── 20251207_192219.jpg
│   │   ├── 20251207_202755.jpg
│   │   ├── model.png
│   │   └── schematic.png
│   ├── models
│   │   ├── Assembly_all.SLDASM
│   │   ├── floor.SLDPRT
│   │   ├── floor.STL
│   │   ├── input-table.SLDPRT
│   │   ├── input-table.STL
│   │   ├── led-servo_table.SLDPRT
│   │   └── led-servo_table.STL
│   ├── FINAL-state-diagram.drawio.png
│   ├── GUI-state-diagram.drawio.png
│   ├── SIMON-state-diagram.drawio.png
│   ├── STATE_PLAYING.drawio.png
│   ├── target-game-state-diagram.drawio.png
│   └── TEST-state-diagram.drawio.png
├── schematic
│   ├── hd44780.kicad_sym
│   ├── schematic.kicad_pcb
│   ├── schematic.kicad_prl
│   ├── schematic.kicad_pro
│   └── schematic.kicad_sch
├── src
│   ├── button
│   │   ├── button.c
│   │   └── button.h
│   ├── gui
│   │   ├── gui.c
│   │   └── gui.h
│   ├── keypad
│   │   ├── keypad.c
│   │   └── keypad.h
│   ├── lcd
│   │   ├── lcd.c
│   │   └── lcd.h
│   ├── led
│   │   ├── led.c
│   │   └── led.h
│   ├── servo
│   │   ├── servo.c
│   │   └── servo.h
│   ├── simon
│   │   ├── simon.c
│   │   └── simon.h
│   ├── speaker
│   │   ├── speaker.c
│   │   └── speaker.h
│   ├── state
│   │   ├── state.c
│   │   └── state.h
│   ├── test
│   │   ├── test.c
│   │   └── test.h
│   ├── timer
│   │   ├── timer.c
│   │   └── timer.h
│   ├── hardware.h
│   ├── main.c
│   └── readme.md
├── readme.md
├── report.pdf
└── schematic.pdf
```

### Folders
- `src`: Contains all source code files for the project, including C source files and header files.
- `assets`: Contains images and state diagrams used in the project documentation.
- `schematic`: contains the KICAD schematic files for the hardware design.

### Files
- `readme.md`: This file, providing an overview of the project structure and module descriptions
- `report.pdf`: The final project report detailing the design, implementation, and testing of the project.
- `schematic.pdf`: A PDF export of the KICAD schematic for easy viewing.