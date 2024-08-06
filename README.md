# cnc-pendant-firmware

Arduino CNC pendant for UGS

## Overview

Create a handy CNC pendant for UGS running on an Arduino Pro Micro.

### BOM

- Arduino Pro Micro (5V, 16MHz)
- KY-040 Rotary encoder
- 5 Push buttons (self-locking)
- SSD1306 0.96 inch OLED display (I2C)

### Connections

- Button X       Pin 4
- Button Y       Pin 5
- Button Z       Pin 6
- Button RANGE   Pin 10
- Button STOP    Pin 16
- Encoder Switch Pin 7
- Encoder A      Pin 8
- Encoder B      Pin 9
- SSD1306 SDA    Pin 2
- SSD1306 SCL    Pin 3

### Hotkeys

When executing an action on the pendant it will generate a hotkey-press to interface with UGS, the following is a list of these actions/hotkeys:

**Single press**

- Button X       - _none_ - Change axis to X
- Button Y       - _none_ - Change axis to Y
- Button Z       - _none_ - Change axis to Z
- Button RANGE   - CTRL+SHIFT+_n_ - Change step size (0.001 mm, 0.01 mm, 0.1 mm, 1 mm, 10 mm), _n_ is numbers 1 to 5
- Button STOP    - CTRL+SHIFT+S - Stop
- Encoder Switch - CTRL+SHIFT+U - Unlock

**Long press**

- Button X       - CTRL+SHIFT+X - Reset X zero
- Button Y       - CTRL+SHIFT+Y - Reset Y zero
- Button Z       - CTRL+SHIFT+Z - Reset Z zero
- Button RANGE   - CTRL+SHIFT+H - Return to XY zero
- Button STOP    - CTRL+SHIFT+R - Soft reset
- Encoder Switch - CTRL+SHIFT+A - Reset XYZ zero

**Encoder rotation**

- Clock-wise         - For axis X LEFT  - For axis Y DOWN - For axis Z SHIFT + DOWN
- Counter-clock-wise - For axis X RIGHT - For axis Y UP   - For axis Z SHIFT + UP

## Licensing

This software is released under the MIT license.

Copyright © 2024 biohzrdmx

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Credits

**Lead coder:** biohzrdmx [github.com/biohzrdmx](http://github.com/biohzrdmx)
